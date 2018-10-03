#ifndef __api_h__
#define __api_h__

#include "multisys.h"
#include <string>

#define invalid_port 0
#define package_max_size 65507

class iModule;

namespace tcore {
    namespace api {
        struct oAddress {
            std::string _ip;
            s32 _port;
            oAddress() : _ip(""), _port(invalid_port) {}
            oAddress(const std::string & ip, const s32 port) :_ip(ip), _port(port) {}
        };

        class iCore;

        class iPipe {
        public:
            virtual ~iPipe() {}

            virtual void cache() = 0;
            virtual void load() = 0;
            virtual void close() = 0;
        };

        class iUdpPipe : public iPipe {
        public:
            virtual ~iUdpPipe() {}
            virtual void sendto(const char * ip, const s32 port, const void * context, const s32 size) = 0;
        };

        class iTcpPipe : public iPipe {
        public:
            virtual ~iTcpPipe() {}
            virtual void send(const void * data, const s32 size, bool immediately) = 0;
        };

        class iSocket {
        public:
            virtual ~iSocket() {}

            iSocket() : _address("", invalid_port) {}

            oAddress _address;
        };

        class iUdpSocket : public iSocket {
        public:
            iUdpSocket() : _pipe(nullptr) {}
            virtual ~iUdpSocket() {}

            iUdpPipe * const _pipe;
        };

        class iTcpSocket : public iSocket {
        public:
            virtual ~iTcpSocket() {}
            iTcpSocket() : _pipe(nullptr) {}

            iTcpPipe * const _pipe;
        };

        class iUdpSession : public iUdpSocket {
        public:
            virtual ~iUdpSession() {}

            virtual void onCreate(bool success) = 0;
            virtual void onRecv(iCore * core, const char * ip, const s32 port, const char * context, const int size) = 0;
            virtual void onClose(iCore * core) = 0;

            void close() const {
                if (this && _pipe) { _pipe->close(); }
            }

            inline void sendto(const char * ip, const s32 port, const void * context, const int size) const {
                if (this && _pipe) { _pipe->sendto(ip, port, context, size); }
            }

            inline void cache() {
                if (this && _pipe) { _pipe->cache(); }
            }

            inline void load() {
                if (this && _pipe) { _pipe->load(); }
            }
        };

        class iAccepter {
        public:
            virtual ~iAccepter() {}
            virtual void release() = 0;
        };


        class iTcpSession : public iTcpSocket {
        public:
            virtual ~iTcpSession() {}
            iTcpSession() : _initiative(false) {}
            virtual int onRecv(iCore * core, const void * context, const int size) = 0;
            virtual void onConnected(iCore * core) = 0;
            virtual void onDisconnect(iCore * core) = 0;
            virtual void onConnectFailed(iCore * core) = 0;

            void close() const {
                if (this && _pipe) { _pipe->close(); }
            }

            inline void send(const void * context, const int size, bool immediately = true) const {
                if (this && _pipe) { _pipe->send(context, size, immediately); }
            }

            inline void cache() {
                if (this && _pipe) { _pipe->cache(); }
            }

            inline void load() {
                if (this && _pipe) { _pipe->load(); }
            }

            const bool _initiative;
        };

        class iUdpServer {
        public:
            iUdpServer() : _ac(nullptr) {}
            virtual ~iUdpServer() {}

            virtual iUdpSession * onMallocConnection(iCore * core, const char * ip, const s32 port) = 0;
            virtual void onRelease(iCore * core) = 0;

            virtual void close() { _ac->release(); }

            iAccepter * _ac;
        };

        class iTcpServer {
        public:
            iTcpServer() : _ac(nullptr) {}
            virtual ~iTcpServer() {}

            virtual iTcpSession * onMallocConnection(iCore * core, const char * remote_ip, const s32 remote_port) = 0;
            virtual void onError(iCore * core, iTcpSession * session) = 0;
            virtual void onRelease(iCore * core) = 0;

            virtual void close() { _ac->release(); }

            iAccepter * const _ac;
        };


        class iUdpBroadcaster {
        public:
            virtual void broadcast(const void * context, const s32 size) = 0;
        };

        struct iContext {
            union {
                const void * _context_point;
                const s64 _context_mark;
            };
            iContext(const void * point) : _context_point(point) {}
            iContext(const s64 mark) : _context_mark(mark) {}
        };

        class iTimer {
        public:
            virtual ~iTimer() {}

            virtual void onStart(iCore * core, const s32 id, const iContext & context, const s64 tick) = 0;
            virtual void onTimer(iCore * core, const s32 id, const iContext & context, const s64 tick) = 0;
            virtual void onEnd(iCore * core, const s32 id, const iContext & context, bool nonviolent, const s64 tick) = 0;
            virtual void onPause(iCore * core, const s32 id, const iContext & context, const s64 tick) = 0;
            virtual void onResume(iCore * core, const s32 id, const iContext & context, const s64 tick) = 0;
        };

        class iHttpRequest {
        public:
            virtual ~iHttpRequest() {}
            virtual void postParam(const char * key, const char * value) = 0;
            virtual void doRequest() = 0;
        };

        class iHttpResponse {
        public:
            iHttpResponse() : _reference(0) {}
            virtual ~iHttpResponse() {}
            virtual void onError(const s32 id, const s32 error, const iContext & context) = 0;
            virtual void onResponse(const s32 id, const void * data, const s32 size, const iContext & context) = 0;
            virtual bool isRequesting() { return _reference != 0; }

