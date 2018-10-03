#include "cards.h"

api::iCore * g_core = nullptr;
iEvent * g_event = nullptr;
iModel * g_model = nullptr;
iRedis * g_redis = nullptr;
iConfig * g_config = nullptr;

bool cards::initialize(api::iCore * core) {
    g_core = core;
    return true;
}

bool cards::launch(api::iCore * core) {
    g_event = (iEvent *)core->findModule("event");
    g_model = (iModel *)core->findModule("model");
    g_redis = (iRedis *)core->findModule("redis");
    g_config = ((iConfiger *)core->findModule("configer"))->getConfig();

    register_event(g_event, ev::id::load_data, cards::on_load_data);
    return true;
}

bool cards::destroy(api::iCore * core) {
    return true;
}

#define calc_size(base, coefficient) (base * (1.0f + (coefficient - 1)/10.0f))

void cards::loadData(iUnit * player, const s64 copyer) {
    iRediser * rediser = g_redis->getRediser(copyer);
    tassert(rediser, "wtf");
    if (nullptr == rediser) {
        tassert(false, "wtf");
        return;
    }

    std::string key = redis::GetValue(redis::key::field_role, copyer);
    trace(g_core, "account %lld load data %s", player->getid(), key.c_str());
    rediser->asyncGet(key.c_str(), player->getid(), [](bool issuccessd, const std::string & key, const redisdata & value, const s64 account) {
        ev::oload_card_data ev;
        ev.successd = false;
        ev.account = account;

        iUnit * player = g_model->findUnit(account);
        tassert(player, "wtf");
        if (player && issuccessd) {
            s32 len;
            const void * data = value.get(len);
            if (data) {
                std::string jsondata;
                jsondata.append((const char *)data, len);

                Json::Reader reader;
                Json::Value root;
                if (reader.parse(jsondata, root)) {
                    if (root["name"].isString()
                        && root["icon"].isString()
                        && root["field_role"].isArray()) {

                        player->setAttrString(dc::player::attr::name, root["name"].asString().c_str());
                        player->setAttrString(dc::player::attr::icon, root["icon"].asString().c_str());

                        ev.successd = true;

                        iTable * used_tab = player->findTable(dc::player::table::card_used::tag);

                        for (s32 i = 0; i < root["field_role"].size(); i++) {
                            if (root["field_role"][i]["config"].isString()
                                && root["field_role"][i]["weight"].isString()
                                && root["field_role"][i]["level"].isString()
                                && root["field_role"][i]["exp"].isString()
                                && root["field_role"][i]["user_times"].isString()
                                && root["field_role"][i]["free_times"].isString()) {

                                const s32 config = tools::stringAsInt(root["field_role"][i]["config"].asString().c_str());
                                const s32 weight = tools::stringAsInt(root["field_role"][i]["weight"].asString().c_str());
                                const s32 level = tools::stringAsInt(root["field_role"][i]["level"].asString().c_str());
                                const s32 exp = tools::stringAsInt(root["field_role"][i]["exp"].asString().c_str());
                                const s32 user_times = tools::stringAsInt(root["field_role"][i]["user_times"].asString().c_str());
                                const s32 free_times = tools::stringAsInt(root["field_role"][i]["free_times"].asString().c_str());

                                //player->setAttrInt64(dc::)

                                iRow * row = add_row_key_int(used_tab, config);
                                trace(g_core, "load account %lld data %s success", account, jsondata.c_str());
                            } else {
                                error(g_core, "load account %lld data %s faild", account, jsondata.c_str());
                                ev.successd = false;
                                break;
                            }
                        }
                    }
                }
            }
        }

        g_event->trigger(ev::load_card_data, ev);
    });
}

void cards::initGameRole(iUnit * scene) {
    iTable * members = scene->findTable(dc::scene::table::member::tag);
    for (s32 i = 0; i < members->rowCount(); i++) {
        iRow * row = members->getRow(i);
        s64 account = row->getInt64(dc::scene::table::member::column_id_int64_key);
        iUnit * player = g_model->findUnit(account);
        tassert(player, "where is player %lld", account);
        if (player) {
            for (s32 j = 0; j < g_config->get_global_config()._role_queue_size; j++) {
                iUnit * role = createRole(player, 0);
            }
        }
    }
}

