#ifndef __webSocket_h__
#define __webSocket_h__

#include "header.h"

class webSocket : public iWebSocket {
public:
    virtual bool initialize(api::iCore * core);
    virtual bool launch(api::iCore * core);
    virtual bool destroy(api::iCore * core);

    virtual bool launchwebSocketServer(iWBServerDelegate * delegate, const char * ip, const s32 port);
};

#endif //__webSocket_h__
