#ifndef __Header_h__
#define __Header_h__

#include "iModel.h"
#include "iBuff.h"
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

/*
buff����:
0:�ı�����
1:����
2:������ײ
3.����
*/

enum eBuffType {
    changeattr = 0,
    stealth = 1,
    discollision = 2,
    congelation = 3
};

#define immortal -1

/*
��������:
0:����
*/

enum eAttrType {
    weight = 0,
};

namespace timer {
    enum id {
        recover_buff,
        change_attr,
    };
}

#endif //__Header_h__
