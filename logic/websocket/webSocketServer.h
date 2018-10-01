#ifndef __webSocketServer_h__
#define __webSocketServer_h__

#include "header.h"

class webSocketServer : public tcore::api::iTcpServer {
public:
    virtual ~webSocketServer() {}

    virtual api::iTcpSession * onMallocConnection(api::iCore * core, const char * remote_ip, const s32 remote_port);
    virtual void onError(api::iCore * core, api::iTcpSession * session);
    virtual void onRelease(api::iCore * core);

    webSocketServer(iWBServerDelegate * delegate) : _delegate(delegate) {}
private:
    iWBServerDelegate * const _delegate;
};

#endif //__webSocketServer_h__
