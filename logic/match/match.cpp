#include "match.h"

api::iCore * g_core = nullptr;
iEvent * g_event = nullptr;
iNoder * g_noder = nullptr;
iIdmanager * g_idmanager = nullptr;
iNoderSession * g_relation = nullptr;

std::map<s64, oWaiterInfo> g_account_logic_map;
std::unordered_map<s64, oChallenge *> g_challenge_map;
std::unordered_map<s64, oChallenge *> g_account_challenge_map;
tlib::tpool<oChallenge> g_challenge_pool;

static oWaiterInfo static_waiter;
static match * static_self = nullptr;

bool match::initialize(api::iCore * core) {
    g_core = core;
    static_self = this;
    return true;
}

bool match::launch(api::iCore * core) {
    g_event = (iEvent *)core->findModule("event");
    g_noder = (iNoder *)core->findModule("noder");
    g_idmanager = (iIdmanager *)core->findModule("idmanager");

    register_node_ev(g_noder, eNoderEvent::noder_report, match::onNoderReport);
    register_node_ev(g_noder, eNoderEvent::noder_dropout, match::onNoderDropout);

    register_noder_c_proto(g_noder, eCProtoID::ClientMatchReq, match::onClientMatchReq);
    register_noder_c_proto(g_noder, eCProtoID::ClientCreateChallengeForFriendReq, match::onClientCreateChallengeForFriendReq);
    register_noder_c_proto(g_noder, eCProtoID::ClientJoinChallengeReq, match::onClientJoinChallengeReq);
    register_noder_c_proto(g_noder, eCProtoID::ClientChallengeCancelReq, match::onClientChallengeCancelReq);
    return true;
}

bool match::destroy(api::iCore * core) {
    return true;
}

