#ifndef __auto_config_h__
#define __auto_config_h__
// tools from sisyphus 
#include <map>
#include <string>

#include "tools.h"
#include "tinyxml/tinyxml.h"

using namespace tools;
using namespace std;

struct oResource {
/*资源ID*/
    int _id;
/*作用类型
1:自己
2:敌方
3:队友
4.所有人*/
    int _type;
/*是否循环执行
0:不循环
1:循环*/
    int _loop;
/*生命周期
多久回收
单位毫秒
如果loop选项为不循环,这个选项作废,
在动画执行完成之后回收*/
    int _life_cycle;
};
struct oSkillAction {
/*动作名*/
    string _action;
/*类型
1:自己执行动作
2:敌方执行动作
3:队友执行动作*/
    int _type;
/*是否循环执行
0:不循环
1:循环*/
    int _loop;
};
struct oSkillBuff {
/*buffid*/
    int _buff;
/*作用类型
1:自己
2:敌方
3:队友
4.所有人
5.路线*/
    int _type;
};
struct o_config_advertise {
/*奖励id*/
    int _id;
/*奖励类型
0.金币
1.碎片
2.食物*/
    int _type;
/*奖励参数*/
    int _param;
/*奖励数量*/
    int _number;
};
extern std::map<int, o_config_advertise> g_config_advertise_map;
inline void init_config_advertise(const std::string & path) {
    std::string xmlpath;
    xmlpath << path << "/" << "advertise.xml";
    TiXmlDocument config;
    if (!config.LoadFile(xmlpath.c_str())) {
        tassert(false, "load config %s error", xmlpath.c_str());
        return;
    }

    TiXmlElement * root = config.RootElement();
    TiXmlElement * advertise = root->FirstChildElement("advertise");
    while(advertise) {
        o_config_advertise oadvertise;
        oadvertise._id = tools::stringAsInt(advertise->Attribute("id"));
        oadvertise._type = tools::stringAsInt(advertise->Attribute("type"));
        oadvertise._param = tools::stringAsInt(advertise->Attribute("param"));
        oadvertise._number = tools::stringAsInt(advertise->Attribute("number"));
        advertise = advertise->NextSiblingElement("advertise");
        g_config_advertise_map.insert(std::make_pair(oadvertise._id, oadvertise));
    }
}

struct o_config_buff {
/*buff的id*/
    int _id;
/*buff类型:
0:改变属性
1:隐身
2:无视碰撞
3.冰封*/
    int _type;
/*属性类型:
0:体重*/
    int _attr_type;
/*当buff类型为改变属性时生效
delay多久开始修改属性
单位毫秒*/
    int _delay;
/*buff持续时间
毫秒
-1.永久*/
    int _period;
/*修改属性间隔,
单位毫秒*/
    int _interval;
/*修改属性次数*/
    int _count;
/*每次修改属性的值
体重按百分比修改*/
    float _value;
/*去除buff的特殊条件
0.无
1.角色后方加入新队友*/
    string _cancel;
/*buff描述*/
    string _des;
/*资源*/
    vector<int> _resource;
};
extern std::map<int, o_config_buff> g_config_buff_map;
inline void init_config_buff(const std::string & path) {
    std::string xmlpath;
    xmlpath << path << "/" << "buff.xml";
    TiXmlDocument config;
    if (!config.LoadFile(xmlpath.c_str())) {
        tassert(false, "load config %s error", xmlpath.c_str());
        return;
    }

    TiXmlElement * root = config.RootElement();
    TiXmlElement * buff = root->FirstChildElement("buff");
    while(buff) {
        o_config_buff obuff;
        obuff._id = tools::stringAsInt(buff->Attribute("id"));
        obuff._type = tools::stringAsInt(buff->Attribute("type"));
        obuff._attr_type = tools::stringAsInt(buff->Attribute("attr_type"));
        obuff._delay = tools::stringAsInt(buff->Attribute("delay"));
        obuff._period = tools::stringAsInt(buff->Attribute("period"));
        obuff._interval = tools::stringAsInt(buff->Attribute("interval"));
        obuff._count = tools::stringAsInt(buff->Attribute("count"));
        obuff._value = tools::stringAsFloat(buff->Attribute("value"));
        obuff._cancel = buff->Attribute("cancel");
        obuff._des = buff->Attribute("des");
        {
            std::string str = buff->Attribute("resource");
            tools::osplitres arrays;
            tools::split(str, ";", arrays);
            for (s32 i = 0; i < arrays.size(); i++) {
                int value = tools::stringAsInt(arrays[i].c_str());
                obuff._resource.push_back(value);
            }
        }
        buff = buff->NextSiblingElement("buff");
        g_config_buff_map.insert(std::make_pair(obuff._id, obuff));
    }
}

