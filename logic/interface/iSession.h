#ifndef __iSession_h__
#define __iSession_h__

#include "api.h"
using namespace tcore;
#include <map>
#include "google/protobuf/message.h"

#pragma pack(push, 1)
struct oProtocol {
    s16 id;
    s32 size;
};
#pragma pack(pop)

class iSession;


template<typename s>
class iProtoer {
public:
    typedef void(*fOnMessage)(api::iCore * core, s * session, s64 context, void * fun, const void * data, const int len);
    virtual ~iProtoer() {}
    iProtoer(const fOnMessage fun, void * callback, const char * debug) : _onmessager(fun), _callback(callback), _debug(debug) {}
    virtual void onMessage(api::iCore * core, s * session, s64 context, const void * data, const int size) {
        _onmessager(core, session, context, _callback, data, size);
    }

    const std::string _debug;
    void * _callback;

    const fOnMessage _onmessager;
};

template<typename s, typename pb>
class tAnalysis {
    typedef void(*fCallback)(api::iCore *, s *, s64 context, const pb &);
public:
    virtual ~tAnalysis() {}
    
    static void Analysis(api::iCore * core, s * session, s64 context, void * fun, const void * data, const int len) {
        pb package;
        if (len > 0) {
            if (!package.ParseFromArray(data, len)) {
                tassert(false, "wtf");
                return;
            }
        }

        fCallback call = (fCallback)fun;
        call(core, session, context, package);
    }
};

class iSession : public api::iTcpSession {
public:
    virtual ~iSession() {}

    virtual int onRecv(api::iCore * core, const void * context, const int size) {
        if (size < sizeof(oProtocol)) {
            return 0;
        }

        const oProtocol * header = (const oProtocol *)context;
        if (header->size > size) {
            return 0;
        }

        const char * data = ((const char *)context) + sizeof(oProtocol);

        onMessage(core, header->id, data, header->size - sizeof(oProtocol));
        return header->size;
    }

    template<typename pb>
    void registerProto(const s16 id, void(*function)(api::iCore *, iSession *, s64 context, const pb &), const char * debug) {
        iProtoer<iSession> protoer(tAnalysis<iSession, pb>::Analysis, (void *)function, debug);
        _protoer_map.insert(std::make_pair(id, protoer));
    }

    virtual void sendproto(const s16 id, const ::google::protobuf::Message & body) {
        const s32 size = body.ByteSize();
        char * temp = (char *)alloca(size);
        if (!body.SerializeToArray(temp, size)) {
            tassert(false, "body format error");
            return;
        }

        oProtocol protocol;
        protocol.id = id;
        protocol.size = size + sizeof(protocol);
        send(&protocol, sizeof(protocol));
        send(temp, size);
    }

    virtual void sendmessage(const s16 id, const void * data, const s32 len) {
        oProtocol protocol;
        protocol.id = id;
        protocol.size = len + sizeof(protocol);
        send(&protocol, sizeof(protocol));
        send(data, len);
    }

protected:
    virtual void onMessage(api::iCore * core, const s16 id, const char * context, const int size) {
        auto itor = _protoer_map.find(id);
        if (itor != _protoer_map.end()) {
            itor->second.onMessage(core, this, 0, context, size);
        }
    }

private:
    std::map<s16, iProtoer<iSession>> _protoer_map;
};

#define register_proto(session, id, fun) session->registerProto(id, fun, #fun)

#endif //__iSession_h__
