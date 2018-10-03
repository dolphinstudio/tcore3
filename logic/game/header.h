#ifndef __header_h__
#define __header_h__

#include "iGame.h"
#include "iLogicer.h"
#include "iEvent.h"
#include "iScene.h"
#include "iModel.h"
#include "iCards.h"
#include "iPublic.h"
#include "iScore.h"
#include "iRobit.h"
#include "iRole.h"
#include "iOrbit.h"
#include "iRedis.h"
#include "iCombiner.h"
#include "iConfiger.h"
#include "iStorage.h"

#include "ev.h"
#include "define.h"
#include "redis_def.h"

#include "game.pb.h"

#include "json/json.h"

extern api::iCore * g_core;

extern iEvent * g_event;
extern iLogicer * g_logicer;
extern iScene * g_scene;
extern iModel * g_model;
extern iConfig * g_config;
extern iCards * g_cards;

extern iRole * g_role;
extern iCombiner * g_combiner;
extern iOrbit * g_orbit;

extern iRedis * g_redis;

extern iStorage * g_storage;

namespace timer {
    enum id {
        game_ready_delay = 1,
        update_role_position = 2,
        update_combiner_position = 3,
        combat_info_broadcast = 4,
        game_over = 5,

        debug_role_position
    };

    namespace config {
        const s32 game_ready_delay = 10 * SECOND;
        const s32 update_position_interval = 30;
        const s32 combat_info_broadcast_interval = 100;
        const s32 debug_role_position_interval = 1000;
    }
}

#define role_point_step 10
#define is_over_finish_pos(y, len) (y >= len || y <= 0)
#endif //__header_h__