struct o_config_food {
/*id*/
    int _id;
/*名称*/
    string _name;
/*增加的经验值*/
    int _exp;
/*资源*/
    int _res;
/*描述*/
    int _dsc;
};
extern std::map<int, o_config_food> g_config_food_map;
inline void init_config_food(const std::string & path) {
    std::string xmlpath;
    xmlpath << path << "/" << "food.xml";
    TiXmlDocument config;
    if (!config.LoadFile(xmlpath.c_str())) {
        tassert(false, "load config %s error", xmlpath.c_str());
        return;
    }

    TiXmlElement * root = config.RootElement();
    TiXmlElement * food = root->FirstChildElement("food");
    while(food) {
        o_config_food ofood;
        ofood._id = tools::stringAsInt(food->Attribute("id"));
        ofood._name = food->Attribute("name");
        ofood._exp = tools::stringAsInt(food->Attribute("exp"));
        ofood._res = tools::stringAsInt(food->Attribute("res"));
        ofood._dsc = tools::stringAsInt(food->Attribute("dsc"));
        food = food->NextSiblingElement("food");
        g_config_food_map.insert(std::make_pair(ofood._id, ofood));
    }
}

struct o_global_config {
/*角色产出CD时间(秒)*/
    int _create_role_timer_interval;
/*角色统一移动速度*/
    int _move_speed;
/*大力士出阵cd时间(毫秒)*/
    int _public_cd;
/*游戏局内角色队列大小*/
    int _role_queue_size;
/*单局时长（秒）*/
    int _game_time;
/*初始角色*/
    string _init_role;
/*最大出战角色数*/
    int _max_role_fighting;
/*刷新商城基础费用*/
    int _shop_refresh_cost_base;
/*刷新商城递增费用*/
    int _shop_refresh_cost_up;
/*刷新商城最大费用*/
    int _shop_refresh_cost_max;
/*刷新商城商品数*/
    int _shop_refresh_number;
/*刷新商城
间隔，秒*/
    int _shop_refresh_interval;
/*广告奖励间隔*/
    int _advertise_interval;
/*每日广告奖励次数*/
    int _advertise_daily_count;
};
extern o_global_config g_global_config;
inline void init_global_config(const std::string & path) {
    std::string xmlpath;
    xmlpath << path << "/" << "global.xml";
    TiXmlDocument config;
    if (!config.LoadFile(xmlpath.c_str())) {
        tassert(false, "load config %s error", xmlpath.c_str());
        return;
    }

    TiXmlElement * root = config.RootElement();
    TiXmlElement * global = root->FirstChildElement("global");
    if (global) {
        g_global_config._create_role_timer_interval = tools::stringAsInt(global->Attribute("create_role_timer_interval"));
        g_global_config._move_speed = tools::stringAsInt(global->Attribute("move_speed"));
        g_global_config._public_cd = tools::stringAsInt(global->Attribute("public_cd"));
        g_global_config._role_queue_size = tools::stringAsInt(global->Attribute("role_queue_size"));
        g_global_config._game_time = tools::stringAsInt(global->Attribute("game_time"));
        g_global_config._init_role = global->Attribute("init_role");
        g_global_config._max_role_fighting = tools::stringAsInt(global->Attribute("max_role_fighting"));
        g_global_config._shop_refresh_cost_base = tools::stringAsInt(global->Attribute("shop_refresh_cost_base"));
        g_global_config._shop_refresh_cost_up = tools::stringAsInt(global->Attribute("shop_refresh_cost_up"));
        g_global_config._shop_refresh_cost_max = tools::stringAsInt(global->Attribute("shop_refresh_cost_max"));
        g_global_config._shop_refresh_number = tools::stringAsInt(global->Attribute("shop_refresh_number"));
        g_global_config._shop_refresh_interval = tools::stringAsInt(global->Attribute("shop_refresh_interval"));
        g_global_config._advertise_interval = tools::stringAsInt(global->Attribute("advertise_interval"));
        g_global_config._advertise_daily_count = tools::stringAsInt(global->Attribute("advertise_daily_count"));
    }
}