            s32 _reference;
        };

        class iCore {
        public:
            virtual ~iCore() {}

            virtual iModule * findModule(const std::string & name) = 0;

            virtual const char * getEnv() = 0;
            virtual const char * getArgs(const char * name) = 0;

            virtual void setCorename(const char * name) = 0;
            virtual const char * getCorename() = 0;

#       define any_eth "0.0.0.0"
            virtual bool launchUdpSession(iUdpSession * session, const char * ip, const s32 port) = 0;
            virtual bool launchTcpSession(iTcpSession * client, const char * ip, const int port, int max_ss, int max_rs) = 0;
            virtual bool launchTcpServer(iTcpServer * server, const char * ip, const int port, int max_ss,  int max_rs) = 0;

            virtual iHttpRequest * getHttpRequest(const s64 account, const s64 id, const char * url, iHttpResponse * response, const iContext & context) = 0;

#       define forever -1
#       define start_timer(core, timer, id, delay, count, interval, context) core->startTimer(timer, id, delay, count, interval, (s64)context, __FILE__, __LINE__);
            virtual void startTimer(iTimer * timer, const s32 id, s64 delay, s32 count, s64 interval, const iContext context, const char * file, const s32 line) = 0;
            virtual void killTimer(iTimer * timer, const s32 id, const iContext context = (s64)0) = 0;
            virtual void pauseTimer(iTimer * timer, const s32 id, const iContext context = (s64)0) = 0;
            virtual void resumeTimer(iTimer * timer, const s32 id, const iContext context = (s64)0) = 0;
            virtual void traceTimer() = 0;

            virtual void logSync(const s64 tick, const char * log, const bool echo) = 0;
            virtual void logAsync(const s64 tick, const char * log, const bool echo) = 0;
            virtual void setSyncFilePrefix(const char * prefix) = 0;
            virtual void setAsyncFilePrefix(const char * prefix) = 0;
        };

    }
}

#ifdef _DEBUG
#define debug(core, format, ...) { \
    printf("debug:%s:%d"#format, __FILE__, __LINE__, ##__VA_ARGS__); \
    printf("\n");}

#define trace(core, format, ...) { \
    char log[4096] = {0}; \
    safesprintf(log, sizeof(log), "trace:%s:%d"#format, __FILE__, __LINE__, ##__VA_ARGS__); \
    printf("%s\n", log); \
    core->logAsync(tools::time::getMillisecond(), log, false);}

#define error(core, format, ...) { \
    char log[4096] = {0}; \
    safesprintf(log, sizeof(log), "error:%s:%d"#format, __FILE__, __LINE__, ##__VA_ARGS__); \
    printf("%s\n", log); \
    core->logSync(tools::time::getMillisecond(), log, true);}

#define imp(core, format, ...) { \
    char log[4096] = {0}; \
    safesprintf(log, sizeof(log), "%s:%d"#format, __FILE__, __LINE__, ##__VA_ARGS__); \
    printf("%s\n", log); \
    core->logSync(tools::time::getMillisecond(), log, true);}

#else 
#define debug(core, format, ...) (void)0;

#define trace(core, format, ...) { \
    char log[4096] = {0}; \
    safesprintf(log, sizeof(log), "trace:%s:%d"#format, __FILE__, __LINE__, ##__VA_ARGS__); \
    core->logAsync(tools::time::getMillisecond(), log, false);}

#define error(core, format, ...) { \
    char log[4096] = {0}; \
    tassert(false, format, ##__VA_ARGS__); \
    safesprintf(log, sizeof(log), "error:%s:%d"#format, __FILE__, __LINE__, ##__VA_ARGS__); \
    core->logSync(tools::time::getMillisecond(), log, false);}

#define imp(core, format, ...) { \
    char log[4096] = {0}; \
    safesprintf(tools::time::getMillisecond(), log, sizeof(log), "%s:%d"#format, __FILE__, __LINE__, ##__VA_ARGS__); \
    core->logSync(log, false);}
#endif //_DEBUG


#define core_version 0x0001

class iModule {
public:
    virtual ~iModule() {}
    virtual bool initialize(tcore::api::iCore * core) = 0;
    virtual bool launch(tcore::api::iCore * core) = 0;
    virtual bool destroy(tcore::api::iCore * core) = 0;

public:
    iModule() : _next(nullptr) {}

    inline void setNext(iModule * & pModule) { _next = pModule; }
    inline iModule * getNext() { return _next; }
    inline void setName(const char * name) { _name = name; }
    inline const char * getName() const { return _name.c_str(); }
    inline int getVersion() const { return core_version; }

private:
    iModule * _next;
    std::string _name;
};

typedef iModule * (*getModule)(void);

#define create_module(name) \
class factroy##name {    \
public:    \
    factroy##name(iModule * & pModule) { \
        iModule * pModule##name = NEW name; \
        pModule##name->setName(#name); \
        pModule##name->setNext(pModule); \
        pModule = pModule##name; \
    } \
}; \
factroy##name factroy##name(s_modules);


#ifdef WIN32
#define get_dll_instance \
    static iModule * s_modules = nullptr; \
    extern "C" __declspec(dllexport) iModule * __cdecl getModules() {\
        return s_modules; \
    }
#else
#define get_dll_instance \
    static iModule * s_modules = nullptr; \
    extern "C" iModule * getModules() { \
        srand(tools::time::getMillisecond()); \
        return s_modules; \
    }
#endif //WIN32

#endif //__api_h__
