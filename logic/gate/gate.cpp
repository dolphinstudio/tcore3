#include "gate.h"
#include "wsDelegate.h"
#include "wbServerDelegate.h"

api::iCore * g_core = nullptr;
iWebSocket * g_websocket = nullptr;
iEvent * g_event = nullptr;
iRedis * g_redis = nullptr;

gate * g_gate = nullptr;
iNoder * g_noder = nullptr;
iNoderSession * g_relation = nullptr;
iNoderSession * g_match = nullptr;

messager_pool g_messager_pool;

bool gate::initialize(api::iCore * core) {
    g_core = core;
    g_gate = this;
    return true;
}

bool gate::launch(api::iCore * core) {
    g_websocket = (iWebSocket *)core->findModule("webSocket");
    g_event = (iEvent *)core->findModule("event");
    g_noder = (iNoder *)core->findModule("noder");
    g_redis = (iRedis *)core->findModule("redis");

    register_node_ev(g_noder, eNoderEvent::noder_report, gate::onNoderReport);
    register_node_ev(g_noder, eNoderEvent::noder_dropout, gate::onNoderDropout);
    register_event(g_event, ev::login_success, gate::on_login_success);

    s32 port = tools::stringAsInt(core->getArgs("gameport"));
    g_websocket->launchwebSocketServer(NEW wbServerDelegate, "0.0.0.0", port);
    return true;
}

bool gate::destroy(api::iCore * core) {
    return true;
}

void gate::unRegisterProtocol(const s64 account, const s16 id) {
    auto itor = g_messager_pool.find(account);
    if (itor != g_messager_pool.end()) {
        auto i = itor->second.find(id);
        if (i != itor->second.end()) {
            itor->second.erase(i);
        }
    }
}

bool gate::pushMessage(wsDelegate * delegate, const s16 id, const void * data, const s32 size) {
    auto itor = g_messager_pool.find(delegate->getid());
    if (itor == g_messager_pool.end()) {
        return false;
    }

    auto i = itor->second.find(id);
    if (i == itor->second.end()) {
        return false;
    }

    i->second.onMessage(delegate, id, (const char *)data, size);
    return true;
}

void gate::send(const s64 account, const void * data, const s32 size) {
    auto i = g_delegate_map.find(account);
    if (i != g_delegate_map.end()) {
        i->second->send(data, size, eFrameType::binary_frame);
    }
}

bool gate::accountLoginReport(wsDelegate * delegate) {
    if (nullptr == g_relation) {
        return false;
    }

    auto itor = g_delegate_map.find(delegate->getid());
    if (g_delegate_map.end() != itor) {
        accountLogoutReport(delegate);
        delegate->close();
    } else {
        oAccountReport body;
        body.set_gateid(g_noder->getid());
        body.set_account(delegate->getid());
        g_relation->sendproto(eNProtoID::AccountReport, body);
    }

    g_delegate_map.insert(std::make_pair(delegate->getid(), delegate));
    return true;
}

void gate::accountLogoutReport(wsDelegate * delegate) {
    {
        messager_pool::iterator itor = g_messager_pool.find(delegate->getid());
        if (itor != g_messager_pool.end()) {
            g_messager_pool.erase(itor);
        }
    }

    {
        auto itor = g_delegate_map.find(delegate->getid());
        if (g_delegate_map.end() != itor) {
            itor->second->close();
            g_delegate_map.erase(itor);
        }

        if (delegate->getid() != define::invalid_id) {
            oAccountLogout logout;
            logout.set_account(delegate->getid());
            g_noder->sendProtoToNoder("match", eNProtoID::AccountLogout, logout);
            g_noder->sendProtoToNoder("relation", eNProtoID::AccountLogout, logout);
        }

        delegate->setid(define::invalid_id);
    }
}