struct o_config_level {
/*等级*/
    int _level;
/*总经验*/
    int _total_exp;
/*升到下一级经验*/
    int _exp;
};
extern std::map<int, o_config_level> g_config_level_map;
inline void init_config_level(const std::string & path) {
    std::string xmlpath;
    xmlpath << path << "/" << "level.xml";
    TiXmlDocument config;
    if (!config.LoadFile(xmlpath.c_str())) {
        tassert(false, "load config %s error", xmlpath.c_str());
        return;
    }

    TiXmlElement * root = config.RootElement();
    TiXmlElement * level = root->FirstChildElement("level");
    while(level) {
        o_config_level olevel;
        olevel._level = tools::stringAsInt(level->Attribute("level"));
        olevel._total_exp = tools::stringAsInt(level->Attribute("total_exp"));
        olevel._exp = tools::stringAsInt(level->Attribute("exp"));
        level = level->NextSiblingElement("level");
        g_config_level_map.insert(std::make_pair(olevel._level, olevel));
    }
}

struct o_config_map {
/*地图id*/
    int _id;
/*路线数*/
    int _path_count;
/*路线长度*/
    int _path_length;
/*资源*/
    string _res;
};
extern std::map<int, o_config_map> g_config_map_map;
inline void init_config_map(const std::string & path) {
    std::string xmlpath;
    xmlpath << path << "/" << "map.xml";
    TiXmlDocument config;
    if (!config.LoadFile(xmlpath.c_str())) {
        tassert(false, "load config %s error", xmlpath.c_str());
        return;
    }

    TiXmlElement * root = config.RootElement();
    TiXmlElement * map = root->FirstChildElement("map");
    while(map) {
        o_config_map omap;
        omap._id = tools::stringAsInt(map->Attribute("id"));
        omap._path_count = tools::stringAsInt(map->Attribute("path_count"));
        omap._path_length = tools::stringAsInt(map->Attribute("path_length"));
        omap._res = map->Attribute("res");
        map = map->NextSiblingElement("map");
        g_config_map_map.insert(std::make_pair(omap._id, omap));
    }
}

struct o_config_match {
/*序号*/
    int _id;
/*最小奖杯数*/
    int _cup_min;
/*最大奖杯数*/
    int _cup_max;
};
extern std::map<int, o_config_match> g_config_match_map;
inline void init_config_match(const std::string & path) {
    std::string xmlpath;
    xmlpath << path << "/" << "match.xml";
    TiXmlDocument config;
    if (!config.LoadFile(xmlpath.c_str())) {
        tassert(false, "load config %s error", xmlpath.c_str());
        return;
    }

    TiXmlElement * root = config.RootElement();
    TiXmlElement * match = root->FirstChildElement("match");
    while(match) {
        o_config_match omatch;
        omatch._id = tools::stringAsInt(match->Attribute("id"));
        omatch._cup_min = tools::stringAsInt(match->Attribute("cup_min"));
        omatch._cup_max = tools::stringAsInt(match->Attribute("cup_max"));
        match = match->NextSiblingElement("match");
        g_config_match_map.insert(std::make_pair(omatch._id, omatch));
    }
}

