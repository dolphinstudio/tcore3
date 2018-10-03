#ifndef __wsSession_h__
#define __wsSession_h__

#include "header.h"

class wsDelegate : public iDelegate, public iReference, public api::iTimer {
public:
    virtual ~wsDelegate() {}
    wsDelegate() : _id(define::invalid_id), _logic(define::invalid_id), _token(""), _is_login(false) {}

    virtual void onConnect();
    virtual void onDisconnect();
    virtual void onRecv(const char * data, const s32 size);

    virtual void setid(const s64 id);
    virtual s64 getid() { return _id; }

    virtual void setToken(const std::string & token) { _token = token; }
    virtual const std::string & getToken() { return _token; }

    virtual void setLogicNoderID(s32 noderid) { _logic = noderid; }
    virtual s32 getLogicNoderID() { return _logic; }

    virtual void setIsLogin(bool is) { _is_login = is; }
    virtual bool isLogin() { return _is_login; }

    void delayClose();

    virtual void onStart(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}
    virtual void onTimer(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick);
    virtual void onEnd(api::iCore * core, const s32 id, const api::iContext & context, bool nonviolent, const s64 tick) {}
    virtual void onPause(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}
    virtual void onResume(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}

protected:
    virtual void recover();

private:
    s64 _id;
    s32 _logic;
    std::string _token;
    bool _is_login;
};

#endif //__WSSession_h__
