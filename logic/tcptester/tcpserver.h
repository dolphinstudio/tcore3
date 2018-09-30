#ifndef __tcpserver_h__
#define __tcpserver_h__

#include "header.h"

class tcpServer : public iTcpServer {
public:
    virtual ~tcpServer() {}

    virtual iTcpSession * onMallocConnection(iCore * core, const char * remote_ip, const s32 remote_port);
    virtual void onError(iCore * core, iTcpSession * session);
    virtual void onRelease(iCore * core);
};

#endif //__tcpserver_h__
