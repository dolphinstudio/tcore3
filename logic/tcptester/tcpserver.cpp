#include "tcpserver.h"
#include "tcpsession.h"


iTcpSession * tcpServer::onMallocConnection(iCore * core, const char * remote_ip, const s32 remote_port) {
    tcpSession * session = create_from_pool(g_tcpsession_pool);
    return session;
}

void tcpServer::onError(iCore * core, iTcpSession * session) {
    trace(core, "tcp server on error %llx", session);
    if (session) {
        tcpSession * ts = dynamic_cast<tcpSession*>(session);
        recover_to_pool(g_tcpsession_pool, ts);
    }
}

void tcpServer::onRelease(iCore * core) {

}
