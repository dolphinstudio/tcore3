#ifndef __game_h__
#define __game_h__

#include "header.h"

class game : public iGame, public api::iTimer, public api::iHttpResponse {
public:
    virtual bool initialize(api::iCore * core);
    virtual bool launch(api::iCore * core);
    virtual bool destroy(api::iCore * core);

    virtual void onStart(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}
    virtual void onTimer(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick);
    virtual void onEnd(api::iCore * core, const s32 id, const api::iContext & context, bool nonviolent, const s64 tick) {}
    virtual void onPause(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}
    virtual void onResume(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}

    virtual void onError(const s32 id, const s32 error, const api::iContext & context);
    virtual void onResponse(const s32 id, const void * data, const s32 size, const api::iContext & context);

    virtual void AiPushSoldiersGoToBattlePush(const s64 account, const s32 orbit_number);

private:
    static void loadData(iUnit * scene, const ev::omatcher & info, const eSide side);

private:
    static void on_match_success(api::iCore * core, const ev::omatch_success & ev);
    static void on_load_card_data(api::iCore * core, const ev::oload_card_data & ev);
    static void on_game_start(api::iCore * core, iUnit * scene);
    static void on_game_end(api::iCore * core, iUnit * scene);

    static void on_account_relogin(api::iCore * core, const s64 & account);

    static void onClientGameReadyPush(const s64 account, const oClientGameReadyPush & body);
    static void onClientSoldiersGoToBattlePush(const s64 account, const oClientSoldiersGoToBattlePush & body);
};

#endif //__game_h__