void gate::registerProtocol(const s64 account, const s16 id, const messager & messager) {
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

void gate::onNoderReport(iNoderSession * session) {
    if (std::string("relation") == session->getname()) {
        trace(g_core, "relation id %d %s:%d report", session->getid(), session->getip().c_str(), session->getport());
        g_relation = session;
        register_proto(session, eNProtoID::AccountRelogin, gate::onAccountRelogin);
    }

    if (std::string("match") == session->getname()) {
        trace(g_core, "relation id %d %s:%d report", session->getid(), session->getip().c_str(), session->getport());
        g_match = session;
        register_proto(session, eNProtoID::TellNoderMatchSuccessd, gate::onTellNoderMatchSuccessd);
    }

    if (std::string("logicer") == session->getname()) {
        register_proto(session, eNProtoID::TellGameOver, gate::onTellGameOver);
    }

    register_proto(session, eNProtoID::RelayServerMessageToClient, gate::onRelayServerMessageToClient);
}

void gate::onNoderDropout(iNoderSession * session) {
    if (std::string("relation") == session->getname()) {
        trace(g_core, "relation id %d %s:%d dropout", session->getid(), session->getip().c_str(), session->getport());
        g_relation = nullptr;
        return;
    }

    if (std::string("match") == session->getname()) {
        trace(g_core, "relation id %d %s:%d report", session->getid(), session->getip().c_str(), session->getport());
        g_match = nullptr;
        return;
    }
}

void gate::onAccountRelogin(api::iCore * core, iSession * session, s64 context, const oAccountRelogin & body) {
    auto itor = g_delegate_map.find(body.account());
    if (itor != g_delegate_map.end()) {
        trace(core, "account %lld relogin from gate %d, so kick it", body.account(), body.gateid());
        itor->second->close();
    }
}

void gate::onTellGameOver(api::iCore * core, iSession * session, s64 context, const oTellGameOver & body) {
    auto itor = g_delegate_map.find(body.account());
    if (itor != g_delegate_map.end()) {
        trace(core, "account %lld game over", body.account());
        itor->second->setLogicNoderID(define::invalid_id);
    }
}

void gate::onRelayServerMessageToClient(api::iCore * core, iSession * session, s64 context, const oRelayServerMessageToClient & body) {
    for (s32 i = 0; i < body.account_size(); i++) {
        s64 account = body.account(i);

        auto itor = g_delegate_map.find(account);
        if (itor != g_delegate_map.end()) {
            if (body.messageid() == eSProtoID::ServerResumeGamePush) {
                itor->second->setLogicNoderID(((iNoderSession *)session)->getid());
            }
            itor->second->sendproto(body.messageid(), body.body().c_str(), body.body().size());
        }
    }
}

void gate::onTellNoderMatchSuccessd(api::iCore * core, iSession * session, s64 context, const oTellNoderMatchSuccessd & body) {
    for (s32 i = 0; i < body.matcher_size(); i++) {
        auto itor = g_delegate_map.find(body.matcher(i).account());
        if (itor != g_delegate_map.end()) {
            itor->second->setLogicNoderID(body.logicid());
        }
    }
}

void gate::on_login_success(api::iCore * core, const ev::ologin_success & body) {
    register_protocol(((iGate *)g_gate), body.deg->getid(), eCProtoID::ClientMatchReq, gate::onClientMessageToMatchReq);
    register_protocol(((iGate *)g_gate), body.deg->getid(), eCProtoID::ClientCreateChallengeForFriendReq, gate::onClientMessageToMatchReq);
    register_protocol(((iGate *)g_gate), body.deg->getid(), eCProtoID::ClientJoinChallengeReq, gate::onClientMessageToMatchReq);
    register_protocol(((iGate *)g_gate), body.deg->getid(), eCProtoID::ClientChallengeCancelReq, gate::onClientMessageToMatchReq);
}

void gate::onClientMessageToMatchReq(iDelegate * delegate, const s16 msgid, const void * data, const s32 size) {
    oServerMatchAws aws;
    if (g_match) {
        oRelayClientMessageToNoder body;
        body.set_account(delegate->getid());
        body.set_messageid(msgid);
        body.set_body(data, size);
        g_noder->sendProtoToNoder(g_match->getid(), eNProtoID::RelayClientMessageToNoder, body);
    } else {
        error(g_core, "where is match");
    }
}
