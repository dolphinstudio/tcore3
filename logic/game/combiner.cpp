#include "combiner.h"

iCombiner * g_combiner = nullptr;
static combiner * static_self = nullptr;

bool combiner::initialize(api::iCore * core) {
    g_combiner = this;
    static_self = this;
    return false;
}

bool combiner::launch(api::iCore * core) {
    g_event = (iEvent *)core->findModule("event");

    register_event(g_event, ev::id::role_join_orbit, combiner::on_role_join_orbit);
    return false;
}

bool combiner::destroy(api::iCore * core) {
    return false;
}

void combiner::onTimer(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {
    switch (id) {
    case timer::id::update_combiner_position: {
        iUnit * combiner = g_model->findUnit(context._context_mark);
        tassert(combiner, "wtf");
        if (nullptr == combiner) {
            error(g_core, "can not find combiner %lld", context._context_mark);
            return;
        }

        iUnit * orbit = g_model->findUnit(combiner->getAttrInt64(dc::combiner::attr::orbit));
        iUnit * scene = g_model->findUnit(combiner->getAttrInt64(dc::combiner::attr::scene));
        tassert(orbit && scene, "wtf");
        if (nullptr == orbit || nullptr == scene) {
            error(g_core, "can not find orbit %lld or scene %lld", combiner->getAttrInt64(dc::combiner::attr::orbit), combiner->getAttrInt64(dc::combiner::attr::scene));
            return;
        }

        if (combiner->getAttrInt64(dc::bufftarget::attr::congelation) > 0) {
            return;
        }

        s32 reference = 0;
        const float combiner_top_weight = combiner->getAttrFloat(dc::combiner::attr::top_weight);
        const float combiner_bottom_weight = combiner->getAttrFloat(dc::combiner::attr::bottom_weight);
        float move_diff = 0.0f;
        if (fabs(combiner_top_weight - combiner_bottom_weight) >= 0.01f) {
            if (combiner_top_weight > combiner_bottom_weight) {
                const float speed = tmin(combiner_top_weight / (combiner_bottom_weight + 0.01f) - 1, 2) * g_config->get_global_config()._move_speed;
                tassert(speed >= 0.0f, "wtf");
                move_diff = -speed * timer::config::update_position_interval / 1000.0f;
            }
            else if (combiner_top_weight < combiner_bottom_weight) {
                const float speed = tmin(combiner_bottom_weight / (combiner_top_weight + 0.01f) - 1, 2) * g_config->get_global_config()._move_speed;
                tassert(speed >= 0.0f, "wtf");
                move_diff = speed * timer::config::update_position_interval / 1000.0f;
            }

            const float new_combiner_pos = combiner->getAttrFloat(dc::combiner::attr::y) + move_diff;
            const float orbitlen = orbit->getAttrFloat(dc::orbit::attr::length);

            combiner->setAttrFloat(dc::combiner::attr::y, new_combiner_pos);
            for (s32 tabtag = dc::combiner::table::top_role::tag; tabtag <= dc::combiner::table::bottom_role::tag; tabtag++) {
                iTable * tab = combiner->findTable(tabtag);
                for (s32 i = 0; i < tab->rowCount(); i++) {
                    iRow * row = tab->getRow(i);
                    iUnit * role = g_model->findUnit(row->getInt64(dc::combiner::table::top_role::column_id_int64_key));
                    tassert(role, "wtf");
                    if (role) {
                        const float y = role->getAttrFloat(dc::role::attr::y) + move_diff;
                        if (is_over_finish_pos(y, orbitlen)) {
                            if (tabtag == dc::combiner::table::top_role::tag) {
                                combiner->setAttrFloat(dc::combiner::attr::top_size, combiner->getAttrFloat(dc::combiner::attr::top_size) - role->getAttrFloat(dc::role::attr::size));
                                combiner->setAttrFloat(dc::combiner::attr::top_weight, combiner->getAttrFloat(dc::combiner::attr::top_weight) - role->getAttrFloat(dc::role::attr::weight));
                            } else {
                                combiner->setAttrFloat(dc::combiner::attr::bottom_size, combiner->getAttrFloat(dc::combiner::attr::bottom_size) - role->getAttrFloat(dc::role::attr::size));
                                combiner->setAttrFloat(dc::combiner::attr::bottom_weight, combiner->getAttrFloat(dc::combiner::attr::bottom_weight) - role->getAttrFloat(dc::role::attr::weight));
                            }

                            role->setAttrInt64(dc::role::attr::combiner, define::invalid_id);
                            g_role->recoverRole(scene, role, role->getAttrInt64(dc::role::attr::side), y);
                            tab->delRow(row->getIndex());
                            i--;
                        }
                        else {
                            role->setAttrFloat(dc::role::attr::y, y);
                        }
                    }
                    else {
                        error(g_core, "can not find role %lld at combiner %lld topers", row->getInt64(dc::combiner::table::top_role::column_id_int64_key), combiner->getid());
                    }
                }
                (tab->rowCount() == 0) ? reference++ : reference;
            }
        }


        if (reference == 2) {
            RecoverCombiner(scene, orbit, combiner);
        } else {
            if (reference == 1) {
                iTable * toper = combiner->findTable(dc::combiner::table::top_role::tag);
                iTable * bottomer = combiner->findTable(dc::combiner::table::bottom_role::tag);
                tassert((toper->rowCount() * bottomer->rowCount()) == 0 && (toper->rowCount() + bottomer->rowCount()) != 0, "wtf");

                if ((toper->rowCount() * bottomer->rowCount()) != 0 || (toper->rowCount() + bottomer->rowCount()) == 0) {
                    error(g_core, "combiner %lld calc postion logic error", combiner->getid());
                    return;
                }

                if (toper->rowCount() != 0) {
                    iRow * row = toper->getRow(0);
                    iUnit * role = g_model->findUnit(row->getInt64(dc::combiner::table::top_role::column_id_int64_key));
                    tassert(role, "wtf");
                    if (role) {
                        const float pos = role->getAttrFloat(dc::role::attr::y) - role->getAttrFloat(dc::role::attr::size) / 2.0f;
                        combiner->setAttrFloat(dc::combiner::attr::y, pos);
                    }
                    else {
                        error(g_core, "where is role %lld", row->getInt64(dc::combiner::table::top_role::column_id_int64_key));
                        return;
                    }

                }
                else if (bottomer->rowCount() != 0) {
                    iRow * row = bottomer->getRow(0);
                    iUnit * role = g_model->findUnit(row->getInt64(dc::combiner::table::bottom_role::column_id_int64_key));
                    tassert(role, "wtf");
                    if (role) {
                        const float pos = role->getAttrFloat(dc::role::attr::y) + role->getAttrFloat(dc::role::attr::size) / 2.0f;
                        combiner->setAttrFloat(dc::combiner::attr::y, pos);
                    }
                    else {
                        error(g_core, "where is role %lld", row->getInt64(dc::combiner::table::top_role::column_id_int64_key));
                        return;
                    }
                }
            }

            CheckTopers(scene, orbit, combiner);
            CheckBottomers(scene, orbit, combiner);
        }

        break;
    }
    }
}

iUnit * combiner::MergeToCombiner(iUnit * orbit, iUnit * toper, iUnit * bottomer) {
    iUnit * combiner = create_unit(g_model, dc::combiner::name);
    combiner->setAttrInt64(dc::combiner::attr::scene, orbit->getAttrInt64(dc::orbit::attr::scene));
    combiner->setAttrInt64(dc::combiner::attr::orbit, orbit->getid());

    iTable * top = combiner->findTable(dc::combiner::table::top_role::tag);
    iTable * bottom = combiner->findTable(dc::combiner::table::bottom_role::tag);

    add_row_key_int(top, toper->getid());
    add_row_key_int(bottom, bottomer->getid());
    toper->setAttrInt64(dc::role::attr::combiner, combiner->getid());
    bottomer->setAttrInt64(dc::role::attr::combiner, combiner->getid());

    float combiner_y = (toper->getAttrFloat(dc::role::attr::y) + bottomer->getAttrFloat(dc::role::attr::y)) / 2.0f;
    combiner->setAttrFloat(dc::combiner::attr::y, combiner_y);
    combiner->setAttrFloat(dc::combiner::attr::top_size, toper->getAttrFloat(dc::role::attr::size));
    combiner->setAttrFloat(dc::combiner::attr::top_weight, toper->getAttrFloat(dc::role::attr::weight));

    combiner->setAttrFloat(dc::combiner::attr::bottom_size, bottomer->getAttrFloat(dc::role::attr::size));
    combiner->setAttrFloat(dc::combiner::attr::bottom_weight, bottomer->getAttrFloat(dc::role::attr::weight));

    toper->setAttrFloat(dc::role::attr::y, combiner_y + toper->getAttrFloat(dc::role::attr::size) / 2.0f);
    bottomer->setAttrFloat(dc::role::attr::y, combiner_y - bottomer->getAttrFloat(dc::role::attr::size) / 2.0f);

    trace(g_core, "create combiner toper y %f, bottomer y %f, combiner y %f", toper->getAttrFloat(dc::role::attr::y), bottomer->getAttrFloat(dc::role::attr::y), combiner_y);

    iTable * tab = orbit->findTable(dc::orbit::table::top::tag);
    iRow * row = tab->findRow(toper->getid());
    tassert(row, "wtf");
    if (row) {
        del_row(tab, row);
    } else {
        error(g_core, "can not find role %lld at dc::orbit::table::top", toper->getid());
    }

    tab = orbit->findTable(dc::orbit::table::bottom::tag);
    row = tab->findRow(bottomer->getid());
    tassert(row, "wtf");
    if (row) {
        del_row(tab, row);
    } else {
        error(g_core, "can not find role %lld at dc::orbit::table::top", bottomer->getid());
    }

    s64 orbit_combiner = orbit->getAttrInt64(dc::orbit::attr::combiner);
    tassert(orbit_combiner == define::invalid_id, "wtf");
    if (orbit_combiner != define::invalid_id) {
        error(g_core, "orbit already has combiner %lld", orbit_combiner);
    } else {
        orbit->setAttrInt64(dc::orbit::attr::combiner, combiner->getid());
        start_timer(g_core, static_self, timer::id::update_combiner_position, timer::config::update_position_interval, forever, timer::config::update_position_interval, combiner->getid());
    }

    return combiner;
}

void combiner::RecoverCombiner(iUnit * scene, iUnit * orbit, iUnit * combiner) {
    g_core->killTimer(this, timer::id::update_combiner_position, combiner->getid());
    trace(g_core, "recover combiner %lld", combiner->getid());
    orbit->setAttrInt64(dc::orbit::attr::combiner, define::invalid_id);
    release_unit(g_model, combiner);
}

void combiner::CheckTopers(iUnit * scene, iUnit * orbit, iUnit * combiner) {
    iTable * orbittop = orbit->findTable(dc::orbit::table::top::tag);
    iTable * combinertop = combiner->findTable(dc::combiner::table::top_role::tag);
    const float combiner_y = combiner->getAttrFloat(dc::combiner::attr::y);
    const float combiner_top_size = combiner->getAttrFloat(dc::combiner::attr::top_size);
    const float combiner_top_weight = combiner->getAttrFloat(dc::combiner::attr::top_weight);

    ev::ojoin_combiner ev;
    for (s32 i = 0; i < orbittop->rowCount(); i++) {
        iRow * row = orbittop->getRow(i);
        iUnit * toper = g_model->findUnit(row->getInt64(dc::orbit::table::top::column_id_int64_key));
        tassert(toper, "wtf");
        if (toper) {
            if (toper->getAttrInt64(dc::role::attr::discollision) == 0) {
                const float role_y = toper->getAttrFloat(dc::role::attr::y);
                const float role_size = toper->getAttrFloat(dc::role::attr::size);

                if (fabs(combiner_y - role_y) <= combiner_top_size + role_size / 2.0f) {
                    //g_core->killTimer(self, timer::id::update_role_position, toper->getid());
                    ev.combiner = combiner;
                    ev.orbit = orbit;
                    ev.role = toper;
                    g_event->trigger(ev::id::join_combiner, ev);

                    const float combiner_y = combiner->getAttrFloat(dc::combiner::attr::y);
                    const float combiner_top_size = combiner->getAttrFloat(dc::combiner::attr::top_size);
                    const float combiner_top_weight = combiner->getAttrFloat(dc::combiner::attr::top_weight);

                    const float toper_size = toper->getAttrFloat(dc::role::attr::size);
                    const float toper_weight = toper->getAttrFloat(dc::role::attr::weight);

                    toper->setAttrFloat(dc::role::attr::y, combiner_y + combiner_top_size + toper_size / 2.0f);


                    combiner->setAttrFloat(dc::combiner::attr::top_size, combiner_top_size + toper_size);
                    combiner->setAttrFloat(dc::combiner::attr::top_weight, combiner_top_weight + toper_weight);

                    add_row_key_int(combiner->findTable(dc::combiner::table::top_role::tag), toper->getid());
                    toper->setAttrInt64(dc::role::attr::combiner, combiner->getid());

                    oServerRoleThrustingPush push;
                    oIdentity * oid = push.add_ids();
                    set_oIdentity(oid, toper->getid());
                    //push.add_ids(tools::int64AsString(toper->getid()));
                    toper->setAttrInt64(dc::role::attr::state, eRoleState::thrusting);
                    g_scene->broadcast(scene, eSProtoID::ServerRoleThrustingPush, push);
                    g_event->trigger(ev::id::role_thrusting, toper);

                    iTable * tab = orbit->findTable(dc::orbit::table::top::tag);
                    iRow * row = tab->findRow(toper->getid());
                    tassert(row, "wtf");
                    if (row) {
                        del_row(tab, row);
                    }
                    else {
                        error(g_core, "can not find role %lld at dc::orbit::table::top", toper->getid());
                    }
                    return;
                }
                break;
            }
        }
        else {
            error(g_core, "can not find role %lld at orbit %lld toper table", row->getInt64(dc::orbit::table::top::column_id_int64_key), orbit->getid());
        }
    }
}

void combiner::CheckBottomers(iUnit * scene, iUnit * orbit, iUnit * combiner) {
    iTable * orbitbottom = orbit->findTable(dc::orbit::table::bottom::tag);
    iTable * combinerbottom = combiner->findTable(dc::combiner::table::bottom_role::tag);
    const float combiner_y = combiner->getAttrFloat(dc::combiner::attr::y);
    const float combiner_bottom_size = combiner->getAttrFloat(dc::combiner::attr::bottom_size);
    const float combiner_bottom_weight = combiner->getAttrFloat(dc::combiner::attr::bottom_weight);

    ev::ojoin_combiner ev;
    for (s32 i = 0; i < orbitbottom->rowCount(); i++) {
        iRow * row = orbitbottom->getRow(i);
        iUnit * bottomer = g_model->findUnit(row->getInt64(dc::orbit::table::bottom::column_id_int64_key));
        tassert(bottomer, "wtf");
        if (bottomer) {
            if (bottomer->getAttrInt64(dc::role::attr::discollision) == 0) {
                const float role_y = bottomer->getAttrFloat(dc::role::attr::y);
                const float role_size = bottomer->getAttrFloat(dc::role::attr::size);

                if (fabs(combiner_y - role_y) <= combiner_bottom_size + role_size / 2.0f) {
                    ev.combiner = combiner;
                    ev.orbit = orbit;
                    ev.role = bottomer;
                    g_event->trigger(ev::id::join_combiner, ev);

                    const float combiner_y = combiner->getAttrFloat(dc::combiner::attr::y);
                    const float combiner_bottom_size = combiner->getAttrFloat(dc::combiner::attr::bottom_size);
                    const float combiner_bottom_weight = combiner->getAttrFloat(dc::combiner::attr::bottom_weight);

                    const float bottomer_size = bottomer->getAttrFloat(dc::role::attr::size);
                    const float bottomer_weight = bottomer->getAttrFloat(dc::role::attr::weight);

                    bottomer->setAttrFloat(dc::role::attr::y, combiner_y - combiner_bottom_size - bottomer_size / 2.0f);

                    combiner->setAttrFloat(dc::combiner::attr::bottom_size, combiner_bottom_size + bottomer_size);
                    combiner->setAttrFloat(dc::combiner::attr::bottom_weight, combiner_bottom_weight + bottomer_weight);

                    add_row_key_int(combiner->findTable(dc::combiner::table::bottom_role::tag), bottomer->getid());
                    bottomer->setAttrInt64(dc::role::attr::combiner, combiner->getid());

                    oServerRoleThrustingPush push;
                    oIdentity * oid = push.add_ids();
                    set_oIdentity(oid, bottomer->getid());
                    //push.add_ids(tools::int64AsString(bottomer->getid()));
                    bottomer->setAttrInt64(dc::role::attr::state, eRoleState::thrusting);
                    g_scene->broadcast(scene, eSProtoID::ServerRoleThrustingPush, push);
                    g_event->trigger(ev::id::role_thrusting, bottomer);

                    iTable * tab = orbit->findTable(dc::orbit::table::bottom::tag);
                    iRow * row = tab->findRow(bottomer->getid());
                    tassert(row, "wtf");
                    if (row) {
                        del_row(tab, row);
                    }
                    else {
                        error(g_core, "can not find role %lld at dc::orbit::table::top", bottomer->getid());
                    }
                    return;
                }
                break;
            }
        }
        else {
            error(g_core, "can not find role %lld at orbit %lld toper table", row->getInt64(dc::orbit::table::top::column_id_int64_key), orbit->getid());
        }
    }
}

void combiner::on_role_join_orbit(api::iCore * core, const ev::orole_join_orbit & ev) {
    reg_attr_call(ev.role, dc::role::attr::weight, combiner::on_role_weight_changed);
}


void combiner::on_role_weight_changed(api::iCore *, iUnit * role, const dc::layout &, const float & oldvalue, const float & newvalue, const bool) {
    iUnit * combiner = g_model->findUnit(role->getAttrInt64(dc::role::attr::combiner));
    if (combiner) {
        const float diff = newvalue - oldvalue;
        if (float_is_zero(diff)) {
            return;
        }

        switch (role->getAttrInt64(dc::role::attr::side)) {
        case eSide::top: {
            const float top_weight = combiner->getAttrFloat(dc::combiner::attr::top_weight);
            combiner->setAttrFloat(dc::combiner::attr::top_weight, top_weight + diff);
            debug(g_core, "role %lld changed weight %2f to %2f, reflush combiner top weight %2f", role->getid(), oldvalue, newvalue, top_weight + diff);
            break;
        }
        case eSide::bottom: {
            const float bottom_weight = combiner->getAttrFloat(dc::combiner::attr::bottom_weight);
            combiner->setAttrFloat(dc::combiner::attr::bottom_weight, bottom_weight + diff);
            debug(g_core, "role %lld changed weight %2f to %2f, reflush combiner bottom weight %2f", role->getid(), oldvalue, newvalue, bottom_weight + diff);
            break;
        }
        }

    }
}
