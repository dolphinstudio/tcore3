#ifndef __iNoder_h__
#define __iNoder_h__

#include "iSession.h"

#include "noder.pb.h"

class iNoderSession : public iSession {
public:
    virtual ~iNoderSession() {}

    virtual const std::string & getname() = 0;
    virtual const s32 getarea() = 0;
    virtual const s32 getid() = 0;
    virtual const std::string & getip() = 0;
    virtual const s32 getport() = 0;
};

enum eNoderEvent {
    noder_report = 0,
    noder_dropout,

    //add before this
    count
};

typedef void(*fNoderEvent)(iNoderSession *);
typedef void(*fAllClientProtoCallback)(api::iCore *, iNoderSession *, s64 context, const oRelayClientMessageToNoder & body);

class iNoder : public iModule{
public:
    virtual ~iNoder() {}

    virtual const std::string & getname() = 0;
    virtual const s32 getarea() = 0;
    virtual const s32 getid() = 0;
    virtual const std::string & getip() = 0;
    virtual const s32 getport() = 0;

    virtual iNoderSession * getNoderSession(const std::string & name, const s32 id) = 0;
    virtual void reportLoad(const s16 load, const std::string & target_name) = 0;
    virtual iNoderSession * getLoadlessNoder(const std::string & name) = 0;

    virtual void registerEvent(const eNoderEvent id, const fNoderEvent ev, const char * debug) = 0;


    template<typename pb>
    void registerClientProto(s16 msgid, void(*function)(api::iCore *, iNoderSession *, s64 context, const pb &), const char * debug) {
        iProtoer<iNoderSession> protoer(tAnalysis<iNoderSession, pb>::Analysis, (void *)function, debug);
        //_noder_c_protoer_map.insert(make_pair(id, protoer));
        registerClientProto(msgid, protoer);
    }

    virtual void setAllClientProtoCallback(const fAllClientProtoCallback fun, const char * debug) = 0;

    virtual void sendMessageToNoder(const s32 noderid, const s16 msgid, const void * data, const s32 size) = 0;
    virtual void sendMessageToNoder(const s32 noderid, const std::string & noder_name, const s16 msgid, const void * data, const s32 size) = 0;
    virtual void sendMessageToNoder(const std::string & noder_name, const s16 msgid, const void * data, const s32 size) = 0;

    virtual void sendProtoToNoder(const s32 noderid, const s16 msgid, const ::google::protobuf::Message & body) {
        const s32 size = body.ByteSize();
        char * temp = (char *)alloca(size);
        if (!body.SerializeToArray(temp, size)) {
            tassert(false, "body format error");
            return;
        }

        sendMessageToNoder(noderid, msgid, temp, size);
    }

    virtual void sendProtoToNoder(const s32 noderid, const std::string & noder_name, const s16 msgid, const ::google::protobuf::Message & body) {
        const s32 size = body.ByteSize();
        char * temp = (char *)alloca(size);
        if (!body.SerializeToArray(temp, size)) {
            tassert(false, "body format error");
            return;
        }

        sendMessageToNoder(noderid, noder_name, msgid, temp, size);
    }

    virtual void sendProtoToNoder(const std::string & noder_name, const s16 msgid, const ::google::protobuf::Message & body) {
        const s32 size = body.ByteSize();
        char * temp = (char *)alloca(size);
        if (!body.SerializeToArray(temp, size)) {
            tassert(false, "body format error");
            return;
        }

        sendMessageToNoder(noder_name, msgid, temp, size);
    }

protected:
    virtual void registerClientProto(s16 msgid, iProtoer<iNoderSession> & protoer) = 0;
};

#define register_node_ev(node, id, fun) node->RegisterEvent(id, fun, #fun)
#define register_noder_c_proto(node, id, fun) node->RegisterClientProto(id, fun, #fun);

#define set_noder_all_c_proto_callback(node, fun) node->SetAllClientProtoCallback(fun, #fun)

#define relay_server_msg_to_client(node, account, id, body, nsid, nsname) { \
    oRelayServerMessageToClient relay; \
    relay.set_messageid(id); \
    relay.add_account(account); \
    \
    char * temp = (char *)alloca(body.ByteSize()); \
    if (body.SerializeToArray(temp, body.ByteSize())) { \
        relay.set_body(temp, body.ByteSize()); \
        node->SendProtoToNoder(nsid, nsname, eNProtoID::RelayServerMessageToClient, relay); \
    }\
}

#endif //__iNoder_h__
