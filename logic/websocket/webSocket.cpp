#include "webSocket.h"
#include "webSocketServer.h"

api::iCore * g_core = nullptr;

bool webSocket::initialize(api::iCore * core) {
    g_core = core;
    return true;
}

bool webSocket::launch(api::iCore * core) {
    return true;
}

bool webSocket::destroy(api::iCore * core) {
    return true;
}

bool webSocket::launchwebSocketServer(iWBServerDelegate * delegate, const char * ip, const s32 port) {
    webSocketServer * server = NEW webSocketServer(delegate);
    return g_core->launchTcpServer(server, ip, port, 16 * 1024 * 1024, 16 * 1024 * 1024);
}
