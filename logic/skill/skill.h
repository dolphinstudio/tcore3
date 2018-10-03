#ifndef __skill_h__
#define __skill_h__

#include "header.h"

class skill : public iSkill, public api::iTimer {
public:
    virtual ~skill() {}

    virtual bool initialize(api::iCore * core);
    virtual bool launch(api::iCore * core);
    virtual bool destroy(api::iCore * core);
    
    virtual bool disappearanceDetectionWithRole(iUnit * role1, iUnit * role2);
    virtual bool disappearanceDetectionWithCombiner(iUnit * role, iUnit * combiner);

    virtual void onStart(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}
    virtual void onTimer(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick);
    virtual void onEnd(api::iCore * core, const s32 id, const api::iContext & context, bool nonviolent, const s64 tick) {}
    virtual void onPause(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}
    virtual void onResume(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}

private:
    static void on_cast_skill(api::iCore * core, const ev::ocast_skill & ev);
    static void on_role_join_orbit(api::iCore * core, const ev::orole_join_orbit & ev);
    static void on_join_combiner(api::iCore * core, const ev::ojoin_combiner & ev);
    static void on_role_collision(api::iCore * core, const ev::orole_collision & ev);
    static void on_remove_role(api::iCore * core, iUnit * role);
    static void on_recover_orbit(api::iCore * core, iUnit * orbit);
    static void on_role_thrusting(api::iCore * core, iUnit * role);
};

#endif //__skill_h__
