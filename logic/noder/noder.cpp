#include "noder.h"
#include "noderSession.h"

target_set g_target_set;

std::string g_master_ip;
s32 g_master_port;

std::string g_noder_name;
std::string g_noder_ip;
s32 g_noder_port;
s32 g_noder_id = invalid_id;
s32 g_noder_area = invalid_id;
bool g_is_all_area = false;

s32 g_send_pipe_size = 64;
s32 g_recv_pipe_size = 64;

id_session_map g_id_session_map;
name_id_session_map g_name_id_sessions_map;
std::map<s16, iProtoer<iNoderSession>> g_noder_c_protoer_map;

api::iCore * g_core = nullptr;
iIDManager * g_idmanager = nullptr;

std::list<oNoderEvent> g_noder_events[eNoderEvent::count];
noder_load_sort_pool_map g_noder_load_sort_pool_map;

fAllClientProtoCallback g_all_client_proto_call = nullptr;

static noder * static_self = nullptr;

bool noder::initialize(api::iCore * core) {
    static_self = this;
    g_core = core;

    std::string path = tools::file::getApppath();
    path += "/server_config/noder.xml";
    g_noder_name = core->getCorename();

    TiXmlDocument configpath;
    if (false == configpath.LoadFile(path.c_str())) {
        tassert(false, "where is noder.xml");
        return false;
    }

    TiXmlElement * root = configpath.RootElement();

    TiXmlElement * config = root->FirstChildElement("config");
    g_send_pipe_size = tools::stringAsInt(config->Attribute("send_pipe_size")) * 1024;
    g_recv_pipe_size = tools::stringAsInt(config->Attribute("recv_pipe_size")) * 1024;

    TiXmlElement * master = root->FirstChildElement("master");
    g_master_ip = master->Attribute("ip");
    g_master_port = tools::stringAsInt(master->Attribute("port"));

    TiXmlElement * relation = root->FirstChildElement("relation");
    TiXmlElement * noder = relation->FirstChildElement("noder");
    while (noder) {
        std::string name = noder->Attribute("name");
        if (name == core->getCorename()) {
            tools::osplitres targets;
            s32 count = tools::split(noder->Attribute("target"), ",", targets);
            for (s32 i = 0; i < targets.size(); i++) {
                g_target_set.insert(targets[i]);
            }
        }

        noder = noder->NextSiblingElement("noder");
    }

    g_noder_ip = core->getArgs("noder_ip");
    g_noder_port = tools::stringAsInt(core->getArgs("noder_port"));
    g_noder_area = tools::stringAsInt(core->getArgs("noder_area"));
    if (nullptr != core->getArgs("allarea")) {
        g_is_all_area = true;
    }

    core->launchTcpSession(this, g_master_ip.c_str(), g_master_port, g_send_pipe_size, g_recv_pipe_size);
    return true;
}

bool noder::launch(api::iCore * core) {
    g_idmanager = (iIDManager *)core->findModule("idmanager");

    register_proto(this, eNProtoID::AssigneID, noder::onAssigneID);
    register_proto(this, eNProtoID::BroadcastNoder, noder::onBroadcastNoder);
    return true;
}

bool noder::destroy(api::iCore * core) {
    return true;
}

iNoderSession * noder::getNoderSession(const std::string & name, const s32 id) {
    auto itor = g_id_session_map.find(id);
    if (itor == g_id_session_map.end()) {
        return nullptr;
    }

    tassert(itor->second->getname() == name, "wtf");
    if (itor->second->getname() == name) {
        return itor->second;
    }

    error(g_core, "session %d is %s but not is %s", id, itor->second->getname().c_str(), name.c_str());
    return nullptr;
}

void noder::reportLoad(const s16 load, const std::string & target_name) {
    auto itor = g_name_id_sessions_map.find(target_name);
    if (itor != g_name_id_sessions_map.end()) {
        oLoadReport report;
        report.set_load(load);

        for (auto i = itor->second.begin(); i != itor->second.end(); i++) {
            i->second->sendproto(eNProtoID::LoadReport, report);
        }
    }
}

iNoderSession * noder::getLoadlessNoder(const std::string & name) {
    auto itor = g_noder_load_sort_pool_map.find(name);
    if (itor != g_noder_load_sort_pool_map.end()) {
        return itor->second.get_end<noderSession>();
    }

    return nullptr;
}

void noder::setAllClientProtoCallback(const fAllClientProtoCallback fun, const char * debug) {
    tassert(g_all_client_proto_call == nullptr, "wtf");
    g_all_client_proto_call = fun;
}

void noder::registerEvent(const eNoderEvent id, const fNoderEvent ev, const char * debug) {
    g_noder_events[id].push_back(oNoderEvent(ev, debug));
}