void match::onTimer(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {
    switch (id) {
    case timer::id::insert_ai: {
        s64 aiid = g_idmanager->create();
        trace(g_core, "account %lld from gate %d match with ai %lld", static_waiter.account, static_waiter.gateid, aiid);
        iNoderSession * logicer = g_noder->getLoadlessNoder("logicer");
        if (logicer) {
            static_waiter.logicid = logicer->getid();
            g_account_logic_map.insert(make_pair(static_waiter.account, static_waiter));

            oTellNoderMatchSuccessd ms;
            ms.set_logicid(logicer->getid());

            oMatcher * matcher = ms.add_matcher();
            matcher->set_account(static_waiter.account);
            matcher->set_gateid(static_waiter.gateid);
            matcher->set_isai(false);

            matcher = ms.add_matcher();
            matcher->set_account(aiid);
            matcher->set_isai(true);

            logicer->sendproto(eNProtoID::TellNoderMatchSuccessd, ms);

            g_noder->sendProtoToNoder(static_waiter.gateid, "gate", eNProtoID::TellNoderMatchSuccessd, ms);
            static_waiter.clear();
        }
        break;
    }
    case timer::id::challenge_recover: {
        auto itor = g_challenge_map.find(context._context_mark);
        tassert(itor != g_challenge_map.end(), "wtf");
        if (g_challenge_map.end() == itor) {
            error(g_core, "where is challenge %lld", context._context_mark);
            return;
        }

        trace(g_core, "challenge %lld time out", itor->second->_id);

        {
            auto i = g_account_challenge_map.find(itor->second->_initiator.account);
            oServerChallengeCancelAws aws;
            aws.set_code(success);
            relay_server_msg_to_client(g_noder, i->second->_initiator.account, eSProtoID::ServerChallengeCancelAws, aws, i->second->_initiator.gateid, "gate");
            g_account_challenge_map.erase(i);
        }


        recover_to_pool(g_challenge_pool, itor->second);
        g_challenge_map.erase(itor);
        break;
    }
    }
}

void match::onNoderReport(iNoderSession * session) {
    if (session->getname() == "relation") {
        tassert(g_relation == nullptr, "wtf");
        g_relation = session;
        register_proto(session, eNProtoID::AccountRelogin, match::onAccountRelogin);
    }
    else if (session->getname() == "logicer") {
        register_proto(session, eNProtoID::TellGameOver, match::onTellGameOver);
    }
    else if (session->getname() == "gate") {
        register_proto(session, eNProtoID::AccountLogout, match::onAccountLogout);
    }
}

void match::onNoderDropout(iNoderSession * session) {
    if (session->getname() == "relation") {
        tassert(g_relation != nullptr, "wtf");
        g_relation = nullptr;
    }
}

void match::onAccountLogout(api::iCore * core, iSession * session, s64 context, const oAccountLogout & body) {
    trace(g_core, "account %lld logout", body.account());
    if (static_waiter.account == body.account() && static_waiter.gateid == ((iNoderSession *)session)->getid()) {
        static_waiter.clear();
        core->killTimer(static_self, timer::id::insert_ai);
    }
    
    auto itor = g_account_challenge_map.find(body.account());
    if (itor == g_account_challenge_map.end()) {
        return;
    }

    trace(g_core, "cancel challenge %lld", itor->second->_id);
    g_core->killTimer(static_self, timer::id::challenge_recover, itor->second->_id);
    g_challenge_map.erase(g_challenge_map.find(itor->second->_id));
    recover_to_pool(g_challenge_pool, itor->second);
    g_account_challenge_map.erase(itor);
}

void match::onAccountRelogin(api::iCore * core, iSession * session, s64 context, const oAccountRelogin & body) {
    trace(core, "account %lld relogin from gate %d", body.account(), body.gateid());
    if (body.account() == static_waiter.account) {
        tassert(g_account_logic_map.find(body.account()) == g_account_logic_map.end(), "wtf");
        static_waiter.clear();
        core->killTimer(static_self, timer::id::insert_ai);
        //start_timer(g_core, static_self, timer::id::insert_ai, timer::config::insert_ai_delay, 1, timer::config::insert_ai_delay, 0);
    } else {
        auto itor = g_account_logic_map.find(body.account());
        if (itor != g_account_logic_map.end()) {
            itor->second.gateid = body.gateid();

            g_noder->sendProtoToNoder(itor->second.logicid, "logicer", eNProtoID::AccountRelogin, body);
        }
    }

    {
        auto itor = g_account_challenge_map.find(body.account());
        if (itor != g_account_challenge_map.end()) {
            itor->second->_initiator.gateid = body.gateid();
        }
    }
}

void match::onTellGameOver(api::iCore * core, iSession * session, s64 context, const oTellGameOver & body) {
    s64 account = body.account();
    auto itor = g_account_logic_map.find(account);
    tassert(itor != g_account_logic_map.end(), "wtf");
    if (itor != g_account_logic_map.end()) {
        trace(core, "game over account %lld", account);
        g_account_logic_map.erase(itor);
    } else {
        error(core, "game over but cant find mater info account %lld on match %d", account, g_noder->getid());
    }
}

void match::onClientMatchReq(api::iCore *, iNoderSession * gate, s64 account, const oClientMatchReq & body) {
    {
        auto itor = g_account_logic_map.find(account);
        if (itor != g_account_logic_map.end()) {
            error(g_core, "account %lld already exists in g_account_logic_map", account);
            return;
        }
    }

    {
        auto itor = g_account_challenge_map.find(account);
        if (itor != g_account_challenge_map.end()) {
            error(g_core, "account %lld already exists in g_account_challenge_map", account);
            return;
        }
    }

    if (static_waiter.account == account) {
        static_waiter.clear();
        g_core->killTimer(static_self, timer::id::insert_ai);
    }

    if (static_waiter.account == define::invalid_id) {
        static_waiter.account = account;
        static_waiter.gateid = gate->getid();
        trace(g_core, "account %lld from gate %d start match", account, gate->getid());
        start_timer(g_core, static_self, timer::id::insert_ai, timer::config::insert_ai_delay, 1, timer::config::insert_ai_delay, 0);
    } else {
        g_core->killTimer(static_self, timer::id::insert_ai);

        trace(g_core, "account %lld from gate %d match with account %lld from gate %d", static_waiter.account, static_waiter.gateid, account, gate->getid());
        iNoderSession * logicer = g_noder->getLoadlessNoder("logicer");
        if (logicer) {
            static_waiter.logicid = logicer->getid();
            g_account_logic_map.insert(make_pair(static_waiter.account, static_waiter));
            g_account_logic_map.insert(make_pair(account, oWaiterInfo(account, gate->getid(), logicer->getid())));

            oTellNoderMatchSuccessd ms;
            ms.set_logicid(logicer->getid());

            oMatcher * matcher = ms.add_matcher();
            matcher->set_account(static_waiter.account);
            matcher->set_gateid(static_waiter.gateid);
            matcher->set_isai(false);

            matcher = ms.add_matcher();
            matcher->set_account(account);
            matcher->set_gateid(gate->getid());
            matcher->set_isai(false);

            logicer->sendproto(eNProtoID::TellNoderMatchSuccessd, ms);

            g_noder->sendProtoToNoder(static_waiter.gateid, "gate", eNProtoID::TellNoderMatchSuccessd, ms);
            if (gate->getid() != static_waiter.gateid) {
                gate->sendproto(eNProtoID::TellNoderMatchSuccessd, ms);
            }

            static_waiter.clear();
        }
    }
}

void match::onClientCreateChallengeForFriendReq(api::iCore *, iNoderSession * gate, s64 account, const oClientCreateChallengeForFriendReq & body) {
    {
        auto itor = g_account_logic_map.find(account);
        if (itor != g_account_logic_map.end()) {
            error(g_core, "account %lld already exists in g_account_logic_map", account);
            return;
        }
    }

    {
        auto itor = g_account_challenge_map.find(account);
        if (itor != g_account_challenge_map.end()) {
            error(g_core, "account %lld already exists in g_account_challenge_map", account);
            return;
        }
    }

    if (static_waiter.account == account) {
        error(g_core, "account %lld already in match", account);
        return;
    }

    oChallenge * challenge = create_from_pool(g_challenge_pool, g_idmanager->create());
    challenge->_initiator.account = account;
    challenge->_initiator.gateid = gate->getid();
    g_challenge_map.insert(std::make_pair(challenge->_id, challenge));
    g_account_challenge_map.insert(std::make_pair(account, challenge));

    start_timer(g_core, static_self, timer::id::challenge_recover, timer::config::challenge_recover_delay, 1, timer::config::challenge_recover_delay, challenge->_id);
    trace(g_core, "account %lld create challenge %lld from gate %d", account, challenge->_id, gate->getid());
    oServerCreateChallengeForFriendAws aws;
    aws.set_code(eError::success);
    oIdentity * oid = aws.mutable_challenge_id();
    set_oIdentity(oid, challenge->_id);

//     oRelayServerMessageToClient relay;
//     relay.set_messageid(eSProtoID::ServerCreateChallengeForFriendAws);
//     relay.add_account(account);
// 
//     char * temp = (char *)alloca(aws.ByteSize());
//     if (aws.SerializeToArray(temp, aws.ByteSize())) {
//         relay.set_body(temp, aws.ByteSize());
//         g_noder->sendProtoToNoder(gate->getid(), "gate", eNProtoID::RelayServerMessageToClient, relay);
//     }
    relay_server_msg_to_client(g_noder, account, eSProtoID::ServerCreateChallengeForFriendAws, aws, gate->getid(), "gate");
}


void match::onClientJoinChallengeReq(api::iCore *, iNoderSession * gate, s64 account, const oClientJoinChallengeReq & body) {
    debug(g_core, "match::onClientJoinChallengeReq");
    {
        auto itor = g_account_logic_map.find(account);
        if (itor != g_account_logic_map.end()) {
            error(g_core, "account %lld already exists in g_account_logic_map", account);
            return;
        }
    }

    {
        auto itor = g_account_challenge_map.find(account);
        if (itor != g_account_challenge_map.end()) {
            error(g_core, "account %lld already exists in g_account_challenge_map", account);
            return;
        }
    }

    oIdentity64 temp;
    temp._up = body.challenge_id().up();
    temp._down = body.challenge_id().down();

    auto itor = g_challenge_map.find(temp._identity);//body.matcher_id());
    if (itor == g_challenge_map.end()) {
        error(g_core, "challenge %lld is not exists", temp._identity);
        return;
    }

    if (itor->second->_initiator.account == account) {
        error(g_core, "account %lld can not challenge its self", account);
        return;
    }


    trace(g_core, "account %lld from gate %d accept challenge, match with account %lld from gate %d", itor->second->_initiator.account, itor->second->_initiator.gateid, account, gate->getid());
    iNoderSession * logicer = g_noder->getLoadlessNoder("logicer");
    if (logicer) {
        itor->second->_initiator.logicid = logicer->getid();
        g_account_logic_map.insert(make_pair(itor->second->_initiator.account, itor->second->_initiator));
        g_account_logic_map.insert(make_pair(account, oWaiterInfo(account, gate->getid(), logicer->getid())));

        oTellNoderMatchSuccessd ms;
        ms.set_logicid(logicer->getid());

        oMatcher * matcher = ms.add_matcher();
        matcher->set_account(itor->second->_initiator.account);
        matcher->set_gateid(itor->second->_initiator.gateid);
        matcher->set_isai(false);

        matcher = ms.add_matcher();
        matcher->set_account(account);
        matcher->set_gateid(gate->getid());
        matcher->set_isai(false);

        logicer->sendproto(eNProtoID::TellNoderMatchSuccessd, ms);

        g_noder->sendProtoToNoder(itor->second->_initiator.gateid, "gate", eNProtoID::TellNoderMatchSuccessd, ms);
        if (gate->getid() != itor->second->_initiator.gateid) {
            gate->sendproto(eNProtoID::TellNoderMatchSuccessd, ms);
        }
    }

    g_core->killTimer(static_self, timer::id::challenge_recover, temp._identity);
    g_account_challenge_map.erase(g_account_challenge_map.find(itor->second->_initiator.account));
    recover_to_pool(g_challenge_pool, itor->second);
    g_challenge_map.erase(itor);
}

void match::onClientChallengeCancelReq(api::iCore *, iNoderSession * gate, s64 account, const oClientChallengeCancelReq & body) {
    oServerChallengeCancelAws aws;
    aws.set_code(success);
    relay_server_msg_to_client(g_noder, account, eSProtoID::ServerChallengeCancelAws, aws, gate->getid(), "gate");

    auto itor = g_account_challenge_map.find(account);
    if (itor == g_account_challenge_map.end()) {
        return;
    }

    trace(g_core, "account %lld cancel challenge %lld", account, itor->second->_id);

    g_core->killTimer(static_self, timer::id::challenge_recover, itor->second->_id);
    g_challenge_map.erase(g_challenge_map.find(itor->second->_id));
    recover_to_pool(g_challenge_pool, itor->second);
    g_account_challenge_map.erase(itor);
}
