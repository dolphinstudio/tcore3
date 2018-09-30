#ifndef __masterSession_h__
#define __masterSession_h__

#include "header.h"

class masterSession : public iSession, public api::iTimer {
public:
    virtual ~masterSession() {}
    masterSession() : _area(invalid_id), _id(invalid_id), _name(""), _ip(""), _port(0) {}

    virtual void onConnected(api::iCore * core);
    virtual void onDisconnect(api::iCore * core);
    virtual void onConnectFailed(api::iCore * core);

    static void onReport(api::iCore * core, iSession * session, s64 context, const oReport & body);
    static void onNoderInited(api::iCore * core, iSession * session, s64 context, const oNoderInited & body);

    virtual void onStart(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}
    virtual void onTimer(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick);
    virtual void onEnd(api::iCore * core, const s32 id, const api::iContext & context, bool nonviolent, const s64 tick) {}
    virtual void onPause(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}
    virtual void onResume(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}

    s32 _area;
    s32 _id;
    std::string _name;
    std::string _ip;
    s32 _port;
};

#endif //__Master_h__
