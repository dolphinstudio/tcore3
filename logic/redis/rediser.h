#ifndef __rediser_h__
#define __rediser_h__

#include "header.h"

struct redisContext;


enum eAsyncRedisIOType {
    get,
    set,
};

struct oAsyncRedisIO {
    eAsyncRedisIOType _type;
    std::string _key;
    redisdata * _value;
    union {
        void(*_async_callback)(bool issuccessd, const std::string & key, const redisdata & value, const s64 context);
    };
    bool _res;
    s64 _context;
};

class rediser : public iRediser, public api::iTimer, public cthread {
public:
    virtual ~rediser();

    rediser(const char * ip, const s32 port, const char * passwd);
    bool initialize();

    virtual void onStart(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}
    virtual void onTimer(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick);
    virtual void onEnd(api::iCore * core, const s32 id, const api::iContext & context, bool nonviolent, const s64 tick) {}
    virtual void onPause(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}
    virtual void onResume(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}

    virtual void terminate();
    virtual void run();

    virtual bool set(const char * key, const void * value, const s32 len);
    virtual bool get(const char * key, OUT redisdata & value);
    virtual bool get(const char * key, OUT::google::protobuf::Message & value);
    virtual bool del(const char * key);

    virtual bool asyncSet(const char * key, const void * value, const s32 len, const s64 context, void(*fun)(bool issuccessd, const std::string & key, const redisdata & value, const s64 context));
    virtual bool asyncGet(const char * key, const s64 context, void(*fun)(bool issuccessd, const std::string & key, const redisdata & value, const s64 context));

private:
    const std::string _passwd;
    const std::string _ip;
    const s32 _port;

    tlib::mutexlock _lock;
    redisContext * _context;

    tqueue<oAsyncRedisIO> _async_io_queue_in;
    tqueue<oAsyncRedisIO> _async_io_queue_out;
};

#endif //__rediser_h__
