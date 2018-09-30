#ifndef __udpsession_h__
#define __udpsession_h__

#include "header.h"

class udpSession : public iUdpSession, public iTimer {
public:
    virtual ~udpSession() {}

    udpSession(const std::string & ip, const s32 port) : _remote(ip, port) {}

    virtual void onCreate(bool success);
    virtual void onRecv(iCore * core, const char * ip, const s32 port, const char * context, const int size);
    virtual void onClose(iCore * core);

    virtual void onStart(iCore * core, const s32 id, const iContext & context, const s64 tick);
    virtual void onTimer(iCore * core, const s32 id, const iContext & context, const s64 tick);
    virtual void onEnd(iCore * core, const s32 id, const iContext & context, bool nonviolent, const s64 tick);
    virtual void onPause(iCore * core, const s32 id, const iContext & context, const s64 tick);
    virtual void onResume(iCore * core, const s32 id, const iContext & context, const s64 tick);

private:
    const oAddress _remote;
};

#endif //__udpsession_h__
