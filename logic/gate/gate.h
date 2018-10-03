#ifndef __gate_h__
#define __gate_h__

#include "header.h"

class gate : public iGate {
public:
    virtual ~gate() {}

    virtual bool initialize(api::iCore * core);
    virtual bool launch(api::iCore * core);
    virtual bool destroy(api::iCore * core);

    virtual void unRegisterProtocol(const s64 account, const s16 id);
    virtual bool pushMessage(wsDelegate * delegate, const s16 id, const void * data, const s32 size);

    virtual void send(const s64 account, const void * data, const s32 size);

    virtual bool accountLoginReport(wsDelegate * delegate);
    virtual void accountLogoutReport(wsDelegate * delegate);

protected:
    virtual void registerProtocol(const s64 account, const s16 id, const messager & messager);

private:
    static void onNoderReport(iNoderSession * session);
    static void onNoderDropout(iNoderSession * session);

private:
    static void onAccountRelogin(api::iCore * core, iSession * session, s64 context, const oAccountRelogin & body);
    static void onTellGameOver(api::iCore * core, iSession * session, s64 context, const oTellGameOver & body);
    static void onRelayServerMessageToClient(api::iCore * core, iSession * session, s64 context, const oRelayServerMessageToClient & body);
    static void onTellNoderMatchSuccessd(api::iCore * core, iSession * session, s64 context, const oTellNoderMatchSuccessd & body);
private:
    static void on_login_success(api::iCore * core, const ev::ologin_success & body);

private:
    static void onClientMessageToMatchReq(iDelegate * delegate, const s16 msgid, const void * data, const s32 size);
};

#endif //__gate_h__
