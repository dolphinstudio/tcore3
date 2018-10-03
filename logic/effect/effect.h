#ifndef __effect_h__
#define __effect_h__

#include "header.h"

class effect : public iEffect, public api::iTimer {
public:
    virtual ~effect() {}

    virtual bool initialize(api::iCore * core);
    virtual bool launch(api::iCore * core);
    virtual bool destroy(api::iCore * core);
    
    virtual void createEffect(iUnit * scene, iUnit * orbit, const s32 configid);

    virtual void onStart(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}
    virtual void onTimer(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick);
    virtual void onEnd(api::iCore * core, const s32 id, const api::iContext & context, bool nonviolent, const s64 tick) {}
    virtual void onPause(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}
    virtual void onResume(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}

private:
    static void on_game_end(api::iCore * core, iUnit * scene);
};

#endif //__effect_h__
