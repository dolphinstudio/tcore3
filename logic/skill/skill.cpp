#include "skill.h"
#include "caster.h"
#include "reciver.h"

api::iCore * g_core = nullptr;
iModel * g_model = nullptr;
iEvent * g_event = nullptr;
iBuff * g_buff = nullptr;
iScene * g_scene = nullptr;
iEffect * g_effect = nullptr;
iConfig * g_config = nullptr;

static skill * static_self = nullptr;

bool skill::initialize(api::iCore * core) {
    g_core = core;
    static_self = this;
    return true;
}

bool skill::launch(api::iCore * core) {
    g_model = (iModel *)core->findModule("model");
    g_event = (iEvent *)core->findModule("event");
    g_buff = (iBuff *)core->findModule("buff");
    g_scene = (iScene *)core->findModule("scene");
    g_effect = (iEffect *)core->findModule("effect");
    g_config = ((iConfiger *)core->findModule("configer"))->getConfig();

    register_event(g_event, ev::id::role_join_orbit, skill::on_role_join_orbit);
    register_event(g_event, ev::id::join_combiner, skill::on_join_combiner);
    register_event(g_event, ev::id::role_collision, skill::on_role_collision);
    register_event(g_event, ev::id::remove_role, skill::on_remove_role);
    register_event(g_event, ev::id::cast_skill, skill::on_cast_skill);
    register_event(g_event, ev::id::recover_orbit, skill::on_recover_orbit);
    register_event(g_event, ev::id::role_thrusting, skill::on_role_thrusting);
    return true;
}

bool skill::destroy(api::iCore * core) {
    return true;
}

bool skill::disappearanceDetectionWithRole(iUnit * role1, iUnit * role2) {
    {
        const s32 skillid = role1->getAttrInt64(dc::role::attr::skill);
        auto itor = g_config->get_skill_config().find(skillid);
        if (g_config->get_skill_config().end() != itor) {
            if (eCastType::encounter_enemy ==  itor->second._cast_type) {
                
            }
        }
    }

    {
        const s32 skillid = role2->getAttrInt64(dc::role::attr::skill);
        auto itor = g_config->get_skill_config().find(skillid);
        if (g_config->get_skill_config().end() != itor) {
            if (eCastType::encounter_enemy == itor->second._cast_type) {

            }
        }
    }

    return false;
}

bool skill::disappearanceDetectionWithCombiner(iUnit * role, iUnit * combiner) {
    return false;
}

