#include "noderSession.h"

void noderReport(noderSession * session, const char * file, const s32 line) {
    if (g_id_session_map[session->getid()] != nullptr) {
        noder_dropout(g_id_session_map[session->getid()]);
    }

    g_id_session_map[session->getid()] = session;
    g_name_id_sessions_map[session->getname()].insert(std::make_pair(session->getid(), session));
    trace(g_core, "noder report to %s area %d id %d %s:%d\n", session->getname().c_str(), session->getarea(), session->getid(), session->getip().c_str(), session->getport());

    g_noder_load_sort_pool_map[session->getname()].push_front(session);

    PushNoderEvent(eNoderEvent::noder_report, session);
}

void noderDropout(noderSession * session, const char * file, const s32 line) {
    if (invalid_id != session->getid()) {
        auto itor = g_noder_load_sort_pool_map.find(session->getname());
        tassert(itor != g_noder_load_sort_pool_map.end(), "wtf");
        if (g_noder_load_sort_pool_map.end() != itor) {
            itor->second.remove(session);
        }

        PushNoderEvent(eNoderEvent::noder_dropout, session);
        {
            auto itor = g_id_session_map.find(session->getid());
            if (itor != g_id_session_map.end()) {
                g_id_session_map.erase(itor);
            } else {
                error(g_core, "g_id_session_map manage logic error, where is noder %s %d", session->getname().c_str(), session->getid());
            }
        }

        {
            auto itor = g_name_id_sessions_map.find(session->getname());
            tassert(g_name_id_sessions_map.end() != itor, "wtf");
            if (itor != g_name_id_sessions_map.end()) {
                auto i = itor->second.find(session->getid());
                if (i != itor->second.end()) {
                    itor->second.erase(i);
                } else {
                    error(g_core, "g_name_id_sessions_map manage logic error, where is noder %s %d", session->getname().c_str(), session->getid());
                }
            } else {
                error(g_core, "g_name_id_sessions_map manage logic error, where is noder %s %d", session->getname().c_str(), session->getid());
            }
        }

        trace(g_core, "noderDropout to %s area %d id %d %s:%d\n", session->getname().c_str(), session->getarea(), session->getid(), session->getip().c_str(), session->getport());
    }

}

void noderSession::onConnected(api::iCore * core) {
    if (this->_initiative) {
        noder_report(this);

        oReport body;
        body.set_name(g_noder_name);
        body.set_id(g_noder_id);
        oAddress * address = body.mutable_address();
        address->set_ip(g_noder_ip);
        address->set_port(g_noder_port);
        address->set_area(g_noder_area);
        sendproto(eNProtoID::Report, body);
    } else {
        register_proto(this, eNProtoID::Report, noderSession::onReport);
    }

    register_proto(this, eNProtoID::LoadReport, noderSession::onLoadReport);
    register_proto(this, eNProtoID::RelayClientMessageToNoder, noderSession::onRelayClientMessageToNoder);
}

void noderSession::onDisconnect(api::iCore * core) {
    noder_dropout(this);
    DEL this;
}

void noderSession::onConnectFailed(api::iCore * core) {

}

void noderSession::onReport(api::iCore * core, iSession * session, s64 context, const oReport & body) {
    noderSession * ns = (noderSession *)session;
    ns->_name = body.name();
    ns->_id = body.id();
    ns->_ip = body.address().ip();
    ns->_port = body.address().port();
    ns->_area = body.address().area();

    noder_report(ns);
}

void noderSession::onLoadReport(api::iCore * core, iSession * session, s64 context, const oLoadReport & body) {
    noderSession * ns = (noderSession *)session;
    auto itor = g_noder_load_sort_pool_map.find(ns->getname());
    tassert(itor != g_noder_load_sort_pool_map.end(), "wtf");
    if (g_noder_load_sort_pool_map.end() != itor) {
        itor->second.set_load(ns, body.load());
    }
}

void noderSession::onRelayClientMessageToNoder(api::iCore * core, iSession * session, s64 context, const oRelayClientMessageToNoder & body) {
    auto itor = g_noder_c_protoer_map.find(body.messageid());
    if (g_noder_c_protoer_map.end() != itor) {
        itor->second.onMessage(g_core, (iNoderSession *)session, body.account(), body.body().c_str(), body.body().size());
    }
    if (g_all_client_proto_call) {
        g_all_client_proto_call(core, (iNoderSession *)session, context, body);
    }
}

