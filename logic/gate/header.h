#ifndef __GateHeader_h__
#define __GateHeader_h__

#include "iGate.h"
#include "iWebSocket.h"
#include "iEvent.h"
#include "iNoder.h"
#include "iRedis.h"

#include "iReference.h"

#include "game.pb.h"

#include "define.h"
#include "ev.h"
#include "redis_def.h"

#include "json/json.h"

#include "UUID.h"

#include <unordered_map>
using namespace std;

extern api::iCore * g_core;

extern iWebSocket * g_websocket;
extern iEvent * g_event;
extern iRedis * g_redis;

typedef unordered_map<s16, messager> messager_map;
typedef unordered_map<s64, messager_map> messager_pool;

extern messager_pool g_messager_pool;

class wsDelegate;
typedef unordered_map<s64, wsDelegate *> delegate_map;
extern delegate_map g_delegate_map;

class gate;
extern gate * g_gate;

extern iNoder * g_noder;
extern iNoderSession * g_relation;
extern iNoderSession * g_match;

namespace timer {
    enum id {
        delay_close,
    };

    namespace config {
        static const s32 delay_close = 1 * SECOND;
    }
}

#endif //__GateHeader_h__
