#ifndef __iRedis_h__
#define __iRedis_h__
#include "api.h"
#include "tools.h"

#include "google/protobuf/message.h"
using namespace tcore;

class redisdata {
public:
    redisdata() : _data(nullptr), _len(0) {}
    virtual ~redisdata() {
        if (_data && _data != _value) {
            DEL _data;
        }
    }

    void set(const void * data, const s32 len) {
        tassert(_data == nullptr, "wtf");
        if (_data != nullptr) {
            return;
        }

        (len > sizeof(_value)) ? _data = NEW char[len] : _data = _value;
        _len = len;
        tools::memery::safeMemcpy(_data, len, data, len);
    }

    const void * get(OUT s32 & len) const {
        len = _len;
        return _data;
    }

private:
    char _value[512];
    void * _data;
    s32 _len;
};

class iRediser {
public:
    virtual ~iRediser() {}

    virtual bool set(const char * key, const void * value, const s32 len) = 0;
    virtual bool get(const char * key, OUT redisdata & value) = 0;
    virtual bool get(const char * key, OUT::google::protobuf::Message & value) = 0;
    virtual bool del(const char * key) = 0;

    virtual bool asyncSet(const char * key, const void * value, const s32 len, const s64 context, void(*fun)(bool issuccessd, const std::string & key, const redisdata & value, const s64 context)) = 0;
    virtual bool asyncGet(const char * key, const s64 context, void(*fun)(bool issuccessd, const std::string & key, const redisdata & value, const s64 context)) = 0;
};

class iRedis : public iModule{
public:
    virtual ~iRedis() {}

    virtual iRediser * getMasterRediser() = 0;
    virtual iRediser * getRediser(const s64 account) = 0;
};

#endif //__iRedis_h__
