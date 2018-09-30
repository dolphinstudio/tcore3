#include "masterSession.h"
#include "idmanager.h"

void masterSession::onConnected(api::iCore * core) {
    register_proto(this, eNProtoID::Report, masterSession::onReport);
    register_proto(this, eNProtoID::NoderInited, masterSession::onNoderInited);
}

void masterSession::onDisconnect(api::iCore * core) {
    if (invalid_id != _id) {
        auto itor = g_id_session_map.find(_id);
        if (itor != g_id_session_map.end()) {
            g_id_session_map.erase(itor);
        }
    }

    auto itor = g_name_sessions_map.find(_name);
    if (itor != g_name_sessions_map.end()) {
        auto i = itor->second.find(this);
        if (i != itor->second.end()) {
            itor->second.erase(i);
        }
    }

    core->killTimer(this, timer::id::broadcast_noder_info);
    DEL this;
}

void masterSession::onConnectFailed(api::iCore * core) {

}

void masterSession::onReport(api::iCore * core, iSession * session, s64 context, const oReport & body) {
    masterSession * ms = (masterSession *)session;
    ms->_id = idmanager::GetInstance().AllocaID(body.name(), body.address().ip(), body.address().port());
    ms->_name = body.name();
    ms->_ip = body.address().ip();
    ms->_port = body.address().port();
    ms->_area = body.address().area();

    if (g_id_session_map[ms->_id] != nullptr) {
        g_id_session_map[ms->_id]->close();
        g_id_session_map[ms->_id]->_id = invalid_id;
    }

    g_id_session_map[ms->_id] = ms;
    g_name_sessions_map[ms->_name].insert(ms);

    oAssigneID assigne;
    assigne.set_id(ms->_id);
    session->sendproto(eNProtoID::AssigneID, assigne);

    trace(core, "on report %s area %d id %d %s:%d", ms->_name.c_str(), ms->_area, ms->_id, ms->_ip.c_str(), ms->_port);
}

void masterSession::onNoderInited(api::iCore * core, iSession * session, s64 context, const oNoderInited & body) {
    masterSession * ms = (masterSession *)session;
    start_timer(core, ms, timer::id::broadcast_noder_info, 0, forever, timer::config::broadcast_noder_info_interval, 0);
}

void masterSession::onTimer(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {
    switch (id) {
    case timer::id::broadcast_noder_info: {
        oBroadcastNoder body;
        body.set_id(_id);
        body.set_name(_name);

        oAddress * addr = body.mutable_address();
        addr->set_ip(_ip);
        addr->set_port(_port);
        addr->set_area(_area);

        for (auto i = g_id_session_map.begin(); i != g_id_session_map.end(); i++) {
            if (i->second != this && i->second != nullptr) {
                i->second->sendproto(eNProtoID::BroadcastNoder, body);
            }
        }
        break;
    }
    }
}
