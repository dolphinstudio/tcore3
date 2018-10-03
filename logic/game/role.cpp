#include "role.h"
iRole * g_role = nullptr;
static role * static_self = nullptr;

#define reverse_side(side) abs((side) - 1)

bool role::initialize(api::iCore * core) {
    g_role = this;
    static_self = this;
    return true;
}

bool role::launch(api::iCore * core) {
    g_event = (iEvent *)core->findModule("event");
    g_scene = (iScene *)core->findModule("scene");
    g_model = (iModel *)core->findModule("model");
    g_cards = (iCards *)core->findModule("cards");
    g_config = ((iConfiger *)core->findModule("configer"))->getConfig();

    register_event(g_event, ev::id::game_end, role::on_game_end);
    register_event(g_event, ev::id::join_combiner, role::on_join_combiner);
    return true;
}

bool role::destroy(api::iCore * core) {
    return true;
}

iUnit * role::popRoleToOrbit(iUnit * player, iUnit * scene, iUnit * orbit) {
    if (orbit->getAttrInt64(dc::orbit::attr::close) > 0) {
        return nullptr;
    }

    iUnit * role = popRole(player);
    tassert(role, "wtf");
    if (nullptr == role) {
        error(g_core, "player %lld pop role error", player->getid());
        return nullptr;
    }

    iTable * tab = nullptr;
    switch (role->getAttrInt64(dc::role::attr::side)) {
    case eSide::top: {
        tab = orbit->findTable(dc::orbit::table::top::tag);
        role->setAttrFloat(dc::biology::attr::y, scene->getAttrFloat(dc::scene::attr::orbit_length));
        break;
    }
    case eSide::bottom: {
        tab = orbit->findTable(dc::orbit::table::bottom::tag);
        role->setAttrFloat(dc::biology::attr::y, 0);
        break;
    }
    default:
        tassert(false, "wtf");
        return nullptr;
    }

    role->setAttrInt64(dc::role::attr::orbit, orbit->getid());
    role->setAttrInt64(dc::role::attr::orbit_num, orbit->getAttrInt64(dc::orbit::attr::number));
    tassert(tab, "wtf");
    add_row_key_int(tab, role->getid());

    start_timer(g_core, static_self, timer::id::update_role_position, timer::config::update_position_interval, forever, timer::config::update_position_interval, role->getid());
    return role;
}

void role::recoverRole(iUnit * scene, iUnit * role, const s32 side, const float y) {
    s32 length = scene->getAttrFloat(dc::scene::attr::orbit_length);

    ev::ocalc_score ev;
    ev.y = y;
    ev.side = side;
    ev.role = role;
    ev.scene = scene;
    g_event->trigger(ev::id::calc_score, ev);
    g_event->trigger(ev::id::remove_role, role);

    //冲出对方防线 加分
    oServerRolesLeaveBattlePush push;
    oIdentity * oid = push.add_roles();
    set_oIdentity(oid, role->getid());
    //push.add_roles(tools::int64AsString(role->getid()));
    g_scene->broadcast(scene, eSProtoID::ServerRolesLeaveBattlePush, push);
    g_cards->recoverRole(scene, role);
}


