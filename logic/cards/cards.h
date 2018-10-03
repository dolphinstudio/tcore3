#ifndef __cards_h__
#define __cards_h__

#include "header.h"

class cards : public iCards, public api::iTimer {
public:
    virtual ~cards() {}

    virtual bool initialize(api::iCore * core);
    virtual bool launch(api::iCore * core);
    virtual bool destroy(api::iCore * core);

    virtual void loadData(iUnit * player, const s64 copyer);
    virtual void initGameRole(iUnit * scene);
    virtual iUnit * createRole(iUnit * player, const s64 tick);
    virtual void recoverRole(iUnit * scene, iUnit * role);

    virtual void onStart(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}
    virtual void onTimer(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick);
    virtual void onEnd(api::iCore * core, const s32 id, const api::iContext & context, bool nonviolent, const s64 tick) {}
    virtual void onPause(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}
    virtual void onResume(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}

private:
    static void on_load_data(api::iCore * core, const ev::oload_data & body);
};

#endif //__cards_h__
