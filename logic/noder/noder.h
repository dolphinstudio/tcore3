#ifndef __noder_h__
#define __noder_h__

#include "header.h"

class noder : public iNoder, public api::iTcpServer, public iSession, public api::iTimer {
public:
    virtual ~noder() {}

    virtual bool initialize(api::iCore * core);
    virtual bool launch(api::iCore * core);
    virtual bool destroy(api::iCore * core);

    virtual const std::string & getname() { return g_noder_name; }
    virtual const s32 getarea() { return g_noder_area; }
    virtual const s32 getid() { return g_noder_id; }
    virtual const std::string & getip() { return g_noder_ip; }
    virtual const s32 getport() { return g_noder_port; }

    virtual iNoderSession * getNoderSession(const std::string & name, const s32 id);
    virtual void reportLoad(const s16 load, const std::string & target_name);
    virtual iNoderSession * getLoadlessNoder(const std::string & name);

    virtual void setAllClientProtoCallback(const fAllClientProtoCallback fun, const char * debug);

    virtual void registerEvent(const eNoderEvent id, const fNoderEvent ev, const char * debug);
    virtual void sendMessageToNoder(const s32 noderid, const s16 msgid, const void * data, const s32 size);
    virtual void sendMessageToNoder(const s32 noderid, const std::string & noder_name, const s16 msgid, const void * data, const s32 size);
    virtual void sendMessageToNoder(const std::string & noder_name, const s16 msgid, const void * data, const s32 size);
    
    virtual api::iTcpSession * onMallocConnection(api::iCore * core, const char * remote_ip, const s32 remote_port);
    virtual void onError(api::iCore * core, api::iTcpSession * session);
    virtual void onRelease(api::iCore * core);

    virtual void onConnected(api::iCore * core);
    virtual void onDisconnect(api::iCore * core);
    virtual void onConnectFailed(api::iCore * core);

    virtual void onStart(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}
    virtual void onTimer(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick);
    virtual void onEnd(api::iCore * core, const s32 id, const api::iContext & context, bool nonviolent, const s64 tick) {}
    virtual void onPause(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}
    virtual void onResume(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}

    void static onAssigneID(api::iCore * core, iSession * session, s64 context, const oAssigneID & body);
    void static onBroadcastNoder(api::iCore * core, iSession * session, s64 context, const oBroadcastNoder & body);

protected:
    virtual void registerClientProto(s16 msgid, iProtoer<iNoderSession> & protoer);
};

#endif //__Noder_h__
