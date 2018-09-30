#ifndef __master_h__
#define __master_h__

#include "header.h"

class master : public iMaster, public api::iTcpServer {
public:
    virtual ~master() {}

    virtual bool initialize(api::iCore * core);
    virtual bool launch(api::iCore * core);
    virtual bool destroy(api::iCore * core);

    virtual api::iTcpSession * onMallocConnection(api::iCore * core, const char * remote_ip, const s32 remote_port);
    virtual void onError(api::iCore * core, api::iTcpSession * session);
    virtual void onRelease(api::iCore * core);
};

#endif //__Master_h__
