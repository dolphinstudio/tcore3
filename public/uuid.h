#ifndef __uuid_h__
#define __uuid_h__

#include "tools.h"

class uuid {
public:
    static uuid & getInstance() {
        static uuid oc;
        return oc;
    }

    inline void setmask(u16 id) {
        mask = id;
    }

    inline u64 create() {
        u64 tick = tools::time::getMillisecond();
        index++;
        if (lastTimestamp != tick) {
            index = 0;
            lastTimestamp = tick;
        }
        else {
            if (index == 0) {
                tick = tilNextMillis(lastTimestamp);
            }
        }
        return mask << 48 | ((tick << 24) >> 16) | index;
    }

private:
    uuid() : mask(0), index(0), lastTimestamp(tools::time::getMillisecond()) {}

    u64 tilNextMillis(s64 lastTimestamp) {
        u64 timestamp = tools::time::getMillisecond();
        while (timestamp <= lastTimestamp) {
            timestamp = tools::time::getMillisecond();
        }
        return timestamp;
    }

    u64 mask;
    u8 index;
    u64 lastTimestamp;
};

#endif //__uuid_h__
