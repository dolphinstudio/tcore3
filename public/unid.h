#ifndef __unid_h__
#define __unid_h__
#include "tools.h"

#define TWEPOCH u64(1435123996570l)
#define PREFIXID_BITS u8(17)
#define MAX_AREA u16(~(-1 << 12))
#define MAX_TYPE u8(~(-1 << 4))
#define MAX_NODE u8(~(-1 << 5))
#define SEQUENCE_BITS u8(5)
#define SEQUENCE_MASK u8(~(-1 << SEQUENCE_BITS))
#define PREFIXID_SHIFT SEQUENCE_BITS
#define TIMESTAMP_SHIFT u8(26)

#pragma pack(push, 1)
class unid {
public:
    static u64 CreateID(u16 area, u8 type, u8 node) {
        Instance()->Init(area, type, node);
        return Instance()->GetID();
    }

private:
    static unid * Instance() {
        static unid oc;
        return &oc;
    }

    bool Init(u16 area, u8 type, u8 node) {
        if (area > MAX_AREA || type > MAX_TYPE || node > MAX_NODE) {
            return false;
        }
        prefixId = u32(area) << 9 | u32(type) << 5 | node;
        return true;
    }

    u64 GetID() {
        u64 timestamp = tools::time::GetTimeMillisecond();
        if (lastTimestamp == timestamp) {
            sequence = (sequence + 1) & SEQUENCE_MASK;
            if (sequence == 0) {
                timestamp = TilNextMillis(lastTimestamp);
            }
        } else {
            sequence = 0;
        }
        lastTimestamp = timestamp;
        return ((timestamp - TWEPOCH) << TIMESTAMP_SHIFT) | (prefixId << PREFIXID_SHIFT) | sequence;
    }

    u64 TilNextMillis(s64 lastTimestamp) {
        u64 timestamp = tools::time::GetTimeMillisecond();
        while (timestamp <= lastTimestamp) {
            timestamp = tools::time::GetTimeMillisecond();
        }
        return timestamp;
    }

    u32 prefixId;
    u8 sequence;
    u64 lastTimestamp;
};
#pragma pack(pop) 

#endif //__unid_h__
