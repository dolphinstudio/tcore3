#ifndef __iLogicer_h__
#define __iLogicer_h__

#include "api.h"
#include "google/protobuf/message.h"

class lMessager {
    typedef void(*fun)(const void * f, const s64 account, const char * context, const s32 size, const char * debug);
public:
    virtual ~lMessager() {}
    lMessager(const fun f, const void * call, const char * debug) : _fun(f), _call(call), _debug(debug) {}
    const std::string _debug;
    const fun _fun;
    const void * _call;

    virtual void onMessage(const s64 account, const char * context, const s32 size) const {
        _fun(_call, account, context, size, _debug.c_str());
    }
};

template <typename packet>
class tLMessager {
public:
    virtual ~tLMessager() {}

    typedef void(*fun)(const s64 account, const packet & info);
    static void onMessage(const void * f, const s64 account, const char * context, const s32 size, const char * debug) {
        if (0 == size) {
            (*(fun)f)(account, packet());
        } else {
            packet info;
            if (info.ParseFromArray(context, size)) {
                (*(fun)f)(account, info);
            }
            else {
                tassert(false, "wtf");
            }
        }
    }
};

class iLogicer : public iModule {
public:
    virtual ~iLogicer() {}
    virtual void tellGameOver(s64 account) = 0;

    template<typename packet>
    void registerProtocol(const s64 account, const s16 id, void(*f)(const s64 account, const packet &), const char * debug) {
        lMessager messager(tLMessager<packet>::onMessage, (void *)f, debug);
        registerProtocol(account, id, messager);
    }

    virtual void unRegisterProtocol(const s64 account, const s16 id) = 0;

    virtual void sendproto(const std::set<s64> & accounts, const s16 id, const ::google::protobuf::Message & body) {
        const s16 size = body.ByteSize();
        char * buff = (char *)alloca(size);
        if (!body.SerializePartialToArray(buff, size)) {
            return;
        }

        send(accounts, id, buff, size);
    }

    virtual void send(const std::set<s64> & accounts, const s16 id, const void * data, const s32 size) = 0;


    virtual void sendproto(const s64 account, const s16 id, const ::google::protobuf::Message & body) {
        const s16 size = body.ByteSize();
        char * buff = (char *)alloca(size);
        if (!body.SerializePartialToArray(buff, size)) {
            return;
        }

        send(account, id, buff, size);
    }

    virtual void send(const s64 account, const s16 id, const void * data, const s32 size) = 0;
protected:
    virtual void registerProtocol(const s64 account, const s16 id, const lMessager & messager) = 0;
};

#define register_l_protocol(logicer, account, id, fun) logicer->registerProtocol(account, id, fun, #fun);
#define unregister_l_protocol(logicer, account, id) logicer->unRegisterProtocol(account, id);
#endif //__iLogicer_h__
