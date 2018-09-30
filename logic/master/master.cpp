#include "master.h"
#include "masterSession.h"

std::string g_master_ip;
s32 g_master_port;

s32 g_send_pipe_size = 64;
s32 g_recv_pipe_size = 64;

id_session_map g_id_session_map;
name_sessions_map g_name_sessions_map;

bool master::initialize(api::iCore * core) {

    std::string path = tools::file::getApppath();
    path += "/server_config/noder.xml";

    TiXmlDocument configdoc;
    if (false == configdoc.LoadFile(path.c_str())) {
        tassert(false, "where is noder.xml");
        return false;
    }

    TiXmlElement * root = configdoc.RootElement();

    TiXmlElement * config = root->FirstChildElement("config");
    g_send_pipe_size = tools::stringAsInt(config->Attribute("send_pipe_size")) * 1024;
    g_recv_pipe_size = tools::stringAsInt(config->Attribute("recv_pipe_size")) * 1024;

    TiXmlElement * master = root->FirstChildElement("master");
    g_master_ip = master->Attribute("ip");
    g_master_port = tools::stringAsInt(master->Attribute("port"));

    core->setAsyncFilePrefix("master");
    core->setSyncFilePrefix("master");

    core->launchTcpServer(this, g_master_ip.c_str(), g_master_port, g_send_pipe_size, g_recv_pipe_size);
    trace(core, "master start listen at port %d", g_master_port);

    return true;
}

bool master::launch(api::iCore * core) {
    return true;
}

bool master::destroy(api::iCore * core) {
    return true;
}

api::iTcpSession * master::onMallocConnection(api::iCore * core, const char * remote_ip, const s32 remote_port) {
    return NEW masterSession;
}

void master::onError(api::iCore * core, api::iTcpSession * session) {
    if (session) {
        DEL session;
    }
}

void master::onRelease(api::iCore * core) {

}
