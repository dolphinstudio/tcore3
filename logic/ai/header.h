#ifndef __AccountHeader_h__
#define __AccountHeader_h__

#include "iAi.h"
#include "iEvent.h"
#include "iModel.h"
#include "iGame.h"
#include "iConfiger.h"
#include "iOrbit.h"

#include "ev.h"
#include "define.h"

#include "game.pb.h"

extern iEvent * g_event;
extern iModel * g_model;
extern iGame * g_game;
extern iConfig * g_config;
extern iOrbit * g_orbit;

namespace timer {
    enum id {
        ai_play
    };

    namespace config {
        static const s32 ai_play = 5.01f * SECOND;
    }
}

#endif //__AccountHeader_h__
