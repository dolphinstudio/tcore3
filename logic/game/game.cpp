#include "game.h"
#include "orbit.h"
#include "combiner.h"
#include "role.h"

api::iCore * g_core = nullptr;

iEvent * g_event = nullptr;
iLogicer * g_logicer = nullptr;
iScene * g_scene = nullptr;
iModel * g_model = nullptr;
iConfig * g_config = nullptr;
iCards * g_cards = nullptr;
iRedis * g_redis = nullptr;
iStorage * g_storage = nullptr;

static game * self = nullptr;

bool game::initialize(api::iCore * core) {
    tassert(eSide::top == dc::orbit::table::top::tag && eSide::bottom == dc::orbit::table::bottom::tag, "wtf");

    if (eSide::top != dc::orbit::table::top::tag || eSide::bottom != dc::orbit::table::bottom::tag) {
        return false;
    }

    g_core = core;
    self = this;
    return true;
}

bool game::launch(api::iCore * core) {
    g_event = (iEvent *)core->findModule("event");
    g_logicer = (iLogicer *)core->findModule("logicer");
    g_scene = (iScene *)core->findModule("scene");
    g_model = (iModel *)core->findModule("model");
    g_cards = (iCards *)core->findModule("cards");
    g_redis = (iRedis *)core->findModule("redis");
    g_storage = (iStorage *)core->findModule("storage");

    g_config = ((iConfiger *)core->findModule("configer"))->getConfig();

    register_event(g_event, ev::id::match_success, game::on_match_success);
    register_event(g_event, ev::id::game_start, game::on_game_start);
    register_event(g_event, ev::id::game_end, game::on_game_end);
    register_event(g_event, ev::id::load_card_data, game::on_load_card_data);
    register_event(g_event, ev::id::account_relogin, game::on_account_relogin);

    return true;
}

bool game::destroy(api::iCore * core) {
    return true;
}

