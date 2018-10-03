#ifndef __header_h__
#define __header_h__

#include "iGame.h"
#include "iEvent.h"
#include "iScene.h"
#include "iModel.h"
#include "iCards.h"
#include "iRedis.h"
#include "iPublic.h"
#include "iConfiger.h"

#include "ev.h"
#include "define.h"
#include "redis_def.h"

#include "game.pb.h"

#include "json/json.h"

extern api::iCore * g_core;

extern iEvent * g_event;
extern iScene * g_scene;
extern iModel * g_model;
extern iRedis * g_redis;
extern iConfig * g_config;

namespace timer {
    enum id {
        game_ready_delay = 1,
        update_role_position = 2,
        update_combiner_position = 3,
        combat_info_broadcast = 4,
        game_over = 5,
    };

    namespace config {
        const s32 game_ready_delay = 10 * SECOND;
        const s32 update_position_interval = 20;
        const s32 combat_info_broadcast_interval = 50;

        const s32 debug_role_position_interval = 1000;
    }
}

#define role_point_step 10

#endif //__header_h__