struct o_config_piece {
/*id*/
    int _id;
/*名称*/
    string _name;
/*合成角色id*/
    int _role;
/*资源*/
    int _res;
/*描述*/
    int _dsc;
};
extern std::map<int, o_config_piece> g_config_piece_map;
inline void init_config_piece(const std::string & path) {
    std::string xmlpath;
    xmlpath << path << "/" << "piece.xml";
    TiXmlDocument config;
    if (!config.LoadFile(xmlpath.c_str())) {
        tassert(false, "load config %s error", xmlpath.c_str());
        return;
    }

    TiXmlElement * root = config.RootElement();
    TiXmlElement * piece = root->FirstChildElement("piece");
    while(piece) {
        o_config_piece opiece;
        opiece._id = tools::stringAsInt(piece->Attribute("id"));
        opiece._name = piece->Attribute("name");
        opiece._role = tools::stringAsInt(piece->Attribute("role"));
        opiece._res = tools::stringAsInt(piece->Attribute("res"));
        opiece._dsc = tools::stringAsInt(piece->Attribute("dsc"));
        piece = piece->NextSiblingElement("piece");
        g_config_piece_map.insert(std::make_pair(opiece._id, opiece));
    }
}

struct o_config_random_shop {
/*序号*/
    int _id;
/*商品类别
0.碎片
1.食物*/
    int _type;
/*商品id*/
    int _content_id;
/*可购买数量*/
    int _number;
/*花费*/
    int _cost;
};
extern std::map<int, o_config_random_shop> g_config_random_shop_map;
inline void init_config_random_shop(const std::string & path) {
    std::string xmlpath;
    xmlpath << path << "/" << "random_shop.xml";
    TiXmlDocument config;
    if (!config.LoadFile(xmlpath.c_str())) {
        tassert(false, "load config %s error", xmlpath.c_str());
        return;
    }

    TiXmlElement * root = config.RootElement();
    TiXmlElement * random_shop = root->FirstChildElement("random_shop");
    while(random_shop) {
        o_config_random_shop orandom_shop;
        orandom_shop._id = tools::stringAsInt(random_shop->Attribute("id"));
        orandom_shop._type = tools::stringAsInt(random_shop->Attribute("type"));
        orandom_shop._content_id = tools::stringAsInt(random_shop->Attribute("content_id"));
        orandom_shop._number = tools::stringAsInt(random_shop->Attribute("number"));
        orandom_shop._cost = tools::stringAsInt(random_shop->Attribute("cost"));
        random_shop = random_shop->NextSiblingElement("random_shop");
        g_config_random_shop_map.insert(std::make_pair(orandom_shop._id, orandom_shop));
    }
}

struct o_config_rank {
/*称号id*/
    int _id;
/*称号名称*/
    string _name;
/*对应地图*/
    int _map;
/*所需奖杯数*/
    int _cups;
};
extern std::map<int, o_config_rank> g_config_rank_map;
inline void init_config_rank(const std::string & path) {
    std::string xmlpath;
    xmlpath << path << "/" << "rank.xml";
    TiXmlDocument config;
    if (!config.LoadFile(xmlpath.c_str())) {
        tassert(false, "load config %s error", xmlpath.c_str());
        return;
    }

    TiXmlElement * root = config.RootElement();
    TiXmlElement * rank = root->FirstChildElement("rank");
    while(rank) {
        o_config_rank orank;
        orank._id = tools::stringAsInt(rank->Attribute("id"));
        orank._name = rank->Attribute("name");
        orank._map = tools::stringAsInt(rank->Attribute("map"));
        orank._cups = tools::stringAsInt(rank->Attribute("cups"));
        rank = rank->NextSiblingElement("rank");
        g_config_rank_map.insert(std::make_pair(orank._id, orank));
    }
}

