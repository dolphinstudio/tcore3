#ifndef __logicer_h__
#define __logicer_h__

#include "header.h"

class logicer : public iLogicer {
public:
    virtual ~logicer() {}

    virtual bool initialize(api::iCore * core);
    virtual bool launch(api::iCore * core);
    virtual bool destroy(api::iCore * core);

    virtual void tellGameOver(s64 account);

    static void onNoderReport(iNoderSession * session);
    static void onNoderDropout(iNoderSession * session);

    virtual void send(const std::set<s64> & accounts, const s16 id, const void * data, const s32 size);
    virtual void send(const s64 account, const s16 id, const void * data, const s32 size);
    virtual void unRegisterProtocol(const s64 account, const s16 id);
protected:
    virtual void registerProtocol(const s64 account, const s16 id, const lMessager & messager);

private:
    static void onTellNoderMatchSuccessd(api::iCore * core, iSession * session, s64 context, const oTellNoderMatchSuccessd & body);
    static void onAccountRelogin(api::iCore * core, iSession * session, s64 context, const oAccountRelogin & body);
private:
    static void onAllClientProto(api::iCore *, iNoderSession *, s64 account, const oRelayClientMessageToNoder & body);
};

#endif //__Logicer_h__
