#ifndef __webSocketSession_h__
#define __webSocketSession_h__

#include "header.h"

class webSocketSession : public iWBSession {
public:
    virtual ~webSocketSession() {}
    webSocketSession(iWBSessionDelegate * delegate);

    virtual void sendmessage(const void * data, const s32 size, const eFrameType frametype);
    virtual int onRecv(api::iCore * core, const void * context, const int size);
    virtual void onConnected(api::iCore * core);
    virtual void onDisconnect(api::iCore * core);
    virtual void onConnectFailed(api::iCore * core);

    virtual iWBSessionDelegate * getDelegate() { return _delegate; }

private:
    bool tryShake(const void * data, const s32 size);

    void fetchFin(char * msg, int & pos);
    void fetchOpcode(char * msg, int & pos);
    void fetchMask(char * msg, int & pos);
    void fetchMaskingKey(char * msg, int & pos);
    void fetchPayloadLength(char * msg, int & pos);
    void fetchPayload(char * msg, int & pos);

    void reset();

private:
    iWBSessionDelegate * const _delegate;
    bool _shaked;
    std::map<std::string, std::string> _header_map;

    u8 _fin;
    u8 _opcode;
    u8 _mask;
    u8 _masking_key[4];
    u64 _payload_length;
    char _payload[8192];
};

#endif //__webSocketSession_h__