void skill::onTimer(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {
    switch (id) {
    case timer::id::delay_cast: {
        iUnit * role = g_model->findUnit(context._context_mark);
        tassert(role, "wtf");
        if (role) {
            const s32 skillid = role->getAttrInt64(dc::role::attr::skill);
            auto itor = g_config->get_skill_config().find(skillid);
            if (g_config->get_skill_config().end() != itor) {
                ev::ocast_skill ev;
                ev.config = &(itor->second);
                reciver::getInstance().getSkillRecivers(role, itor->second._cast_target, ev.targets);
                g_event->trigger(ev::id::cast_skill, ev);
            }
        }
        break;
    }
    }
}

void skill::on_cast_skill(api::iCore * core, const ev::ocast_skill & ev) {
    iUnit * role = ev.role;
    const o_config_skill * config = ev.config;
    iUnit * scene = g_model->findUnit(role->getAttrInt64(dc::role::attr::scene));

    oServerCastSkillPush body;
    set_oIdentity(body.mutable_caster(), role->getid());
    //body.set_caster(tools::int64AsString(role->getid()));
    body.set_skillid(config->_id);
    for (auto i = ev.targets.begin(); i != ev.targets.end(); i++) {
        oIdentity * oid = body.add_roles();
        set_oIdentity(oid, *i);
        //body.add_roles(tools::int64AsString(*i));
    }

    g_scene->broadcast(scene, eSProtoID::ServerCastSkillPush, body);

    switch (config->_type) {
    case eSkillType::add_buff: {

        for (s32 i = 0; i < config->_buff.size(); i++) {
            switch (config->_buff[i]._type) {
            case eBuffTarget::add_self: {
                g_buff->createBuff(scene, config->_buff[i]._buff, role->getid());
                break;
            }
            case eBuffTarget::add_enemy: {
                set<s64> enemys;
                for (auto itor = ev.targets.begin(); itor != ev.targets.end(); itor++) {
                    iUnit * target = g_model->findUnit(*itor);
                    tassert(target, "wtf");
                    if (target) {
                        if (target->getAttrInt64(dc::role::attr::side) != role->getAttrInt64(dc::role::attr::side) && target->getid() != role->getid()) {
                            enemys.insert(target->getid());
                        }
                    }
                    else {
                        error(g_core, "where is role %lld", *itor);
                    }
                }

                g_buff->createBuff(scene, config->_buff[i]._buff, enemys);
                break;
            }
            case eBuffTarget::add_teammate: {
                set<s64> teammate;
                for (auto itor = ev.targets.begin(); itor != ev.targets.end(); itor++) {
                    iUnit * target = g_model->findUnit(*itor);
                    tassert(target, "wtf");
                    if (target) {
                        if (target->getAttrInt64(dc::role::attr::side) == role->getAttrInt64(dc::role::attr::side) && target->getid() != role->getid()) {
                            teammate.insert(target->getid());
                        }
                    } else {
                        error(g_core, "where is role %lld", *itor);
                    }
                }

                g_buff->createBuff(scene, config->_buff[i]._buff, teammate);
                break;
            }
            case eBuffTarget::add_all: {
                g_buff->createBuff(scene, config->_buff[i]._buff, ev.targets);
                break;
            }
            case eBuffTarget::add_orbit: {
                g_buff->createBuff(scene, config->_buff[i]._buff, ev.targets);
                break;
            }
            }
        }

        //g_buff->createBuff(scene, )
        break;
    }
    default:
        break;
    }

    iUnit * orbit = g_model->findUnit(role->getAttrInt64(dc::role::attr::orbit));
    for (s32 i = 0; i < config->_scene_effect.size(); i++) {
        g_effect->createEffect(scene, orbit, config->_scene_effect[i]);
    }
}

void skill::on_role_join_orbit(api::iCore * core, const ev::orole_join_orbit & ev) {
    iUnit * orbit = ev.orbit;
    iUnit * player = ev.player;
    iUnit * role = ev.role;

    s32 skillid = role->getAttrInt64(dc::role::attr::skill);

    auto itor = g_config->get_skill_config().find(skillid);
    if (itor == g_config->get_skill_config().end()) {
        return;
    }

    if (itor->second._cast_type == eCastType::delay_cast) {
        if (0 == itor->second._cast_delay) {
            ev::ocast_skill ev;
            ev.role = role;
            ev.config = &(itor->second);
            reciver::getInstance().getSkillRecivers(role, (eTargetType)itor->second._cast_target, ev.targets);
            g_event->trigger(ev::id::cast_skill, ev);
        }
        else {
            start_timer(core, static_self, timer::id::delay_cast, itor->second._cast_delay, 1, itor->second._cast_delay, role->getid());
        }
    }

}

void skill::on_join_combiner(api::iCore * core, const ev::ojoin_combiner & ev) {
    iUnit * orbit = ev.orbit;
    iUnit * combiner = ev.combiner;
    iUnit * role = ev.role;

}

void skill::on_role_collision(api::iCore * core, const ev::orole_collision & ev) {

}

void skill::on_remove_role(api::iCore * core, iUnit * role) {
    core->killTimer(static_self, timer::id::delay_cast, role->getid());
}

void skill::on_recover_orbit(api::iCore * core, iUnit * orbit) {
}


void skill::on_role_thrusting(api::iCore * core, iUnit * role) {
    s32 skillid = role->getAttrInt64(dc::role::attr::skill);

    auto itor = g_config->get_skill_config().find(skillid);
    if (itor == g_config->get_skill_config().end()) {
        return;
    }

    if (itor->second._cast_type == eCastType::join_team) {
        ev::ocast_skill ev;
        ev.role = role;
        ev.config = &(itor->second);
        reciver::getInstance().getSkillRecivers(role, (eTargetType)itor->second._cast_target, ev.targets);
        g_event->trigger(ev::id::cast_skill, ev);
    }
}
