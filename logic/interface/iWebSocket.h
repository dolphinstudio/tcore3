#ifndef __iWebSocket_h__
#define __iWebSocket_h__

#include "api.h"

enum eFrameType {
    error_frame = 0xFF00,
    incomplete_frame = 0xFE00,

    opening_frame = 0x3300,
    closing_frame = 0x3400,

    incomplete_text_frame = 0x01,
    incomplete_binary_frame = 0x02,

    text_frame = 0x81,
    binary_frame = 0x82,

    ping_frame = 0x19,
    pong_frame = 0x1A
};

class iWBSessionDelegate;

class iWBSession : public tcore::api::iTcpSession {
public:
    virtual ~iWBSession() {}
    virtual iWBSessionDelegate * getDelegate() = 0;
    virtual void sendmessage(const void * data, const s32 size, const eFrameType frametype) = 0;
};

class iWBSessionDelegate {
public:
    virtual ~iWBSessionDelegate() {}
    iWBSessionDelegate() : _session(nullptr) {}

    virtual void onConnect() = 0;
    virtual void onDisconnect() = 0;
    virtual void onRecv(const char * data, const s32 size) = 0;

    virtual void send(const void * data, const s32 size, const eFrameType frametype) {
        if (_session) {
            _session->sendmessage(data, size, frametype);
        }
    }

    virtual void close() {
        _session->close();
    }

    iWBSession * const _session;
};

class iWBServerDelegate {
public:
    virtual ~iWBServerDelegate() {}
    
    virtual iWBSessionDelegate * onMallocSessionDelegate() = 0;
};

class iWebSocket : public iModule {
public:
    virtual ~iWebSocket() {}

    virtual bool launchwebSocketServer(iWBServerDelegate * delegate, const char * ip, const s32 port) = 0;
};

#endif //__iWebSocket_h__
