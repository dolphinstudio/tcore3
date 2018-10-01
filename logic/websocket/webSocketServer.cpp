#include "webSocketServer.h"
#include "webSocketSession.h"

api::iTcpSession * webSocketServer::onMallocConnection(api::iCore * core, const char * remote_ip, const s32 remote_port) {
    iWBSessionDelegate * delegate = _delegate->onMallocSessionDelegate();
    return NEW webSocketSession(delegate);
}

void webSocketServer::onError(api::iCore * core, api::iTcpSession * session) {
    DEL session;
}

void webSocketServer::onRelease(api::iCore * core) {

}