void game::onTimer(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {
    switch (id) {
    case timer::id::game_ready_delay: {
        iUnit * scene = g_model->findUnit(context._context_mark);
        tassert(scene, "can not find scene object id %lld", context._context_mark);
        trace(core, "scene %lld game start, some body are not ready", scene->getid());
        g_event->trigger(ev::id::game_start, scene);
        break;
    }
    case timer::id::debug_role_position: {
        iUnit * scene = g_model->findUnit(context._context_mark);
        tassert(scene, "wtf");
        if (scene) {
            iTable * role_tab = scene->findTable(dc::scene::table::role::tag);
            tassert(role_tab, "wtf");
            for (s32 i = 0; i < role_tab->rowCount(); i++) {
                iRow * row = role_tab->getRow(i);
                iUnit * role = g_model->findUnit(row->getInt64(dc::scene::table::role::column_role_id_int64_key));
                tassert(role, "wtf");
                if (role) {
                    trace(g_core, "role %lld position y %f", role->getid(), role->getAttrFloat(dc::role::attr::y));
                }
            }
        }
        trace(g_core, "=============================================");
        break;
    }
    case timer::id::combat_info_broadcast: {
        iUnit * scene = g_model->findUnit(context._context_mark);
        tassert(scene, "wtf");
        if (scene) {
            oServerGameFrame frame;

            s32 game_time = g_config->get_global_config()._game_time - (tools::time::getMillisecond() - scene->getAttrInt64(dc::scene::attr::start_tick)) / 1000;
            frame.set_game_time(game_time);

            iTable * orbits = scene->findTable(dc::scene::table::orbit::tag);
            for (s32 i = 0; i < orbits->rowCount(); i++) {
                iRow * row = orbits->getRow(i);
                iUnit * orbit = g_model->findUnit(row->getInt64(dc::scene::table::orbit::column_id_int64));
                tassert(orbit, "wtf");
                if (orbit) {

                    for (s32 tabtag = dc::orbit::table::top::tag; tabtag <= dc::orbit::table::bottom::tag; tabtag++) {
                        iTable * roles = orbit->findTable(tabtag);
                        for (s32 j = 0; j < roles->rowCount(); j++) {
                            iRow * row_role = roles->getRow(j);
                            iUnit * role = g_model->findUnit(row_role->getInt64(dc::orbit::table::top::column_id_int64_key));
                            tassert(role, "wtf");
                            if (role) {
                                oUnit * unit = frame.add_roles();
                                getUnitProtoData(role, *unit);
                            }
                        }
                    }


                    iUnit * combiner = g_model->findUnit(orbit->getAttrInt64(dc::orbit::attr::combiner));
                    if (combiner) {
                        for (s32 tabtag = dc::combiner::table::top_role::tag; tabtag <= dc::combiner::table::bottom_role::tag; tabtag++) {
                            iTable * tab = combiner->findTable(tabtag);
                            for (s32 index = 0; index < tab->rowCount(); index++) {
                                iRow * row = tab->getRow(index);
                                iUnit * role = g_model->findUnit(row->getInt64(dc::combiner::table::top_role::column_id_int64_key));
                                tassert(role, "wtf");
                                if (role) {
                                    oUnit * unit = frame.add_roles();
                                    getUnitProtoData(role, *unit);
                                } else {
                                    error(g_core, "can not find role %lld at combiner %lld topers", row->getInt64(dc::combiner::table::top_role::column_id_int64_key), combiner->getid());
                                }
                            }
                        }
                    }
                }
            }

            iTable * members = scene->findTable(dc::scene::table::member::tag);
            for (s32 i = 0; i < members->rowCount(); i++) {
                iRow * row = members->getRow(i);
                iUnit * player = g_model->findUnit(row->getInt64(dc::scene::table::member::column_id_int64_key));
                tassert(player, "wtf");
                if (player) {
                    oCdInfo * info = frame.add_cdinfos();
                    set_oIdentity(info->mutable_account(), player->getid());
                    s64 cd = g_config->get_global_config()._public_cd - (tools::time::getMillisecond() - player->getAttrInt64(dc::player::attr::last_cd_tick));
                    if (cd < 0) {
                        cd = 0;
                    }
                    info->set_cd(cd);
                }
            }

            g_scene->broadcast(scene, eSProtoID::ServerGameFrame, frame);
        }
        break;
    }
    case timer::id::game_over: {
        iUnit * scene = g_model->findUnit(context._context_mark);
        tassert(scene, "wtf");
        if (nullptr == scene) {
            error(g_core, "cant find scene %lld", context._context_mark);
            return;
        }

        oServerGameOverPush over;
        g_scene->broadcast(scene, eSProtoID::ServerGameOverPush, over);

        g_event->trigger(ev::id::game_end, scene);
        iTable * members = scene->findTable(dc::scene::table::member::tag);
        for (s32 i = 0; i < members->rowCount(); i++) {
            iRow * row = members->getRow(i);
            iUnit * player = g_model->findUnit(row->getInt64(dc::scene::table::member::column_id_int64_key));
            tassert(player, "wtf");
            release_unit(g_model, player);
        }

        g_core->killTimer(self, timer::id::debug_role_position, scene->getid());
        g_core->killTimer(self, timer::id::combat_info_broadcast, scene->getid());
        //release_unit(g_model, scene);
        break;
    }
    }
}

void game::loadData(iUnit * scene, const ev::omatcher & info, const eSide side) {
    iUnit * player = create_unit_with_id(g_model, dc::player::name, info.account);
    player->setAttrInt64(dc::player::attr::state, eGameState::loading_data);
    player->setAttrInt64(dc::player::attr::gate, info.gate);
    player->setAttrInt64(dc::player::attr::match, info.match);
    player->setAttrInt64(dc::player::attr::scene, scene->getid());
    player->setAttrInt64(dc::player::attr::side, side);
    player->setAttrBool(dc::player::attr::isai, info.is_ai);
    if (info.is_ai) {
        g_cards->loadData(player, info.copyer);
    } else {
        g_cards->loadData(player, info.account);
    }
}

void game::on_match_success(api::iCore * core, const ev::omatch_success & ev) {
    if (ev.matchA.account == ev.matchB.account) {
        return;
    }

    iUnit * scene = g_scene->createGameScene();
    iTable * member_tab = scene->findTable(dc::scene::table::member::tag);

    iRow * row = add_row_key_int(member_tab, ev.matchA.account);
    row->setInt64(dc::scene::table::member::column_gate_int64, ev.matchA.gate);

    row = add_row_key_int(member_tab, ev.matchB.account);
    row->setInt64(dc::scene::table::member::column_gate_int64, ev.matchB.gate);

    loadData(scene, ev.matchA, eSide::bottom);
    loadData(scene, ev.matchB, eSide::top);
}

void game::on_load_card_data(api::iCore * core, const ev::oload_card_data & ev) {
    iUnit * self_player = g_model->findUnit(ev.account);
    tassert(self_player, "wtf");
    if (self_player) {
        iUnit * scene = g_model->findUnit(self_player->getAttrInt64(dc::player::attr::scene));
        tassert(scene, "wtf");
        if (scene) {
            iTable * member_tab = scene->findTable(dc::scene::table::member::tag);
            if (ev.successd) {
                iRow * row = member_tab->findRow(self_player->getid());
                row->setInt64(dc::scene::table::member::column_state_int64, eGameState::loaded_data);
                self_player->setAttrInt64(dc::player::attr::state, eGameState::loaded_data);

                bool allready = true;
                for (s32 i = 0; i < member_tab->rowCount(); i++) {
                    iRow * row = member_tab->getRow(i);
                    if (row->getInt64(dc::scene::table::member::column_state_int64) != eGameState::loaded_data) {
                        allready = false;
                    }
                }

                if (allready) {
                    oServerMatchAws aws;
                    iTable * members = scene->findTable(dc::scene::table::member::tag);
                    oUnit * scene_data = aws.mutable_scene();
                    getUnitProtoData(scene, *scene_data);

                    for (s32 i = 0; i < member_tab->rowCount(); i++) {
                        iRow * row = member_tab->getRow(i);
                        iUnit * player = g_model->findUnit(row->getInt64(dc::scene::table::member::column_id_int64_key));
                        tassert(player, "wtf");
                        if (player) {
                            oMatcherInfo * matcher_info = aws.add_matcher_info();
                            oUnit * info = matcher_info->mutable_player_info();
                            getUnitProtoData(player, *info);
                            iTable * card_used = player->findTable(dc::player::table::card_used::tag);
                            for (s32 i = 0; i < card_used->rowCount(); i++) {
                                iRow * row = card_used->getRow(i);
                                matcher_info->add_used_card_config_id(row->getInt64(dc::player::table::card_used::column_config_id_int64_key));
                            }
                            if (player->getAttrBool(dc::player::attr::isai)) {
                                row->setInt64(dc::scene::table::member::column_state_int64, eGameState::ready);
                                g_event->trigger(ev::id::ai_ready, player);
                            } else {
                                register_l_protocol(g_logicer, player->getid(), eCProtoID::ClientGameReadyPush, game::onClientGameReadyPush);
                            }
                        }
                    }

                    g_scene->broadcast(scene, eSProtoID::ServerMatchAws, aws);
                    start_timer(core, self, timer::id::game_ready_delay, timer::config::game_ready_delay, 1, timer::config::game_ready_delay, scene->getid());
                }

            } else {
                for (s32 i = 0; i < member_tab->rowCount(); i++) {
                    iRow * row = member_tab->getRow(i);
                    if (row->getInt64(dc::scene::table::member::column_state_int64) == eGameState::loaded_data) {
                        iUnit * member = g_model->findUnit(row->getInt64(dc::scene::table::member::column_id_int64_key));
                        if (member) {
                            g_logicer->tellGameOver(member->getid());
                        } else {
                            error(g_core, "where is player %lld", row->getInt64(dc::scene::table::member::column_id_int64_key));
                        }
                    }
                }

                release_unit(g_model, scene);
                release_unit(g_model, self_player);
            }
        } else {
            g_logicer->tellGameOver(self_player->getid());
            error(g_core, "where is scene %lld", self_player->getAttrInt64(dc::player::attr::scene));
        }
    } else {
        error(g_core, "where is player %lld", ev.account);
    }
}

void game::on_game_start(api::iCore * core, iUnit * scene) {
    scene->setAttrInt64(dc::scene::attr::start_tick, tools::time::getMillisecond());
    scene->setAttrInt64(dc::scene::attr::game_time, g_config->get_global_config()._game_time * SECOND);
    //start_timer(g_core, self, timer::id::debug_role_position, timer::config::debug_role_position_interval, forever, timer::config::debug_role_position_interval, scene->getid());
    start_timer(g_core, self, timer::id::game_over, g_config->get_global_config()._game_time * SECOND, 1, g_config->get_global_config()._game_time * SECOND, scene->getid());

    g_cards->initGameRole(scene);

    oServerGameStartPush push;

    iTable * members = scene->findTable(dc::scene::table::member::tag);
    for (s32 i = 0; i < members->rowCount(); i++) {
        iRow * row = members->getRow(i);
        row->setInt64(dc::scene::table::member::column_state_int64, eGameState::ready);
        s64 account = row->getInt64(dc::scene::table::member::column_id_int64_key);
        unregister_l_protocol(g_logicer, account, eCProtoID::ClientGameReadyPush);
        register_l_protocol(g_logicer, account, eCProtoID::ClientSoldiersGoToBattlePush, game::onClientSoldiersGoToBattlePush);

        iUnit * player = g_model->findUnit(account);
        tassert(player, "wtf, game start, but where is player %lld", account);
        if (player) {
            iTable * role_queue = player->findTable(dc::player::table::role_queue::tag);
            oRolesInfo * info = push.add_info();
            set_oIdentity(info->mutable_account(), player->getid());
            for (s32 i = 0; i < role_queue->rowCount(); i++) {
                iRow * row = role_queue->getRow(i);
                s64 role_id = row->getInt64(dc::player::table::role_queue::column_id_int64);
                iUnit * role = g_model->findUnit(role_id);
                tassert(role, "where is card object %lld", role_id);
                if (role) {
                    oUnit * unit = info->add_roles();
                    getUnitProtoData(role, *unit);
                }
            }
        }
    }

    g_scene->broadcast(scene, eSProtoID::ServerGameStartPush, push);

    //生成线路
    iTable * orbits = scene->findTable(dc::scene::table::orbit::tag);
    const s32 orbit_count = scene->getAttrInt64(dc::scene::attr::orbit_count);
    const s32 orbit_length = scene->getAttrFloat(dc::scene::attr::orbit_length);
    for (s32 i = 0; i < orbit_count; i++) {
        iUnit * orbit = create_unit(g_model, dc::orbit::name);
        orbit->setAttrInt64(dc::orbit::attr::scene, scene->getid());
        orbit->setAttrInt64(dc::orbit::attr::number, i);
        orbit->setAttrFloat(dc::orbit::attr::length, orbit_length);
        orbit->setAttrInt64(dc::orbit::attr::combiner, define::invalid_id);
        iRow * row = add_row_key_int(orbits, i);
        row->setInt64(dc::scene::table::orbit::column_id_int64, orbit->getid());
    }
    
    core->killTimer(self, timer::id::game_ready_delay, scene->getid());
    start_timer(g_core, self, timer::id::combat_info_broadcast, timer::config::combat_info_broadcast_interval, forever, timer::config::combat_info_broadcast_interval, scene->getid());
}

void game::on_game_end(api::iCore * core, iUnit * scene) {
    iTable * members = scene->findTable(dc::scene::table::member::tag);

    Json::Value root;
    Json::Value player_array;

    for (s32 i = 0; i < members->rowCount(); i++) {
        iRow * row = members->getRow(i);
        s64 account = row->getInt64(dc::scene::table::member::column_id_int64_key);
        iUnit * player = g_model->findUnit(account);
        tassert(player, "wtf");
        Json::Value value;
        value["is_player"] = !player->getAttrBool(dc::player::attr::isai);
        value["account"] = tools::int64AsString(account);
        value["score"] = tools::intAsString(row->getInt64(dc::scene::table::member::column_point_int64));

        player_array.append(value);
        //g_logicer->tellGameOver(account);
        unregister_l_protocol(g_logicer, account, eCProtoID::ClientSoldiersGoToBattlePush);
    }

    root["player"] = player_array;

    string strval = root.toStyledString();

    api::iHttpRequest * request = core->getHttpRequest(0, scene->getid(), g_storage->getInterfaceUrl(http::interface::statements).c_str(), self, scene->getid());
    request->postParam("package", strval.c_str());
    request->doRequest();
    trace(core, "scene %lld statements request %s", scene->getid(), strval.c_str());
}

void game::onError(const s32 id, const s32 error, const api::iContext & context) {
    error(g_core, "scene %lld statement error", context._context_mark);
    iUnit * scene = g_model->findUnit(context._context_mark);
    tassert(scene, "wtf");
    if (scene) {
        oServerGameStatementsPush body;
        body.set_code(eError::system_http_request_error);
        g_scene->broadcast(scene, eSProtoID::ServerGameStatementsPush, body);

        iTable * member_tab = scene->findTable(dc::scene::table::member::tag);
        for (s32 i = 0; i < member_tab->rowCount(); i++) {
            const s64 account = member_tab->getRow(i)->getInt64(dc::scene::table::member::column_id_int64_key);
            g_logicer->tellGameOver(account);
        }

        release_unit(g_model, scene);
    }
}

void game::onResponse(const s32 id, const void * data, const s32 size, const api::iContext & context) {
    trace(g_core, "scene %lld statement string %s", context._context_mark, (const char *)data);
    iUnit * scene = g_model->findUnit(context._context_mark);
    tassert(scene, "wtf");

    if (scene) {
        oServerGameStatementsPush body;
        body.set_code(eError::success);
        body.set_statements_json((const char *)data);
        g_scene->broadcast(scene, eSProtoID::ServerGameStatementsPush, body);

        iTable * member_tab = scene->findTable(dc::scene::table::member::tag);
        for (s32 i = 0; i < member_tab->rowCount(); i++) {
            const s64 account = member_tab->getRow(i)->getInt64(dc::scene::table::member::column_id_int64_key);
            g_logicer->tellGameOver(account);
        }

        release_unit(g_model, scene);
    }
}

void game::on_account_relogin(api::iCore * core, const s64 & account) {
    iUnit * self_player = g_model->findUnit(account);
    if (self_player && self_player->getAttrInt64(dc::player::attr::state) == eGameState::loaded_data) {
        iUnit * scene = g_model->findUnit(self_player->getAttrInt64(dc::player::attr::scene));
        if (scene) {
            oServerResumeGamePush push;
            oUnit * os = push.mutable_scene();
            getUnitProtoData(scene, *os);

            iTable * member_tab = scene->findTable(dc::scene::table::member::tag);
            for (s32 i = 0; i < member_tab->rowCount(); i++) {
                iRow * row = member_tab->getRow(i);
                iUnit * player = g_model->findUnit(row->getInt64(dc::scene::table::member::column_id_int64_key));
                if (player) {
                    if (player->getid() == account) {
                        push.set_player_state((eGameState)row->getInt64(dc::scene::table::member::column_state_int64));
                    }

                    oMatcherInfo * info = push.add_matcher_info();
                    oUnit * op = info->mutable_player_info();
                    getUnitProtoData(player, *op);
                    iTable * cards_use_tab = player->findTable(dc::player::table::card_used::tag);
                    for (s32 j = 0; j < cards_use_tab->rowCount(); j++) {
                        iRow * row = cards_use_tab->getRow(j);
                        info->add_used_card_config_id(row->getInt64(dc::player::table::card_used::column_config_id_int64_key));
                    }

                    oRolesInfo * orole = push.add_roles();
                    set_oIdentity(orole->mutable_account(), player->getid());
                    //orole->set_account(tools::int64AsString(player->getid()));
                    iTable * role_tab = scene->findTable(dc::scene::table::role::tag);
                    for (s32 i = 0; i < role_tab->rowCount(); i++) {
                        iRow * row = role_tab->getRow(i);
                        iUnit * role = g_model->findUnit(row->getInt64(dc::scene::table::role::column_role_id_int64_key));
                        if (role->getAttrInt64(dc::role::attr::player) == player->getid()) {
                            oUnit * odata = orole->add_roles();
                            getUnitProtoData(role, *odata);
                        }
                    }
                }
            }

            iTable * buff_tab = scene->findTable(dc::scene::table::buff::tag);
            for (s32 i = 0; i < buff_tab->rowCount(); i++) {
                iRow * row = buff_tab->getRow(i);
                iUnit * buff = g_model->findUnit(row->getInt64(dc::scene::table::buff::column_id_int64_key));
                tassert(buff, "wtf");
                if (buff) {
                    oServerCreateBuff * obuff = push.add_buffs();
                    set_oIdentity(obuff->mutable_buffid(), buff->getid());
                    //obuff->set_buffid(tools::int64AsString(buff->getid()).c_str());
                    obuff->set_configid(buff->getAttrInt64(dc::buff::attr::configid));
                    iTable * role_tab = buff->findTable(dc::buff::table::target::tag);
                    for (s32 j = 0; j < role_tab->rowCount(); j++) {
                        iRow * row = role_tab->getRow(j);
                        const s64 target = row->getInt64(dc::buff::table::target::column_id_int64_key);
                        oIdentity * oid = obuff->add_roles();
                        set_oIdentity(oid, target);
                        //obuff->add_roles(tools::int64AsString(row->getInt64(dc::buff::table::target::column_id_int64_key)));
                    }
                }
            }

            iTable * effect_tab = scene->findTable(dc::scene::table::effect::tag);
            for (s32 i = 0; i < effect_tab->rowCount(); i++) {
                iRow * row = effect_tab->getRow(i);
                iUnit * effect = g_model->findUnit(row->getInt64(dc::scene::table::effect::column_id_int64_key));
                tassert(effect, "wtf");
                if (effect) {
                    oServerCreateSceneEffect * oeffect = push.add_effects();
                    set_oIdentity(oeffect->mutable_effectid(), effect->getid());
                    //oeffect->set_effectid(tools::int64AsString(effect->getid()).c_str());
                    oeffect->set_configid(effect->getAttrInt64(dc::effect::attr::configid));
                    iUnit * orbit = g_model->findUnit(effect->getAttrInt64(dc::effect::attr::orbit));
                    if (orbit) {
                        oeffect->set_oribt_number(orbit->getAttrInt64(dc::orbit::attr::number));
                    }
                }
            }

            oServerRoleQueueUpdatePush * oqueue = push.mutable_role_queue();
            iTable * queue = self_player->findTable(dc::player::table::role_queue::tag);
            for (s32 i = 0; i < queue->rowCount(); i++) {
                iRow * row = queue->getRow(i);
                iUnit * role = g_model->findUnit(row->getInt64(dc::player::table::role_queue::column_id_int64));
                tassert(role, "wtf");
                if (role) {
                    oUnit * unit = oqueue->add_role();
                    getUnitProtoData(role, *unit);
                }
            }

            g_logicer->sendproto(account, eSProtoID::ServerResumeGamePush, push);
        }
    }
}

void game::onClientGameReadyPush(const s64 account, const oClientGameReadyPush & body) {
    iUnit * player = g_model->findUnit(account);
    tassert(player, "wtf");
    if (player) {
        s64 scene_id = player->getAttrInt64(dc::player::attr::scene);
        iUnit * scene = g_model->findUnit(scene_id);
        tassert(scene, "wtf");
        if (scene) {
            iTable * members = scene->findTable(dc::scene::table::member::tag);

            iRow * row = members->findRow(player->getid());
            tassert(row, "wtf");
            if (row) {
                row->setInt64(dc::scene::table::member::column_state_int64, eGameState::ready);
            }

            bool allready = true;
            for (s32 i = 0; i < members->rowCount(); i++) {
                iRow * row = members->getRow(i);
                if (row->getInt64(dc::scene::table::member::column_state_int64) != eGameState::ready) {
                    allready = false;
                }
            }

            if (allready) {
                g_event->trigger(ev::id::game_start, scene);
            }
        }
    }

    unregister_l_protocol(g_logicer, player->getid(), eCProtoID::ClientGameReadyPush);
}

void game::AiPushSoldiersGoToBattlePush(const s64 account, const s32 orbit_number) {
    iUnit * player = g_model->findUnit(account);
    tassert(player, "wtf");
    if (player && tools::time::getMillisecond() - player->getAttrInt64(dc::player::attr::last_cd_tick) > g_config->get_global_config()._public_cd) {
        iUnit * scene = g_model->findUnit(player->getAttrInt64(dc::player::attr::scene));
        tassert(scene, "wtf");
        if (scene) {
            iUnit * orbit = g_orbit->GetOrbit(scene, orbit_number);
            tassert(orbit, "wtf");
            if (orbit) {
                iUnit * role = g_role->popRoleToOrbit(player, scene, orbit);
                if (nullptr == role) {
                    return;
                }

                trace(g_core, "player %lld push role %lld, side %s, orbit number : %d",
                    player->getid(), role->getid(), (player->getAttrInt64(dc::player::attr::side) == eSide::top ? "top" : "bottom"), orbit_number);

                {
                    oServerRolesGoToBattlePush push;
                    set_oIdentity(push.mutable_account(), player->getid());
                    //push.set_account(tools::int64AsString(player->getid()));
                    push.set_orbit_number(orbit_number);
                    oUnit * unit = push.mutable_role();
                    getUnitProtoData(role, *unit);
                    g_scene->broadcast(scene, eSProtoID::ServerRolesGoToBattlePush, push);

                    role->setAttrInt64(dc::role::attr::state, eRoleState::in_orbit);

                    ev::orole_join_orbit ev;
                    ev.orbit = orbit;
                    ev.player = player;
                    ev.role = role;
                    g_event->trigger(ev::id::role_join_orbit, ev);
                }

                {
                    //上阵一个 产出一个
                    g_cards->createRole(player, tools::time::getMillisecond() - scene->getAttrInt64(dc::scene::attr::start_tick));
                    oServerRoleQueueUpdatePush push;
                    iTable * queue = player->findTable(dc::player::table::role_queue::tag);
                    for (s32 i = 0; i < queue->rowCount(); i++) {
                        iRow * row = queue->getRow(i);
                        iUnit * role = g_model->findUnit(row->getInt64(dc::player::table::role_queue::column_id_int64));
                        tassert(role, "wtf");
                        if (role) {
                            oUnit * unit = push.add_role();
                            getUnitProtoData(role, *unit);
                        }
                    }
                    g_logicer->sendproto(player->getid(), eSProtoID::ServerRoleQueueUpdatePush, push);
                }

                player->setAttrInt64(dc::player::attr::last_cd_tick, tools::time::getMillisecond());
            }
        }
    }
}

void game::onClientSoldiersGoToBattlePush(const s64 account, const oClientSoldiersGoToBattlePush & body) {
    self->AiPushSoldiersGoToBattlePush(account, body.orbit_number());
}
