#ifndef __match_h__
#define __match_h__

#include "header.h"

class match : public IMatch, public api::iTimer {
public:
    virtual bool initialize(api::iCore * core);
    virtual bool launch(api::iCore * core);
    virtual bool destroy(api::iCore * core);

    virtual void onStart(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}
    virtual void onTimer(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick);
    virtual void onEnd(api::iCore * core, const s32 id, const api::iContext & context, bool nonviolent, const s64 tick) {}
    virtual void onPause(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}
    virtual void onResume(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}

private:
    static void onNoderReport(iNoderSession * session);
    static void onNoderDropout(iNoderSession * session);

    static void onAccountLogout(api::iCore * core, iSession * session, s64 context, const oAccountLogout & body);
    static void onAccountRelogin(api::iCore * core, iSession * session, s64 context, const oAccountRelogin & body);
    static void onTellGameOver(api::iCore * core, iSession * session, s64 context, const oTellGameOver & body);

    static void onClientMatchReq(api::iCore *, iNoderSession *, s64 account, const oClientMatchReq & body);
    static void onClientCreateChallengeForFriendReq(api::iCore *, iNoderSession *, s64 account, const oClientCreateChallengeForFriendReq & body);
    static void onClientJoinChallengeReq(api::iCore *, iNoderSession *, s64 account, const oClientJoinChallengeReq & body);
    static void onClientChallengeCancelReq(api::iCore *, iNoderSession *, s64 account, const oClientChallengeCancelReq & body);
};

#endif //__match_h__
