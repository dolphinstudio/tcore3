#include "tcptester.h"
#include "tcpsession.h"
#include "tcpserver.h"

iCore * g_core = nullptr;
s32 g_connection_count = 0;
s32 g_recv_total_size = 0;
s32 g_send_total_size = 0;

static std::string static_server_ip = "";
static s32 static_server_port = invalid_port;

bool tcptester::initialize(tcore::api::iCore * core) {
    g_core = core;
    return true;
}

bool tcptester::launch(tcore::api::iCore * core) {
    static_server_ip = core->getArgs("ip");
    static_server_port = tools::stringAsInt(core->getArgs("port"));
    if (core->getArgs("server")) {
        tcpServer * ts = NEW tcpServer;
        core->launchTcpServer(ts, static_server_ip.c_str(), static_server_port, 40960, 40960);
        start_timer(core, this, 101, 100, forever, 5000, 0);
    } else {
        start_timer(core, this, 100, 100, forever, 20, 0);
    }

    return true;
}

bool tcptester::destroy(tcore::api::iCore * core) {
    return true;
}

void tcptester::onStart(iCore * core, const s32 id, const iContext & context, const s64 tick) {

}

void tcptester::onTimer(iCore * core, const s32 id, const iContext & context, const s64 tick) {
   switch(id) {
   case 100: {
       if (g_connection_count < 1000) {
           tcpSession * session = create_from_pool(g_tcpsession_pool);
           core->launchTcpSession(session, static_server_ip.c_str(), static_server_port, 40960, 40960);
       }
       break;
   }
   case 101: {
       trace(core, "total recv size %.2fMB, total send size %.2fMB", g_recv_total_size / (1024 * 1024.0f), g_send_total_size / (1024 * 1024.0f));
       break;
   }
   }
}

void tcptester::onEnd(iCore * core, const s32 id, const iContext & context, bool nonviolent, const s64 tick) {

}

void tcptester::onPause(iCore * core, const s32 id, const iContext & context, const s64 tick) {

}

void tcptester::onResume(iCore * core, const s32 id, const iContext & context, const s64 tick) {

}
