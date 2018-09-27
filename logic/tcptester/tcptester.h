#ifndef __tcptester_h__
#define __tcptester_h__

#include "header.h"

class tcptester : public iModule, public iTimer {
public:
    virtual ~tcptester() {}

    virtual bool initialize(tcore::api::iCore * core);
    virtual bool launch(tcore::api::iCore * core);
    virtual bool destroy(tcore::api::iCore * core);

    virtual void onStart(iCore * core, const s32 id, const iContext & context, const s64 tick);
    virtual void onTimer(iCore * core, const s32 id, const iContext & context, const s64 tick);
    virtual void onEnd(iCore * core, const s32 id, const iContext & context, bool nonviolent, const s64 tick);
    virtual void onPause(iCore * core, const s32 id, const iContext & context, const s64 tick);
    virtual void onResume(iCore * core, const s32 id, const iContext & context, const s64 tick);
};

#endif //__tcptester_h__