struct o_config_resource {
/*唯一id
1-.角色文件
2-.角色icon
3-.特效*/
    int _id;
/*类型
0.图片
1.spine文件
2.粒子特效*/
    int _type;
/*资源路径
(注意不要后缀名)
程序会通过type自行添加后缀名*/
    string _res;
/*动作名称(只有spine动画需要):
不填为默认default*/
    string _actionname;
/*是否循环*/
    string _loop;
/*层级*/
    int _zorder;
};
extern std::map<int, o_config_resource> g_config_resource_map;
inline void init_config_resource(const std::string & path) {
    std::string xmlpath;
    xmlpath << path << "/" << "resource.xml";
    TiXmlDocument config;
    if (!config.LoadFile(xmlpath.c_str())) {
        tassert(false, "load config %s error", xmlpath.c_str());
        return;
    }

    TiXmlElement * root = config.RootElement();
    TiXmlElement * resource = root->FirstChildElement("resource");
    while(resource) {
        o_config_resource oresource;
        oresource._id = tools::stringAsInt(resource->Attribute("id"));
        oresource._type = tools::stringAsInt(resource->Attribute("type"));
        oresource._res = resource->Attribute("res");
        oresource._actionname = resource->Attribute("actionname");
        oresource._loop = resource->Attribute("loop");
        oresource._zorder = tools::stringAsInt(resource->Attribute("zorder"));
        resource = resource->NextSiblingElement("resource");
        g_config_resource_map.insert(std::make_pair(oresource._id, oresource));
    }
}

struct o_config_role {
/*角色id*/
    int _id;
/*名称*/
    string _name;
/*资源*/
    int _res;
/*资源*/
    int _icon_res;
/*碎片id*/
    int _piece_id;
/*碎片数量*/
    int _piece_number;
/*技能
-1.无技能*/
    int _skill;
/*基础体重*/
    float _base_weight;
/*升级基础体重*/
    float _up_weight;
/*最大体力*/
    int _strength;
/*大小*/
    float _size;
/*出场音效*/
    string _appear_sound;
/*描述*/
    string _dsc;
};
extern std::map<int, o_config_role> g_config_role_map;
inline void init_config_role(const std::string & path) {
    std::string xmlpath;
    xmlpath << path << "/" << "role.xml";
    TiXmlDocument config;
    if (!config.LoadFile(xmlpath.c_str())) {
        tassert(false, "load config %s error", xmlpath.c_str());
        return;
    }

    TiXmlElement * root = config.RootElement();
    TiXmlElement * role = root->FirstChildElement("role");
    while(role) {
        o_config_role orole;
        orole._id = tools::stringAsInt(role->Attribute("id"));
        orole._name = role->Attribute("name");
        orole._res = tools::stringAsInt(role->Attribute("res"));
        orole._icon_res = tools::stringAsInt(role->Attribute("icon_res"));
        orole._piece_id = tools::stringAsInt(role->Attribute("piece_id"));
        orole._piece_number = tools::stringAsInt(role->Attribute("piece_number"));
        orole._skill = tools::stringAsInt(role->Attribute("skill"));
        orole._base_weight = tools::stringAsFloat(role->Attribute("base_weight"));
        orole._up_weight = tools::stringAsFloat(role->Attribute("up_weight"));
        orole._strength = tools::stringAsInt(role->Attribute("strength"));
        orole._size = tools::stringAsFloat(role->Attribute("size"));
        orole._appear_sound = role->Attribute("appear_sound");
        orole._dsc = role->Attribute("dsc");
        role = role->NextSiblingElement("role");
        g_config_role_map.insert(std::make_pair(orole._id, orole));
    }
}

struct o_config_rule {
/*id*/
    int _id;
/*起始时间*/
    int _begin;
/*结束时间*/
    int _end;
/*随机种子最大值*/
    int _max_rand;
};
extern std::map<int, o_config_rule> g_config_rule_map;
inline void init_config_rule(const std::string & path) {
    std::string xmlpath;
    xmlpath << path << "/" << "rule.xml";
    TiXmlDocument config;
    if (!config.LoadFile(xmlpath.c_str())) {
        tassert(false, "load config %s error", xmlpath.c_str());
        return;
    }

    TiXmlElement * root = config.RootElement();
    TiXmlElement * rule = root->FirstChildElement("rule");
    while(rule) {
        o_config_rule orule;
        orule._id = tools::stringAsInt(rule->Attribute("id"));
        orule._begin = tools::stringAsInt(rule->Attribute("begin"));
        orule._end = tools::stringAsInt(rule->Attribute("end"));
        orule._max_rand = tools::stringAsInt(rule->Attribute("max_rand"));
        rule = rule->NextSiblingElement("rule");
        g_config_rule_map.insert(std::make_pair(orule._id, orule));
    }
}

