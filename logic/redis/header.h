#ifndef __header_h__
#define __header_h__

#ifdef WIN32
#include <winsock.h>
#endif //WIN32

#include "iRedis.h"
#include "tools.h"
#include "cthread.h"
#include "tqueue.h"
#include "tpool.h"
#include "autolock.h"

#include "hiredis/hiredis.h"

using namespace tools;
using namespace tcore;
using namespace tlib;
extern api::iCore * g_core;

extern tpool<redisdata> g_value_pool;

namespace timer {
    enum id {
        async_io_check = 0,
    };

    namespace config {
        static const s32 async_io_check_interval = 100;
    }
}

class section {
public:
    section(const s64 account) : _min(account), _max(account), _is_section(false) {

    }

    section(const s64 min, const s64 max) : _min(min), _max(max), _is_section(true) {
        tassert(min <= max, "wtf");
    }

    bool operator<(const section & section) const {
        //tassert((_min > section._min) == (_max > section._max) && (_min != section._min) == (_max != section._max), "wtf");
        return _min < section._min && _max < section._max;
    }


private:
    const bool _is_section;

    const s64 _min;
    const s64 _max;
};

class rediser;
extern rediser * g_master_redis;
extern std::map<section, vector<rediser *>> g_rediser_map;

#endif //__header_h__
