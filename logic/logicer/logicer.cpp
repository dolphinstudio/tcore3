#include "logicer.h"

api::iCore * g_core = nullptr;
iNoder * g_noder = nullptr;
iEvent * g_event = nullptr;

std::map<s64, ev::omatcher> g_account_info_map;
messager_pool g_messager_pool;

static logicer * self = nullptr;

bool logicer::initialize(api::iCore * core) {
    g_core = core;
    self = this;
    return true;
}

bool logicer::launch(api::iCore * core) {
    g_noder = (iNoder *)core->findModule("noder");
    g_event = (iEvent *)core->findModule("event");

    register_node_ev(g_noder, eNoderEvent::noder_report, logicer::onNoderReport);
    register_node_ev(g_noder, eNoderEvent::noder_dropout, logicer::onNoderDropout);

    set_noder_all_c_proto_callback(g_noder, logicer::onAllClientProto);
    return true;
}

bool logicer::destroy(api::iCore * core) {
    return true;
}

void logicer::tellGameOver(s64 account) {
    oTellGameOver body;
    body.set_account(account);

    {
        auto itor = g_account_info_map.find(account);
        if (g_account_info_map.end() != itor) {

            g_noder->sendProtoToNoder(itor->second.match, eNProtoID::TellGameOver, body);
            g_noder->sendProtoToNoder(itor->second.gate, eNProtoID::TellGameOver, body);

            g_account_info_map.erase(itor);
        }
    }

    {
        auto itor = g_messager_pool.find(account);
        if (g_messager_pool.end() != itor) {
            g_messager_pool.erase(itor);
        }
    }
}

void logicer::onNoderReport(iNoderSession * session) {
    if (session->getname() == "match") {
        register_proto(session, eNProtoID::TellNoderMatchSuccessd, logicer::onTellNoderMatchSuccessd);
        register_proto(session, eNProtoID::AccountRelogin, logicer::onAccountRelogin);
    }
}

void logicer::onNoderDropout(iNoderSession * session) {

}

void logicer::send(const std::set<s64> & accounts, const s16 id, const void * data, const s32 size) {
    std::set<s32> gates;
    oRelayServerMessageToClient body;
    body.set_messageid(id);
    for (auto i = accounts.begin(); i != accounts.end(); i++) {
        body.add_account(*i);

        auto itor = g_account_info_map.find(*i);
        if (itor != g_account_info_map.end()) {
            if (itor->second.gate != define::invalid_id && gates.find(itor->second.gate) == gates.end()) {
                gates.insert(itor->second.gate);
            }
        }
    }
    body.set_body(data, size);
    
    for (auto i = gates.begin(); i != gates.end(); i++) {
        g_noder->sendProtoToNoder(*i, "gate", eNProtoID::RelayServerMessageToClient, body);
    }
}

void logicer::send(const s64 account, const s16 id, const void * data, const s32 size) {
    oRelayServerMessageToClient body;
    body.add_account(account);
    body.set_messageid(id);
    body.set_body(data, size);

    auto itor = g_account_info_map.find(account);
    if (itor != g_account_info_map.end()) {
        if (itor->second.gate != define::invalid_id) {
            g_noder->sendProtoToNoder(itor->second.gate, "gate", eNProtoID::RelayServerMessageToClient, body);
        }
    }
}

void logicer::unRegisterProtocol(const s64 account, const s16 id) {
    auto itor = g_messager_pool.find(account);
    if (itor != g_messager_pool.end()) {
        auto i = itor->second.find(id);
        if (i != itor->second.end()) {
            itor->second.erase(i);
        }
    }
}

void logicer::registerProtocol(const s64 account, const s16 id, const lMessager & messager) {
    messager_pool::iterator itor = g_messager_pool.find(account);
    if (itor == g_messager_pool.end()) {
        itor = g_messager_pool.insert(make_pair(account, messager_map())).first;
    }

    messager_map::iterator i = itor->second.find(id);
    if (i != itor->second.end()) {
        tassert(false, "message %d already registed %s", id, i->second._debug.c_str());
        itor->second.erase(i);
    }
    itor->second.insert(make_pair(id, messager));
}

void logicer::onTellNoderMatchSuccessd(api::iCore * core, iSession * session, s64 context, const oTellNoderMatchSuccessd & body) {
    ev::omatch_success ev;
    ev.matchA.account = body.matcher(0).account();
    ev.matchA.is_ai = body.matcher(0).isai();
    tassert(!ev.matchA.is_ai, "wtf");
    if (ev.matchA.is_ai) {
        ev.matchA.gate = define::invalid_id;
        ev.matchA.match = define::invalid_id;
    }
    else {
        ev.matchA.gate = body.matcher(0).gateid();
        ev.matchA.match = ((iNoderSession *)session)->getid();
        tassert(g_account_info_map.find(ev.matchA.account) == g_account_info_map.end(), "wtf");
        g_account_info_map.insert(make_pair(ev.matchA.account, ev.matchA));
    }

    ev.matchB.account = body.matcher(1).account();
    ev.matchB.is_ai = body.matcher(1).isai();
    if (ev.matchB.is_ai) {
        ev.matchB.gate = define::invalid_id;
        ev.matchB.match = define::invalid_id;
        ev.matchB.copyer = ev.matchA.account;
    } else {
        ev.matchB.gate = body.matcher(1).gateid();
        ev.matchB.match = ((iNoderSession *)session)->getid();
        tassert(g_account_info_map.find(ev.matchB.account) == g_account_info_map.end(), "wtf");
        g_account_info_map.insert(make_pair(ev.matchB.account, ev.matchB));
    }
    g_event->trigger(ev::match_success, ev);
}

void logicer::onAccountRelogin(api::iCore * core, iSession * session, s64 context, const oAccountRelogin & body) {
    auto itor = g_account_info_map.find(body.account());
    if (itor != g_account_info_map.end()) {
        itor->second.gate = body.gateid();
        g_event->trigger(ev::account_relogin, body.account());
    }
}

void logicer::onAllClientProto(api::iCore *, iNoderSession *, s64 account, const oRelayClientMessageToNoder & body) {
    messager_pool::iterator itor = g_messager_pool.find(body.account());
    if (itor != g_messager_pool.end()) {
        messager_map::iterator i = itor->second.find(body.messageid());
        if (i != itor->second.end()) {
            i->second.onMessage(body.account(), body.body().c_str(), body.body().size());
        }
    }
}