void noder::sendMessageToNoder(const s32 noderid, const s16 msgid, const void * data, const s32 size) {
    auto itor = g_id_session_map.find(noderid);
    if (g_id_session_map.end() != itor) {
        itor->second->sendmessage(msgid, data, size);
    }
}

void noder::sendMessageToNoder(const s32 noderid, const std::string & noder_name, const s16 msgid, const void * data, const s32 size) {
    auto itor = g_id_session_map.find(noderid);
    if (g_id_session_map.end() != itor) {
        tassert(itor->second->getname() == noder_name, "wtf");
        if (itor->second->getname() == noder_name) {
            itor->second->sendmessage(msgid, data, size);
        }
    }
}

void noder::sendMessageToNoder(const std::string & noder_name, const s16 msgid, const void * data, const s32 size) {
    auto itor = g_name_id_sessions_map.find(noder_name);
    if (itor != g_name_id_sessions_map.end()) {
        for (auto i = itor->second.begin(); i != itor->second.end(); i++) {
            i->second->sendmessage(msgid, data, size);
        }
    }
}

api::iTcpSession * noder::onMallocConnection(api::iCore * core, const char * remote_ip, const s32 remote_port) {
    return NEW noderSession;
}

void noder::onError(api::iCore * core, api::iTcpSession * session) {
    if (session) {
        DEL session;
    }
}

void noder::onRelease(api::iCore * core) {

}

void noder::onConnected(api::iCore * core) {
    oReport report;
    report.set_name(g_noder_name);
    report.set_id(g_noder_id);
    oAddress * address = report.mutable_address();
    address->set_ip(g_noder_ip);
    address->set_port(g_noder_port);
    address->set_area(g_noder_area);

    sendproto(eNProtoID::Report, report);
    trace(core, "connected master %s:%d report self info %s:%d", g_master_ip.c_str(), g_master_port, g_noder_ip.c_str(), g_noder_port);
    sendproto(eNProtoID::NoderInited, oNoderInited());
}

void noder::onDisconnect(api::iCore * core) {
    trace(core, "master %s:%d faild disconnect, try again", g_master_ip.c_str(), g_master_port);
    start_timer(core, this, timer::id::try_connect_master_again, timer::config::try_connect_master_again_interval, 1, timer::config::try_connect_master_again_interval, 0);
}

void noder::onConnectFailed(api::iCore * core) {
    trace(core, "connect master %s:%d faild, try again", g_master_ip.c_str(), g_master_port);
    start_timer(core, this, timer::id::try_connect_master_again, timer::config::try_connect_master_again_interval, 1, timer::config::try_connect_master_again_interval, 0);
}

void noder::onTimer(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {
    switch (id) {
    case timer::id::try_connect_master_again: {
        core->launchTcpSession(this, g_master_ip.c_str(), g_master_port, g_send_pipe_size, g_recv_pipe_size);
        break;
    }
    }
}

void noder::onAssigneID(api::iCore * core, iSession * session, s64 context, const oAssigneID & body) {
    if (g_noder_id == invalid_id) {
        g_noder_id = body.id();
        std::string prefix = g_noder_name;
        prefix += "_" + tools::intAsString(g_noder_id);
        core->setSyncFilePrefix(prefix.c_str());
        core->setAsyncFilePrefix(prefix.c_str());
        core->launchTcpServer(static_self, g_noder_ip.c_str(), g_noder_port, g_send_pipe_size, g_recv_pipe_size);

        if (g_idmanager) {
            g_idmanager->setmask(g_noder_id);
        }
    }

    trace(core, "on assigne id %d, %s start listen %s:%d", g_noder_id, g_noder_name.c_str(), g_noder_ip.c_str(), g_noder_port);
    session->sendproto(eNProtoID::NoderInited, oNoderInited());
}

void noder::onBroadcastNoder(api::iCore * core, iSession * session, s64 context, const oBroadcastNoder & body) {
    //trace(core, "OnBroadcastNoder name: %s, id: %d, %s:%d", body.name().c_str(), body.id(), body.ip().c_str(), body.port());
    if (g_target_set.find(body.name()) != g_target_set.end() && body.id() != g_noder_id) {
        if (body.name() == g_noder_name && body.id() > g_noder_id) {
            return;
        }

        if (!g_is_all_area && body.address().area() != g_noder_area) {
            return;
        }

        auto itor = g_id_session_map.find(body.id());
        if (itor != g_id_session_map.end()) {
            return;
        }

        noderSession * noder_session = NEW noderSession(body.name(), body.address().area(), body.id(), body.address().ip(), body.address().port());
        core->launchTcpSession(noder_session, body.address().ip().c_str(), body.address().port(), g_send_pipe_size, g_recv_pipe_size);
    }
}
    
void noder::registerClientProto(s16 msgid, iProtoer<iNoderSession> & protoer) {
    g_noder_c_protoer_map.insert(std::make_pair(msgid, protoer));
}