void role::onTimer(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {
    switch (id) {
    case timer::id::update_role_position: {
        iUnit * role = g_model->findUnit(context._context_mark);
        tassert(role, "where is role %lld do update_role_position", context._context_mark);
        if (role) {
            iUnit * scene = g_model->findUnit(role->getAttrInt64(dc::role::attr::scene));
            iUnit * orbit = g_model->findUnit(role->getAttrInt64(dc::role::attr::orbit));
            tassert(scene && orbit, "wtf");
            if (nullptr == scene || nullptr == orbit) {
                return;
            }

            if (role->getAttrInt64(dc::role::attr::congelation) > 0) {
                return;
            }

            float y = role->getAttrFloat(dc::biology::attr::y);
            const eSide side = (eSide)role->getAttrInt64(dc::role::attr::side);
            (eSide::top == side) 
                ? (y = y - g_config->get_global_config()._move_speed * timer::config::update_position_interval / 1000.0f) 
                : (y = y + g_config->get_global_config()._move_speed * timer::config::update_position_interval / 1000.0f);

            const float orbitlen = scene->getAttrFloat(dc::scene::attr::orbit_length);
            if (is_over_finish_pos(y, orbitlen)) {
                //冲出对方防线 加分
                core->killTimer(static_self, timer::id::update_role_position, context);
                debug(core, "kill timer timer::id::update_role_postion %lld", context._context_mark);
                iTable * roles = orbit->findTable(side);
                iRow * row = roles->findRow(role->getid());
                del_row(roles, row);

                recoverRole(scene, role, side, y);
            } else {
                role->setAttrFloat(dc::biology::attr::y, y);
                if (role->getAttrInt64(dc::role::attr::discollision) > 0) {
                    return;
                }

                const float size = role->getAttrFloat(dc::role::attr::size);

                iTable * tab = orbit->findTable(reverse_side(side));
                for (s32 i = 0; i < tab->rowCount(); i++) {
                    iRow * row = tab->getRow(i);
                    iUnit * enemy = g_model->findUnit(row->getInt64(dc::orbit::table::top::column_id_int64_key));
                    tassert(enemy, "wtf");
                    if (enemy) {
                        if (enemy->getAttrInt64(dc::role::attr::discollision) > 0) {
                            continue;
                        }

                        float enemy_size = enemy->getAttrFloat(dc::role::attr::size);
                        float enemy_y = enemy->getAttrFloat(dc::role::attr::y);

                        if (fabs(enemy_y - y) <= ((enemy_size + size) / 2.0f)) {
                            (eSide::top == side) ? g_combiner->MergeToCombiner(orbit, role, enemy) : g_combiner->MergeToCombiner(orbit, enemy, role);
                            oServerRoleThrustingPush push;
                            oIdentity * oid = push.add_ids();
                            set_oIdentity(oid, role->getid());
                            //push.add_ids(tools::int64AsString(role->getid()));
                            oid = push.add_ids();
                            set_oIdentity(oid, enemy->getid());
                            //push.add_ids(tools::int64AsString(enemy->getid()));
                            role->setAttrInt64(dc::role::attr::state, eRoleState::thrusting);
                            enemy->setAttrInt64(dc::role::attr::state, eRoleState::thrusting);
                            g_scene->broadcast(scene, eSProtoID::ServerRoleThrustingPush, push);
                            g_event->trigger(ev::id::role_thrusting, role);
                            g_event->trigger(ev::id::role_thrusting, enemy);

                            core->killTimer(static_self, timer::id::update_role_position, role->getid());
                            core->killTimer(static_self, timer::id::update_role_position, enemy->getid());

                            return;
                        }
                    }
                    else {
                        error(g_core, "can not find role %lld", row->getInt64(dc::orbit::table::top::column_id_int64_key));
                    }
                }
            }
        }
        break;
    }
    }
}

void role::on_game_end(api::iCore * core, iUnit * scene) {
    iTable * roles = scene->findTable(dc::scene::table::role::tag);
    for (s32 i = 0; i < roles->rowCount(); i++) {
        iRow * row = roles->getRow(i);
        iUnit * role = g_model->findUnit(row->getInt64(dc::scene::table::role::column_role_id_int64_key));
        tassert(role, "wtf");
        if (role) {
            g_core->killTimer(static_self, timer::id::update_role_position, role->getid());
            release_unit(g_model, role);
        }
    }

    roles->clear();
}

void role::on_join_combiner(api::iCore * core, const ev::ojoin_combiner & ev) {
    core->killTimer(static_self, timer::id::update_role_position, ev.role->getid());
}

iUnit * role::popRole(iUnit * player) {
    iTable * role_queue = player->findTable(dc::player::table::role_queue::tag);
    tassert(role_queue, "wtf");

    if (role_queue->rowCount() > 0) {
        iRow * row = role_queue->getRow(0);
        iUnit * role = g_model->findUnit(row->getInt64(dc::player::table::role_queue::column_id_int64));
        tassert(role, "wtf");
        role_queue->delRow(row->getIndex());
        return role;
    }

    return nullptr;
}
