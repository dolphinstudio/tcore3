#include "buff.h"

api::iCore * g_core = nullptr;
iModel * g_model = nullptr;
iEvent * g_event = nullptr;
iScene * g_scene = nullptr;
iConfig * g_config = nullptr;

static buff * static_self = nullptr;

bool buff::initialize(api::iCore * core) {
    g_core = core;
    static_self = this;
    return true;
}

bool buff::launch(api::iCore * core) {
    g_model = (iModel *)core->findModule("model");
    g_event = (iEvent *)core->findModule("event");
    g_scene = (iScene *)core->findModule("scene");
    g_config = ((iConfiger *)core->findModule("configer"))->getConfig();

    register_event(g_event, ev::id::remove_role, buff::on_remove_role);
    register_event(g_event, ev::id::game_end, buff::on_game_end);

    return true;
}

bool buff::destroy(api::iCore * core) {
    return true;
}

void buff::onTimer(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {
    switch (id) {
    case timer::id::change_attr: {
        iUnit * buff = g_model->findUnit(context._context_mark);
        tassert(buff, "wtf");
        const float percent = buff->getAttrFloat(dc::buff::attr::value);
        if (buff) {
            iTable * targets = buff->findTable(dc::buff::table::target::tag);
            for (s32 i = 0; i < targets->rowCount(); i++) {
                iRow * row = targets->getRow(i);
                iUnit * role = g_model->findUnit(row->getInt64(dc::buff::table::target::column_id_int64_key));
                tassert(role, "wtf");
                if (role) {
                    switch (buff->getAttrInt64(dc::buff::attr::attr_type)) {
                    case eAttrType::weight: {
                        const float weight = role->getAttrFloat(dc::role::attr::weight);
                        role->setAttrFloat(dc::role::attr::weight, weight * (1.0f + percent));
                        row->setFloat(dc::buff::table::target::column_value_float, row->getFloat(dc::buff::table::target::column_value_float) + weight * percent);
                        debug(g_core, "change role %lld weight, add %2f", role->getid(), row->getFloat(dc::buff::table::target::column_value_float));
                        break;
                    }
                    }
                } else {
                    error(g_core, "can not find role %lld in buff %lld target table", row->getInt64(dc::buff::table::target::column_id_int64_key), buff->getid());
                }
            }

        }
        else {
            error(g_core, "where is buff %lld", buff->getid());
            core->killTimer(static_self, timer::id::change_attr, context);
        }
        break;
    }
    case timer::id::recover_buff: {
        iUnit * buff = g_model->findUnit(context._context_mark);
        tassert(buff, "wtf");
        if (buff) {
            recoverBuff(buff);
        } else {
            error(g_core, "where is buff %lld", buff->getid());
            core->killTimer(static_self, timer::id::recover_buff, context);
        }
        break;
    }
    }
}

void buff::createBuff(iUnit * scene, const s32 configid, const s64 target) {
    auto itor = g_config->get_buff_config().find(configid);
    tassert(itor != g_config->get_buff_config().end(), "wtf");
    if (itor == g_config->get_buff_config().end()) {
        error(g_core, "can not find buff config %d", configid);
        return;
    }

    iUnit * buff = create_unit(g_model, dc::buff::name);

    iTable * Buff_tab = scene->findTable(dc::scene::table::buff::tag);
    add_row_key_int(Buff_tab, buff->getid());

    /*
        <attribute name="role" type="s64" visual="false" share="false" save="false" important="false" />
        <attribute name="configid" type="s64" visual="false" share="false" save="false" important="false" />
        <attribute name="type" type="s64" visual="false" share="false" save="false" important="false" />
        <attribute name="attr_type" type="s64" visual="true" share="true" save="false" important="false" />
        <attribute name="delay" type="s64" visual="true" share="true" save="false" important="false" />
        <attribute name="interval" type="s64" visual="true" share="true" save="false" important="false" />
        <attribute name="count" type="s64" visual="false" share="false" save="false" important="false" />
        <attribute name="value" type="s64" visual="true" share="true" save="false" important="false" />
    */
    buff->setAttrInt64(dc::buff::attr::scene, scene->getid());
    buff->setAttrInt64(dc::buff::attr::configid, configid);
    buff->setAttrInt64(dc::buff::attr::type, itor->second._type);
    buff->setAttrInt64(dc::buff::attr::attr_type, itor->second._attr_type);
    buff->setAttrInt64(dc::buff::attr::delay, itor->second._delay);
    buff->setAttrInt64(dc::buff::attr::interval, itor->second._interval);
    buff->setAttrInt64(dc::buff::attr::count, itor->second._count);
    buff->setAttrFloat(dc::buff::attr::value, itor->second._value);

    debug(g_core, "create buff %lld, config %d, type %d, attr_type %d, delay %d, interval %d, count %d, value %d",
        buff->getid(),
        configid,
        itor->second._type,
        itor->second._attr_type,
        itor->second._delay,
        itor->second._interval,
        itor->second._count,
        itor->second._value
    );

    iTable * tab = buff->findTable(dc::buff::table::target::tag);
    iUnit * role = g_model->findUnit(target);
    tassert(role, "wtf");
    if (role) {
        add_row_key_int(tab, target);
        iTable * Buffs = role->findTable(dc::role::table::buff::tag);
        add_row_key_int(Buffs, buff->getid());

        switch (itor->second._type) {
        case eBuffType::stealth: {
            role->setAttrInt64(dc::role::attr::invisible, role->getAttrInt64(dc::role::attr::invisible) + 1);
            break;
        }
        case eBuffType::discollision: {
            role->setAttrInt64(dc::role::attr::discollision, role->getAttrInt64(dc::role::attr::discollision) + 1);
            break;
        }
        case eBuffType::congelation: {
            role->setAttrInt64(dc::role::attr::congelation, role->getAttrInt64(dc::role::attr::congelation) + 1);
            break;
        }
        }

        //debug
    }
    else {
        error(g_core, "where is role %lld", target);
    }

    if (itor->second._period != immortal) {
        start_timer(g_core, static_self, timer::id::recover_buff, itor->second._period, 1, itor->second._period, buff->getid());
    }

    switch (itor->second._type) {
    case eBuffType::changeattr: {
        start_timer(g_core, static_self, timer::id::change_attr, itor->second._delay, itor->second._count, itor->second._interval, buff->getid());
        break;
    }
    }

    oServerCreateBuff body;
    set_oIdentity(body.mutable_buffid(), buff->getid());
    body.set_configid(configid);
    oIdentity * oid = body.add_roles();
    set_oIdentity(oid, target);

    g_scene->broadcast(scene, eSProtoID::ServerCreateBuff, body);
}

void buff::createBuff(iUnit * scene, const s32 configid, const std::set<s64>& targets) {
    auto itor = g_config->get_buff_config().find(configid);
    tassert(itor != g_config->get_buff_config().end(), "wtf");
    if (itor == g_config->get_buff_config().end()) {
        error(g_core, "can not find buff config %d", configid);
        return;
    }

    iUnit * buff = create_unit(g_model, dc::buff::name);

    iTable * Buff_tab = scene->findTable(dc::scene::table::buff::tag);
    add_row_key_int(Buff_tab, buff->getid());

    /*
        <attribute name="role" type="s64" visual="false" share="false" save="false" important="false" />
        <attribute name="configid" type="s64" visual="false" share="false" save="false" important="false" />
        <attribute name="type" type="s64" visual="false" share="false" save="false" important="false" />
        <attribute name="attr_type" type="s64" visual="true" share="true" save="false" important="false" />
        <attribute name="delay" type="s64" visual="true" share="true" save="false" important="false" />
        <attribute name="interval" type="s64" visual="true" share="true" save="false" important="false" />
        <attribute name="count" type="s64" visual="false" share="false" save="false" important="false" />
        <attribute name="value" type="s64" visual="true" share="true" save="false" important="false" />
    */

    buff->setAttrInt64(dc::buff::attr::scene, scene->getid());
    buff->setAttrInt64(dc::buff::attr::configid, configid);
    buff->setAttrInt64(dc::buff::attr::type, itor->second._type);
    buff->setAttrInt64(dc::buff::attr::attr_type, itor->second._attr_type);
    buff->setAttrInt64(dc::buff::attr::delay, itor->second._delay);
    buff->setAttrInt64(dc::buff::attr::interval, itor->second._interval);
    buff->setAttrInt64(dc::buff::attr::count, itor->second._count);
    buff->setAttrFloat(dc::buff::attr::value, itor->second._value);

    debug(g_core, "create buff %lld, config %d, type %d, attr_type %d, delay %d, interval %d, count %d, value %d",
        buff->getid(),
        configid,
        itor->second._type,
        itor->second._attr_type,
        itor->second._delay,
        itor->second._interval,
        itor->second._count,
        itor->second._value
    );

    oServerCreateBuff body;
    set_oIdentity(body.mutable_buffid(), buff->getid());
    body.set_configid(configid);

    iTable * tab = buff->findTable(dc::buff::table::target::tag);
    for (auto i = targets.begin(); i != targets.end(); i++) {
        iUnit * role = g_model->findUnit(*i);
        tassert(role, "wtf");
        if (role) {
            add_row_key_int(tab, *i);
            iTable * Buffs = role->findTable(dc::role::table::buff::tag);
            add_row_key_int(Buffs, buff->getid());

            switch (itor->second._type) {
            case eBuffType::stealth: {
                role->setAttrInt64(dc::role::attr::invisible, role->getAttrInt64(dc::role::attr::invisible) + 1);
                break;
            }
            case eBuffType::discollision: {
                role->setAttrInt64(dc::role::attr::discollision, role->getAttrInt64(dc::role::attr::discollision) + 1);
                break;
            }
            case eBuffType::congelation: {
                role->setAttrInt64(dc::role::attr::congelation, role->getAttrInt64(dc::role::attr::congelation) + 1);
                break;
            }
            }
            oIdentity * oid = body.add_roles();
            set_oIdentity(oid, role->getid());
        } else {
            error(g_core, "where is role %lld", *i);
        }
    }

    if (itor->second._period != immortal) {
        start_timer(g_core, static_self, timer::id::recover_buff, itor->second._period, 1, itor->second._period, buff->getid());
    }

    switch (itor->second._type) {
    case eBuffType::changeattr: {
        start_timer(g_core, static_self, timer::id::change_attr, itor->second._delay, itor->second._count, itor->second._interval, buff->getid());
        break;
    }
    }

    g_scene->broadcast(scene, eSProtoID::ServerCreateBuff, body);
}

void buff::removeBuffFromRole(iUnit * scene, iUnit * role, iUnit * buff) {
    oServerRemoveBuff body;
    set_oIdentity(body.mutable_buffid(), buff->getid());
    body.set_configid(buff->getAttrInt64(dc::buff::attr::configid));
    oIdentity * oid = body.add_roles();
    set_oIdentity(oid, role->getid());
    g_scene->broadcast(scene, eSProtoID::ServerRemoveBuff, body);

    {
        iTable * Bufftab = role->findTable(dc::role::table::buff::tag);
        iRow * Buffrow = Bufftab->findRow(buff->getid());
        if (Buffrow) {
            del_row(Bufftab, Buffrow);
        }
    }
    {
        iTable * targettab = buff->findTable(dc::buff::table::target::tag);
        iRow * targetrow = targettab->findRow(role->getid());
        if (targetrow) {
            switch (buff->getAttrInt64(dc::buff::attr::type)) {
            case eBuffType::congelation:
                role->setAttrInt64(dc::bufftarget::attr::congelation, role->getAttrInt64(dc::bufftarget::attr::congelation) - 1);
                break;
            case eBuffType::discollision:
                role->setAttrInt64(dc::bufftarget::attr::discollision, role->getAttrInt64(dc::bufftarget::attr::discollision) - 1);
                break;
            case eBuffType::stealth:
                role->setAttrInt64(dc::bufftarget::attr::invisible, role->getAttrInt64(dc::bufftarget::attr::invisible) - 1);
                break;
            case eBuffType::changeattr: {
                switch (buff->getAttrInt64(dc::buff::attr::attr_type)) {
                case eAttrType::weight: {
                    const float value_diff = targetrow->getFloat(dc::buff::table::target::column_value_float);
                    role->setAttrFloat(dc::role::attr::weight, role->getAttrFloat(dc::role::attr::weight) - value_diff);
                    debug(g_core, "remove buff recovery role %lld weight %2f", role->getid(), value_diff);
                    break;
                }
                }
            }
            }
            del_row(targettab, targetrow);
        }
        if (targettab->rowCount() == 0) {
            static_self->recoverBuff(buff);
        }
    }
}

void buff::recoverBuff(iUnit * buff) {
    const s64 sceneid = buff->getAttrInt64(dc::buff::attr::scene);
    iUnit * scene = g_model->findUnit(sceneid);
    tassert(scene, "wtf");
    if (nullptr == scene) {
        error(g_core, "cant find scene %lld from buff %lld", sceneid, buff->getid());
    } else {
        iTable * Buff_tab = scene->findTable(dc::scene::table::buff::tag);
        iRow * row = Buff_tab->findRow(buff->getid());
        if (row) {
            del_row(Buff_tab, row);
        } else {
            error(g_core, "can not find buff %lld on scene %lld buff table", buff->getid(), sceneid);
        }
    }


    oServerRemoveBuff body;
    set_oIdentity(body.mutable_buffid(), buff->getid());
    //body.set_buffid(tools::int64AsString(buff->getid()));
    body.set_configid(buff->getAttrInt64(dc::buff::attr::configid));

    iTable * tab = buff->findTable(dc::buff::table::target::tag);
    for (s32 i = 0; i < tab->rowCount(); i++) {
        iRow * row = tab->getRow(i);
        const s64 roleid = row->getInt64(dc::buff::table::target::column_id_int64_key);
        iUnit * role = g_model->findUnit(roleid);
        tassert(role, "wtf");
        if (role) {
            oIdentity * oid = body.add_roles();
            set_oIdentity(oid, role->getid());
            //body.add_roles(tools::int64AsString(role->getid()));

            switch (buff->getAttrInt64(dc::buff::attr::type)) {
            case eBuffType::congelation:
                role->setAttrInt64(dc::bufftarget::attr::congelation, role->getAttrInt64(dc::bufftarget::attr::congelation) - 1);
                break;
            case eBuffType::discollision:
                role->setAttrInt64(dc::bufftarget::attr::discollision, role->getAttrInt64(dc::bufftarget::attr::discollision) - 1);
                break;
            case eBuffType::stealth:
                role->setAttrInt64(dc::bufftarget::attr::invisible, role->getAttrInt64(dc::bufftarget::attr::invisible) - 1);
                break;
            case eBuffType::changeattr: {
                switch (buff->getAttrInt64(dc::buff::attr::attr_type)) {
                case eAttrType::weight: {
                    const float value_diff = row->getFloat(dc::buff::table::target::column_value_float);
                    role->setAttrFloat(dc::role::attr::weight, role->getAttrFloat(dc::role::attr::weight) - value_diff);
                    debug(g_core, "remove buff recovery role %lld weight %2f", role->getid(), value_diff);
                    break;
                }
                }
                break;
            }
            }

            iTable * Buffs = role->findTable(dc::role::table::buff::tag);
            iRow * Buffrow = Buffs->findRow(buff->getid());
            if (Buffrow) {
                del_row(Buffs, Buffrow);
            } else {
                error(g_core, "can not find buff %lld on role %lld buff table", buff->getid(), roleid);
            }
        } else {
            error(g_core, "where is role %lld", roleid);
        }
    }

    g_scene->broadcast(scene, eSProtoID::ServerRemoveBuff, body);

    g_core->killTimer(static_self, timer::id::change_attr, buff->getid());
    g_core->killTimer(static_self, timer::id::recover_buff, buff->getid());

    release_unit(g_model, buff);
}

void buff::on_remove_role(api::iCore * core, iUnit * role) {
    iUnit * scene = g_model->findUnit(role->getAttrInt64(dc::role::attr::scene));
    tassert(scene, "wtf");

    iTable * Buffs = role->findTable(dc::role::table::buff::tag);
    for (s32 i = 0; i < Buffs->rowCount(); i++) {
        iRow * row = Buffs->getRow(i);
        const s64 Buffid = row->getInt64(dc::role::table::buff::column_id_int64_key);
        iUnit * buff = g_model->findUnit(Buffid);
        tassert(buff, "wtf");
        if (buff) {
            static_self->removeBuffFromRole(scene, role, buff);
            i--;
        } else {
            error(g_core, "where is buff %lld, on role %lld", Buffid, role->getid());
        }
    }
}

void buff::on_game_end(api::iCore * core, iUnit * scene) {
    iTable * Bufftab = scene->findTable(dc::scene::table::buff::tag);
    for (s32 i = 0; i < Bufftab->rowCount(); i++) {
        iRow * row = Bufftab->getRow(i);
        iUnit * buff = g_model->findUnit(row->getInt64(dc::scene::table::buff::column_id_int64_key));
        static_self->recoverBuff(buff);
        i--;
    }
}