struct o_config_sceneeffect {
/*技能id*/
    int _id;
/*效果类型:
0:封锁赛道
1:封锁全场景*/
    int _type;
/**/
    vector<oResource> _view;
/*持续时间
毫秒*/
    int _time;
};
extern std::map<int, o_config_sceneeffect> g_config_sceneeffect_map;
inline void init_config_sceneeffect(const std::string & path) {
    std::string xmlpath;
    xmlpath << path << "/" << "sceneeffect.xml";
    TiXmlDocument config;
    if (!config.LoadFile(xmlpath.c_str())) {
        tassert(false, "load config %s error", xmlpath.c_str());
        return;
    }

    TiXmlElement * root = config.RootElement();
    TiXmlElement * sceneeffect = root->FirstChildElement("sceneeffect");
    while(sceneeffect) {
        o_config_sceneeffect osceneeffect;
        osceneeffect._id = tools::stringAsInt(sceneeffect->Attribute("id"));
        osceneeffect._type = tools::stringAsInt(sceneeffect->Attribute("type"));
        {
            std::string str = sceneeffect->Attribute("view");
            osplitres arrays;
            tools::split(str, ";", arrays);
            for (s32 i = 0; i < arrays.size(); i++) {
                osplitres values;
                tools::split(arrays[i], ",", values);
                oResource value;
                value._id = tools::stringAsInt(values[0].c_str());
                value._type = tools::stringAsInt(values[1].c_str());
                value._loop = tools::stringAsInt(values[2].c_str());
                value._life_cycle = tools::stringAsInt(values[3].c_str());
                osceneeffect._view.push_back(value);
            }
        }
        osceneeffect._time = tools::stringAsInt(sceneeffect->Attribute("time"));
        sceneeffect = sceneeffect->NextSiblingElement("sceneeffect");
        g_config_sceneeffect_map.insert(std::make_pair(osceneeffect._id, osceneeffect));
    }
}

