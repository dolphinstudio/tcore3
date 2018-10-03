#ifndef __header_h__
#define __header_h__

#include "iScene.h"
#include "iLogicer.h"
#include "iEvent.h"
#include "iModel.h"
#include "iConfiger.h"

#include "UUID.h"

#include "ev.h"

#include "game.pb.h"

extern api::iCore * g_core;
extern iEvent * g_event;
extern iLogicer * g_logicer;
extern iModel * g_model;
extern iConfig * g_config;

#endif //__header_h__
