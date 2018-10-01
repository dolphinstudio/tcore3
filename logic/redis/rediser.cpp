#include "rediser.h"

rediser::~rediser() {
    if (_context) {
        redisFree(_context);
        _context = NULL;
    }

    g_core->killTimer(this, timer::id::async_io_check);
}

rediser::rediser(const char * ip, const s32 port, const char * passwd) : _ip(ip), _port(port), _passwd(passwd), _context(NULL) {}

bool rediser::initialize() {
    tlib::autolock autolock(&_lock);
    if (_context) {
        redisFree(_context);
        _context = NULL;
    }

    _context = redisConnect(_ip.c_str(), _port);
    if (REDIS_OK != _context->err) {
        error(g_core, "connect redis %s:%d error", _ip.c_str(), _port);
        redisFree(_context);
        _context = NULL;
        return false;
    }

    if (_passwd.length() > 0) {
        redisReply * res = (redisReply *)redisCommand(_context, "auth %s", _passwd.c_str());
        bool b = (res && res->type == REDIS_REPLY_STATUS && tools::memery::safeStrcmp(res->str, "OK")) ? true : false;
        if (res) {
            freeReplyObject(res);
        }
        return b;
    }

    start();
    start_timer(g_core, this, timer::id::async_io_check, timer::config::async_io_check_interval, forever, timer::config::async_io_check_interval, 0);
    return true;
}

void rediser::onTimer(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {
    switch (id) {
    case timer::id::async_io_check: {
        oAsyncRedisIO io;
        while (_async_io_queue_out.tryPull(io)) {
            io._async_callback(io._res, io._key, *io._value, io._context);
            recover_to_pool(g_value_pool, io._value);
        }
        break;
    }
    }
}

void rediser::terminate() {

}

void rediser::run() {
    while (true) {
        oAsyncRedisIO io;
        if (!_async_io_queue_in.tryPull(io)) {
            msleep(10);
        } else {
            switch (io._type) {
            case eAsyncRedisIOType::set: {
                s32 len;
                const void * value = io._value->get(len);
                if (nullptr == value || len <= 0) {
                    io._res = false;
                    _async_io_queue_out.pull(io);
                    break;
                }

                io._res = set(io._key.c_str(), value, len);
                _async_io_queue_out.push(io);
                break;
            }
            case eAsyncRedisIOType::get: {
                io._res = get(io._key.c_str(), *io._value);
                _async_io_queue_out.push(io);
                break;
            }
            }
        }
    }
}

bool rediser::set(const char * key, const void * value, const s32 len) {
    redisReply * res = (redisReply *)redisCommand(_context, "set %s %b", key, value, (size_t)len);
    bool b = (res && res->type == REDIS_REPLY_STATUS && tools::memery::safeStrcmp(res->str, "OK")) ? true : false;
    if (res) {
        freeReplyObject(res);
    }

    return b;
}

bool rediser::get(const char * key, OUT redisdata & value) {
    redisReply * res = (redisReply *)redisCommand(_context, "get %s", key);
    while (NULL == res) {
        while (false == initialize()) {
            msleep(10);
        }
        res = (redisReply *)redisCommand(_context, "get %s", key);
    }

    bool b = (res && res->type == REDIS_REPLY_STRING) ? true : false;
    if (b) {
        value.set(res->str, res->len);
    }

    if (res) {
        freeReplyObject(res);
    }

    return b;
}

bool rediser::get(const char * key, OUT google::protobuf::Message & data) {
    redisReply * res = (redisReply *)redisCommand(_context, "get %s", key);
    while (NULL == res) {
        while (false == initialize()) {
            msleep(10);
        }
        res = (redisReply *)redisCommand(_context, "get %s", key);
    }

    bool b = (res && res->type == REDIS_REPLY_STRING) ? true : false;
    if (b) {
        if (!data.ParseFromArray(res->str, res->len)) {
            b = false;
        }
    }

    if (res) {
        freeReplyObject(res);
    }

    return b;
}

bool rediser::del(const char * key){
    redisReply * res = (redisReply *)redisCommand(_context, "del %s", key);
    if (!res || res->type != REDIS_REPLY_INTEGER) {
        (NULL == res) ? (void()) : (freeReplyObject(res));
        return false;
    }

    freeReplyObject(res);
    return true;
}

bool rediser::asyncSet(const char * key, const void * value, const s32 len, const s64 context, void(*fun)(bool issuccessd, const std::string & key, const redisdata & value, const s64 context)) {
    oAsyncRedisIO async;
    async._type = eAsyncRedisIOType::set;
    async._key = key;
    async._value = create_from_pool(g_value_pool);
    async._value->set(value, len);
    async._async_callback = fun;
    async._context = context;

    return _async_io_queue_in.tryPush(async);
}

bool rediser::asyncGet(const char * key, const s64 context, void(*fun)(bool issuccessd, const std::string & key, const redisdata & value, const s64 context)) {
    oAsyncRedisIO async;
    async._type = eAsyncRedisIOType::get;
    async._key = key;
    async._value = create_from_pool(g_value_pool);
    async._async_callback = fun;
    async._context = context;

    return _async_io_queue_in.tryPush(async);
}
