#ifndef __role_h__
#define __role_h__

#include "header.h"

class role : public iRole, public api::iTimer {
public:
    virtual ~role() {}

    virtual bool initialize(api::iCore * core);
    virtual bool launch(api::iCore * core);
    virtual bool destroy(api::iCore * core);

    virtual iUnit * popRoleToOrbit(iUnit * player, iUnit * scene, iUnit * orbit);
    virtual void recoverRole(iUnit * scene, iUnit * role, const s32 side, const float y);

    virtual void onStart(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick)  {}
    virtual void onTimer(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick);
    virtual void onEnd(api::iCore * core, const s32 id, const api::iContext & context, bool nonviolent, const s64 tick)  {}
    virtual void onPause(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick)  {}
    virtual void onResume(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}

private:
    static void on_game_end(api::iCore * core, iUnit * scene);
    static void on_join_combiner(api::iCore * core, const ev::ojoin_combiner & ev);
private:
    iUnit * popRole(iUnit * player);
};


#endif //__role_h__