#ifndef __Header_h__
#define __Header_h__

#include "iSkill.h"
#include "iModel.h"
#include "iEvent.h"
#include "iBuff.h"
#include "iScene.h"
#include "iEffect.h"
#include "iConfiger.h"

#include "define.h"
#include "ev.h"
#include "game.pb.h"

extern api::iCore * g_core;
extern iModel * g_model;
extern iEvent * g_event;
extern iBuff * g_buff;
extern iScene * g_scene;
extern iEffect * g_effect;
extern iConfig * g_config;

enum eSkillType {
    add_buff = 0,
    clean_role = 1,
    copy_self = 2,
};

enum eCastType {
    delay_cast = 1,
    encounter_enemy = 2,
    join_team = 3,
};

enum eTargetType {
    self = 1,
    own_team = 2,
    enemy_team = 3,
    first_enemy = 4,
    orbit =  5,
    first_enemy_and_teammate = 6,
};

enum eAdvantageInferiority {
    default_cast = 0,
    team_advantage = 1,
    team_inferiority = 2,
    advantage_over_face_enemy = 3,
    inferiority_to_face_enemy = 4,
    only_one_enemy = 5,
};

enum eBuffTarget {
    /*��������
    1:�Լ�
    2:�з�
    3:����
    4.������
    5.·��*/

    add_self = 1,
    add_enemy = 2,
    add_teammate = 3,
    add_all = 4,
    add_orbit = 5
};

namespace timer {
    enum id {
        delay_cast = 1,
    };
}

#endif //__Header_h__
