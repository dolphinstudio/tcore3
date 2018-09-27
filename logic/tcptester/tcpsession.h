#ifndef __tcpSession_h__
#define __tcpSession_h__

#include "header.h"

class tcpSession : public iTcpSession, public iTimer {
public:
    virtual ~tcpSession() {}

    void release();

    virtual int onRecv(iCore * core, const void * context, const int size);
    virtual void onConnected(iCore * core);
    virtual void onDisconnect(iCore * core);
    virtual void onConnectFailed(iCore * core);

    virtual void onStart(iCore * core, const s32 id, const iContext & context, const s64 tick);
    virtual void onTimer(iCore * core, const s32 id, const iContext & context, const s64 tick);
    virtual void onEnd(iCore * core, const s32 id, const iContext & context, bool nonviolent, const s64 tick);
    virtual void onPause(iCore * core, const s32 id, const iContext & context, const s64 tick);
    virtual void onResume(iCore * core, const s32 id, const iContext & context, const s64 tick);

private:
    friend tlib::tpool<tcpSession>;
    tcpSession();

    const s32 _index;
    const s64 _bron_tick;
    
    s32 _total_size;
};

#endif //__tcpSession_h__
