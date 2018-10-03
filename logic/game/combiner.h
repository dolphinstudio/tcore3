#ifndef __combiner_h__
#define __combiner_h__

#include "header.h"

class combiner : public iCombiner, public api::iTimer {
public:
    virtual ~combiner() {}
    
    virtual bool initialize(api::iCore * core);
    virtual bool launch(api::iCore * core);
    virtual bool destroy(api::iCore * core);

    virtual void onStart(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick)  {}
    virtual void onTimer(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick);
    virtual void onEnd(api::iCore * core, const s32 id, const api::iContext & context, bool nonviolent, const s64 tick)  {}
    virtual void onPause(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick)  {}
    virtual void onResume(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick)  {}

    virtual iUnit * MergeToCombiner(iUnit * orbit, iUnit * toper, iUnit * bottomer);
    virtual void RecoverCombiner(iUnit * scene, iUnit * orbit, iUnit * combiner);

private:
    void CheckTopers(iUnit * scene, iUnit * orbit, iUnit * combiner);
    void CheckBottomers(iUnit * scene, iUnit * orbit, iUnit * combiner);

private:
    static void on_role_join_orbit(api::iCore * core, const ev::orole_join_orbit & ev);
    static void on_role_weight_changed(api::iCore *, iUnit *, const dc::layout &, const float & oldvalue, const float &newvalue, const bool);
};

#endif //__combiner_h__