struct o_config_skill {
/*技能id*/
    int _id;
/*技能类型:
0:加buff
1:消除角色
2:复制自己*/
    int _type;
/*施法类型:
0:默认施法
1:队伍优势施法
2:队伍劣势施法
3:直接接触单体优势施法
4:直接接触单体劣势施法
5:敌方队伍只有一人*/
    int _advantage_or_inferiority;
/*施法类型:
1:出场后多久施法
2:遭遇敌人施法
3:加入队伍后施法*/
    int _cast_type;
/*作用单位
1.自己
2.队伍中己方所有
3.队伍中敌方所有
4.敌方队伍第一个
5.队伍中所有人
6.自己和敌方队伍第一个*/
    int _cast_target;
/*cast_delay代表延时施法时间,单位毫秒*/
    int _cast_delay;
/*技能名称*/
    string _name;
/*技能音效*/
    string _sound;
/*技能描述*/
    string _des;
/*特效*/
    vector<oResource> _res;
/*动作*/
    vector<oSkillAction> _action;
/*buff*/
    vector<oSkillBuff> _buff;
/*场景特效*/
    vector<int> _scene_effect;
};
extern std::map<int, o_config_skill> g_config_skill_map;
inline void init_config_skill(const std::string & path) {
    std::string xmlpath;
    xmlpath << path << "/" << "skill.xml";
    TiXmlDocument config;
    if (!config.LoadFile(xmlpath.c_str())) {
        tassert(false, "load config %s error", xmlpath.c_str());
        return;
    }

    TiXmlElement * root = config.RootElement();
    TiXmlElement * skill = root->FirstChildElement("skill");
    while(skill) {
        o_config_skill oskill;
        oskill._id = tools::stringAsInt(skill->Attribute("id"));
        oskill._type = tools::stringAsInt(skill->Attribute("type"));
        oskill._advantage_or_inferiority = tools::stringAsInt(skill->Attribute("advantage_or_inferiority"));
        oskill._cast_type = tools::stringAsInt(skill->Attribute("cast_type"));
        oskill._cast_target = tools::stringAsInt(skill->Attribute("cast_target"));
        oskill._cast_delay = tools::stringAsInt(skill->Attribute("cast_delay"));
        oskill._name = skill->Attribute("name");
        oskill._sound = skill->Attribute("sound");
        oskill._des = skill->Attribute("des");
        {
            std::string str = skill->Attribute("res");
            osplitres arrays;
            tools::split(str, ";", arrays);
            for (s32 i = 0; i < arrays.size(); i++) {
                osplitres values;
                tools::split(arrays[i], ",", values);
                oResource value;
                value._id = tools::stringAsInt(values[0].c_str());
                value._type = tools::stringAsInt(values[1].c_str());
                value._loop = tools::stringAsInt(values[2].c_str());
                value._life_cycle = tools::stringAsInt(values[3].c_str());
                oskill._res.push_back(value);
            }
        }
        {
            std::string str = skill->Attribute("action");
            osplitres arrays;
            tools::split(str, ";", arrays);
            for (s32 i = 0; i < arrays.size(); i++) {
                osplitres values;
                tools::split(arrays[i], ",", values);
                oSkillAction value;
                value._action = values[0].c_str();
                value._type = tools::stringAsInt(values[1].c_str());
                value._loop = tools::stringAsInt(values[2].c_str());
                oskill._action.push_back(value);
            }
        }
        {
            std::string str = skill->Attribute("buff");
            osplitres arrays;
            tools::split(str, ";", arrays);
            for (s32 i = 0; i < arrays.size(); i++) {
                osplitres values;
                tools::split(arrays[i], ",", values);
                oSkillBuff value;
                value._buff = tools::stringAsInt(values[0].c_str());
                value._type = tools::stringAsInt(values[1].c_str());
                oskill._buff.push_back(value);
            }
        }
        {
            std::string str = skill->Attribute("scene_effect");
            osplitres arrays;
            tools::split(str, ";", arrays);
            for (s32 i = 0; i < arrays.size(); i++) {
                int value = tools::stringAsInt(arrays[i].c_str());
                oskill._scene_effect.push_back(value);
            }
        }
        skill = skill->NextSiblingElement("skill");
        g_config_skill_map.insert(std::make_pair(oskill._id, oskill));
    }
}


struct o_config_task {
/*id*/
    int _id;
/*类型
0.对战
1.获胜
2.喂养
3.分享
4.购买*/
    int _type;
/*描述*/
    string _dsc;
/*次数*/
    int _count;
/*金币奖励*/
    int _reward;
};
extern std::map<int, o_config_task> g_config_task_map;
inline void init_config_task(const std::string & path) {
    std::string xmlpath;
    xmlpath << path << "/" << "task.xml";
    TiXmlDocument config;
    if (!config.LoadFile(xmlpath.c_str())) {
        tassert(false, "load config %s error", xmlpath.c_str());
        return;
    }

    TiXmlElement * root = config.RootElement();
    TiXmlElement * task = root->FirstChildElement("task");
    while(task) {
        o_config_task otask;
        otask._id = tools::stringAsInt(task->Attribute("id"));
        otask._type = tools::stringAsInt(task->Attribute("type"));
        otask._dsc = task->Attribute("dsc");
        otask._count = tools::stringAsInt(task->Attribute("count"));
        otask._reward = tools::stringAsInt(task->Attribute("reward"));
        task = task->NextSiblingElement("task");
        g_config_task_map.insert(std::make_pair(otask._id, otask));
    }
}


