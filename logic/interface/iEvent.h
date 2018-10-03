#ifndef __iEvent_h__
#define __iEvent_h__

#include "api.h"

using namespace tcore;

class iEventer {
public:
    virtual ~iEventer() {}
    iEventer(const char * debug) : _debug(debug) {}
    virtual void trigger(api::iCore * core, const void * context, const s32 len) const = 0;
    
    const std::string _debug;
};

template<typename t>
class eventer : public iEventer {
    typedef void(*ef)(api::iCore *, const t &);
public:
    virtual ~eventer () {}
    eventer(const ef f, const char * debug) : iEventer(debug), _ef(f) {}
    virtual void trigger(api::iCore * core, const void * context, const s32 len) const {
        tassert(sizeof(t) == len, "wtf");
        (*_ef)(core, *(t*)context);
    }

private:
    const ef _ef;
};

template<typename t>
class tEventer : public iEventer {
    typedef void(*ef)(api::iCore *, t *);
public:
    virtual ~tEventer() {}
    tEventer(const ef f, const char * debug) : iEventer(debug), _ef(f) {}
    virtual void trigger(api::iCore * core, const void * context, const s32 len) const {
        tassert(sizeof(t) == len, "wtf");
        (*_ef)(core, (t *)context);
    }

private:
    const ef _ef;
};

class cEventer : public iEventer {
    typedef void(*ef)(api::iCore *);
public:
    virtual ~cEventer() {}
    cEventer(const ef f, const char * debug) : iEventer(debug), _ef(f) {}
    virtual void trigger(api::iCore * core, const void * context, const s32 len) const {
        (*_ef)(core);
    }

private:
    const ef _ef;
};

class iEvent : public iModule {
public:
    virtual ~iEvent() {}

    template<class t>
    void register_ev(const s32 eventid, void(*ef)(api::iCore *, t *), const char * debug) {
        register_ev(eventid, NEW tEventer<t>(ef, debug));
    }

    template<class t>
    void register_ev(const s32 eventid, void(*ef)(api::iCore *, const t &), const char * debug) {
        register_ev(eventid, NEW eventer<t>(ef, debug));
    }

    void register_ev(const s32 eventid, void(*ef)(api::iCore *), const char * debug) {
        register_ev(eventid, NEW cEventer(ef, debug));
    }

    template<class t>
    void trigger(const s32 eventid, const t & context) {
        trigger(eventid, &context, sizeof(context));
    }

    template<class t>
    void trigger(const s32 eventid, t * context) {
        trigger(eventid, context, sizeof(t));
    }

protected:
    virtual void trigger(const s32 eventid, const void * context, const s32 size) = 0;
    virtual void register_ev(const s32 eventid, iEventer * handle) = 0;
};

#define register_event(ev, id, fun) ev->register_ev(id, fun, #fun);

#endif //__iEvent_h__