iUnit * cards::createRole(iUnit * player, const s64 tick) {
    iTable * card_used = player->findTable(dc::player::table::card_used::tag);
    const s32 count = card_used->rowCount();
    tassert(count > 0, "nothing used");
    if (count == 0) {
        return nullptr;
    }

    s32 index = tools::rand(count);
    iRow * row = card_used->getRow(index);
    tassert(row, "wtf");
    s32 config_id = row->getInt64(dc::player::table::card_used::column_config_id_int64_key);

    const std::map<int, o_config_role> & config = g_config->get_role_config();
    auto itor = config.find(config_id);
    if (itor == config.end()) {
        return nullptr;
    }

    iUnit * role = create_unit(g_model, dc::role::name);
    role->setAttrInt64(dc::role::attr::scene, player->getAttrInt64(dc::player::attr::scene));
    role->setAttrInt64(dc::role::attr::configid, config_id);
    role->setAttrString(dc::role::attr::name, itor->second._name.c_str());
    role->setAttrInt64(dc::role::attr::skill, itor->second._skill);
    role->setAttrFloat(dc::role::attr::base_weight, itor->second._base_weight);
    role->setAttrFloat(dc::role::attr::up_weight, itor->second._up_weight);
    role->setAttrInt64(dc::role::attr::speed, g_config->get_global_config()._move_speed);
    role->setAttrInt64(dc::role::attr::side, player->getAttrInt64(dc::player::attr::side));

    s32 max_rand = 1;
    const std::map<int, o_config_rule> & rule_config = g_config->get_rule_config();
    for (auto i = rule_config.begin(); i != rule_config.end(); i++) {
        if (tick >= i->second._begin * SECOND && tick <= i->second._end * SECOND) {
            max_rand = i->second._max_rand;
            break;
        }
    }

    s32 coefficient = 1 + tools::rand(max_rand);
    float weight = itor->second._base_weight * coefficient;
    role->setAttrInt64(dc::role::attr::state, eRoleState::in_queue);
    role->setAttrFloat(dc::role::attr::weight, weight);
    float size = calc_size(itor->second._size, coefficient);
    role->setAttrFloat(dc::role::attr::size, size);
    role->setAttrInt64(dc::role::attr::point, coefficient * role_point_step);

    iTable * role_queue = player->findTable(dc::player::table::role_queue::tag);
    row = add_row(role_queue);
    row->setInt64(dc::player::table::role_queue::column_id_int64, role->getid());

    iUnit * scene = g_model->findUnit(player->getAttrInt64(dc::player::attr::scene));
    iTable * role_tab = scene->findTable(dc::scene::table::role::tag);
    add_row_key_int(role_tab, role->getid());

    role->setAttrInt64(dc::role::attr::player, player->getid());
    trace(g_core, "player %lld create role %lld, config %d, name %s, skill %d, speed %d, weight %f, size %f, side %s", 
        player->getid(), role->getid(), config_id, itor->second._name.c_str(), itor->second._skill, g_config->get_global_config()._move_speed,
        weight, size, (player->getAttrInt64(dc::player::attr::side) == eSide::top ? "top" : "bottom"));

    return role;
}

void cards::recoverRole(iUnit * scene, iUnit * role) {
    iTable * role_tab = scene->findTable(dc::scene::table::role::tag);
    iRow * row = role_tab->findRow(role->getid());
    tassert(row, "wtf");
    if (row) {
        del_row(role_tab, row);
    }

    trace(g_core, "recover role %lld", role->getid());
    release_unit(g_model, role);
}

void cards::onTimer(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {

}

void cards::on_load_data(api::iCore * core, const ev::oload_data & body) {
//     if (body.isnewer) {
//         iTable * card_packager = body.player->findTable(dc::player::table::card_packager::tag);
// 
//         s32 count = 0;
//         for (auto itor = g_config->get_role_config().cbegin(); itor != g_config->get_role_config().cend(); itor++) {
//             s32 card_config_id = itor->second._id;
//             add_row_key_int(card_packager, card_config_id);
//             trace(core, "new player %lld got card config %d", body.player->getid(), card_config_id);
// 
//             if (count < g_config->get_global_config()._max_role_fighting) {
//                 iTable * card_used = body.player->findTable(dc::player::table::card_used::tag);
//                 add_row_key_int(card_used, card_config_id);
//                 trace(core, "new player %lld used card config %d", body.player->getid(), card_config_id);
//             }
//             count++;
//         }
//     }
}