#define config_init_macro     std::map<int, o_config_advertise> g_config_advertise_map; \
    std::map<int, o_config_buff> g_config_buff_map; \
    std::map<int, o_config_food> g_config_food_map; \
    o_global_config g_global_config; \
    std::map<int, o_config_level> g_config_level_map; \
    std::map<int, o_config_map> g_config_map_map; \
    std::map<int, o_config_match> g_config_match_map; \
    std::map<int, o_config_piece> g_config_piece_map; \
    std::map<int, o_config_random_shop> g_config_random_shop_map; \
    std::map<int, o_config_rank> g_config_rank_map; \
    std::map<int, o_config_resource> g_config_resource_map; \
    std::map<int, o_config_role> g_config_role_map; \
    std::map<int, o_config_rule> g_config_rule_map; \
    std::map<int, o_config_sceneeffect> g_config_sceneeffect_map; \
    std::map<int, o_config_skill> g_config_skill_map; \
    std::map<int, o_config_task> g_config_task_map; \


inline void initconfig(const std::string & path) {
    init_config_advertise(path);
    init_config_buff(path);
    init_config_food(path);
    init_global_config(path);
    init_config_level(path);
    init_config_map(path);
    init_config_match(path);
    init_config_piece(path);
    init_config_random_shop(path);
    init_config_rank(path);
    init_config_resource(path);
    init_config_role(path);
    init_config_rule(path);
    init_config_sceneeffect(path);
    init_config_skill(path);
    init_config_task(path);
}


class iConfig {
public:
    virtual ~iConfig() {}
    virtual const std::map<int, o_config_advertise> & get_advertise_config() = 0;
    virtual const std::map<int, o_config_buff> & get_buff_config() = 0;
    virtual const std::map<int, o_config_food> & get_food_config() = 0;
    virtual const o_global_config & get_global_config() = 0;
    virtual const std::map<int, o_config_level> & get_level_config() = 0;
    virtual const std::map<int, o_config_map> & get_map_config() = 0;
    virtual const std::map<int, o_config_match> & get_match_config() = 0;
    virtual const std::map<int, o_config_piece> & get_piece_config() = 0;
    virtual const std::map<int, o_config_random_shop> & get_random_shop_config() = 0;
    virtual const std::map<int, o_config_rank> & get_rank_config() = 0;
    virtual const std::map<int, o_config_resource> & get_resource_config() = 0;
    virtual const std::map<int, o_config_role> & get_role_config() = 0;
    virtual const std::map<int, o_config_rule> & get_rule_config() = 0;
    virtual const std::map<int, o_config_sceneeffect> & get_sceneeffect_config() = 0;
    virtual const std::map<int, o_config_skill> & get_skill_config() = 0;
    virtual const std::map<int, o_config_task> & get_task_config() = 0;
};


#define define_oconfig class OConfig : public iConfig { \
public: \
    virtual ~OConfig() {} \
    virtual const std::map<int, o_config_advertise> & get_advertise_config() { return g_config_advertise_map; } \
    virtual const std::map<int, o_config_buff> & get_buff_config() { return g_config_buff_map; } \
    virtual const std::map<int, o_config_food> & get_food_config() { return g_config_food_map; } \
    virtual const o_global_config & get_global_config() { return g_global_config; } \
    virtual const std::map<int, o_config_level> & get_level_config() { return g_config_level_map; } \
    virtual const std::map<int, o_config_map> & get_map_config() { return g_config_map_map; } \
    virtual const std::map<int, o_config_match> & get_match_config() { return g_config_match_map; } \
    virtual const std::map<int, o_config_piece> & get_piece_config() { return g_config_piece_map; } \
    virtual const std::map<int, o_config_random_shop> & get_random_shop_config() { return g_config_random_shop_map; } \
    virtual const std::map<int, o_config_rank> & get_rank_config() { return g_config_rank_map; } \
    virtual const std::map<int, o_config_resource> & get_resource_config() { return g_config_resource_map; } \
    virtual const std::map<int, o_config_role> & get_role_config() { return g_config_role_map; } \
    virtual const std::map<int, o_config_rule> & get_rule_config() { return g_config_rule_map; } \
    virtual const std::map<int, o_config_sceneeffect> & get_sceneeffect_config() { return g_config_sceneeffect_map; } \
    virtual const std::map<int, o_config_skill> & get_skill_config() { return g_config_skill_map; } \
    virtual const std::map<int, o_config_task> & get_task_config() { return g_config_task_map; } \
};

#endif //__auto_config_h__
