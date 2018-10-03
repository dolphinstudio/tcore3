#ifndef __iGate_h__
#define __iGate_h__

#include "api.h"
#include "iWebSocket.h"
#include "tools.h"
#include "google/protobuf/message.h"

class iDelegate : public iWBSessionDelegate {
public:
    virtual ~iDelegate() {}

    virtual s64 getid() = 0;

    virtual void sendproto(const s16 id, const ::google::protobuf::Message & body) {
        const s16 size = body.ByteSize();
        char * buff = (char *)alloca(size + sizeof(s16));
        if (!body.SerializePartialToArray(buff + sizeof(s16), size)) {
            return;
        }

        tools::memery::safeMemcpy(buff, sizeof(s16), &id, sizeof(s16));

        send(buff, size + sizeof(s16), eFrameType::binary_frame);
    }

    virtual void sendproto(const s16 id, const void * data, const s32 size) {
        char * buff = (char *)alloca(sizeof(s16) + size);
        tools::memery::safeMemcpy(buff, sizeof(s16), &id, sizeof(s16));
        tools::memery::safeMemcpy(buff + sizeof(s16), size, data, size);
        send(buff, size + sizeof(s16), eFrameType::binary_frame);
    }
};

class messager {
    typedef void(*fun)(const void * f, iDelegate * delegate, const s16 msgid, const char * context, const s32 size, const char * debug);
public:
    virtual ~messager() {}
    messager(const fun f, const void * call, const char * debug) : _fun(f), _call(call), _debug(debug) {}
    const std::string _debug;
    const fun _fun;
    const void * _call;

    virtual void onMessage(iDelegate * delegate, const s16 msgid, const char * context, const s32 size) const {
        _fun(_call, delegate, msgid, context, size, _debug.c_str());
    }
};

template <typename packet>
class tMessager {
public:
    virtual ~tMessager() {}

    typedef void(*fun)(iDelegate * delegate, const s16 msgid, const packet & info);
    static void onMessage(const void * f, iDelegate * delegate, const s16 msgid, const char * context, const s32 size, const char * debug) {
        if (0 == size) {
            (*(fun)f)(delegate, msgid, packet());
        }
        else {
            packet info;
            if (info.ParseFromArray(context, size)) {
                (*(fun)f)(delegate, msgid, info);
            } else {
                tassert(false, "wtf");
            }
        }
    }
};

class cMessager {
public:
    virtual ~cMessager() {}
    typedef void(*fun)(iDelegate * delegate, const s16 msgid, const void *, const s32);
    static void onMessage(const void * f, iDelegate * delegate, const s16 msgid, const char * context, const s32 size, const char * debug) {
        (*(fun)f)(delegate, msgid, context, size);
    }
};

class iGate : public iModule {
public:
    virtual ~iGate() {}

    template<typename Packet>
    void registerProtocol(const s64 account, const s16 id, void(*f)(iDelegate * delegate, const s16 id, const Packet &), const char * debug) {
        messager msger(tMessager<Packet>::onMessage, (void *)f, debug);
        registerProtocol(account, id, msger);
    }

    void registerProtocol(const s64 account, const s16 id, void(*f)(iDelegate * delegate, const s16 id, const void *, const s32 size), const char * debug) {
        messager msger(cMessager::onMessage, (void *)f, debug);
        registerProtocol(account, id, msger);
    }

    virtual void unRegisterProtocol(const s64 account, const s16 id) = 0;

    virtual void sendproto(const s64 account, const s16 id, const ::google::protobuf::Message & body) {
        const s16 size = body.ByteSize();
        char * buff = (char *)alloca(size + sizeof(s16));
        if (!body.SerializePartialToArray(buff + sizeof(s16), size)) {
            return;
        }

        tools::memery::safeMemcpy(buff, sizeof(s16), &id, sizeof(s16));
        send(account, buff, size + sizeof(s16));
    }

    virtual void send(const s64 account, const void * data, const s32 size) = 0;
protected:
    virtual void registerProtocol(const s64 account, const s16 id, const messager & meger) = 0;
};

#define register_protocol(gate, account, id, fun) gate->registerProtocol(account, id, fun, #fun);
#define unregister_protocol(gate, account, id) gate->unRegisterProtocol(account, id);

#endif //__iGate_h__
