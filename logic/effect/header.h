#ifndef __header_h__
#define __header_h__

#include "iEffect.h"
#include "iModel.h"
#include "iEvent.h"
#include "iScene.h"
#include "iConfiger.h"

#include "define.h"
#include "ev.h"
#include "game.pb.h"

extern api::iCore * g_core;

extern iModel * g_model;
extern iEvent * g_event;
extern iScene * g_scene;
extern iConfig * g_config;

enum eSceneEffectType {
    block_orbit = 0,
    block_scene = 1,
};

namespace timer {
    enum id {
        unblock_orbit = 1,
    };
}

#endif //__Header_h__
