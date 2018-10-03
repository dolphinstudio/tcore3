#include "effect.h"

api::iCore * g_core = nullptr;
iModel * g_model = nullptr;
iScene * g_scene = nullptr;
iEvent * g_event = nullptr;
iConfig * g_config = nullptr;

static effect * static_self = nullptr;

bool effect::initialize(api::iCore * core) {
    g_core = core;
    static_self = this;
    return true;
}

bool effect::launch(api::iCore * core) {
    g_model = (iModel *)core->findModule("model");
    g_event = (iEvent *)core->findModule("event");
    g_scene = (iScene *)core->findModule("scene");
    g_config = ((iConfiger *)core->findModule("configer"))->getConfig();

    register_event(g_event, ev::id::game_end, effect::on_game_end);
    return true;
}

bool effect::destroy(api::iCore * core) {
    return true;
}

void effect::createEffect(iUnit * scene, iUnit * orbit, const s32 configid) {
    auto itor = g_config->get_sceneeffect_config().find(configid);
    if (g_config->get_sceneeffect_config().end() == itor) {
        error(g_core, "where is scene effect %d", configid);
        return;
    }

    const o_config_sceneeffect * effectcfg = &(itor->second);

    iTable * efftab = scene->findTable(dc::scene::table::effect::tag);

    iUnit * effect = create_unit(g_model, dc::effect::name);
    effect->setAttrInt64(dc::effect::attr::configid, effectcfg->_id);
    effect->setAttrInt64(dc::effect::attr::type, effectcfg->_type);
    effect->setAttrInt64(dc::effect::attr::time, effectcfg->_time);
    effect->setAttrInt64(dc::effect::attr::scene, scene->getid());
    effect->setAttrInt64(dc::effect::attr::orbit, orbit->getid());
    add_row_key_int(efftab, effect->getid());

    oServerCreateSceneEffect body;
    set_oIdentity(body.mutable_effectid(), effect->getid());
    body.set_configid(effectcfg->_id);

    switch (effectcfg->_type) {
    case eSceneEffectType::block_orbit: {
        if (orbit) {
            orbit->setAttrInt64(dc::orbit::attr::close, orbit->getAttrInt64(dc::orbit::attr::close) + 1);
            start_timer(g_core, static_self, timer::id::unblock_orbit, effectcfg->_time, 1, effectcfg->_time, effect->getid());
            body.set_oribt_number(orbit->getAttrInt64(dc::orbit::attr::number));

            debug(g_core, "scene %lld orbit number %d create effect %lld config %d time %lld", scene->getid(), orbit->getAttrInt64(dc::orbit::attr::number), effect->getid(), effectcfg->_id, effectcfg->_time);
        }
        break;
    }
    default:
        break;
    }

    g_scene->broadcast(scene, eSProtoID::ServerCreateSceneEffect, body);
}

void effect::onTimer(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {
    switch (id) {
    case timer::id::unblock_orbit: {
        iUnit * effect = g_model->findUnit(context._context_mark);
        tassert(effect, "wtf");
        if (effect) {
            switch (effect->getAttrInt64(dc::effect::attr::type)) {
            case eSceneEffectType::block_orbit: {
                iUnit * orbit = g_model->findUnit(effect->getAttrInt64(dc::effect::attr::orbit));
                tassert(orbit, "wtf");
                if (orbit) {
                    orbit->setAttrInt64(dc::orbit::attr::close, orbit->getAttrInt64(dc::orbit::attr::close) - 1);
                }
            }
            }

            iUnit * scene = g_model->findUnit(effect->getAttrInt64(dc::effect::attr::scene));
            tassert(scene, "wtf");
            if (scene) {
                oServerRemoveSceneEffect body;
                set_oIdentity(body.mutable_effectid(), effect->getid());
                g_scene->broadcast(scene, eSProtoID::ServerRemoveSceneEffect, body);
            }

            debug(g_core, "recover effect %lld", effect->getid());
        }
        break;
    }
    }
}

void effect::on_game_end(api::iCore * core, iUnit * scene) {
    iTable * efftab = scene->findTable(dc::scene::table::effect::tag);
    for (s32 i = 0; i < efftab->rowCount(); i++) {
        iRow * row = efftab->getRow(i);
        iUnit * effect = g_model->findUnit(row->getInt64(dc::scene::table::effect::column_id_int64_key));
        tassert(effect, "wtf");
        if (effect) {
            core->killTimer(static_self, timer::id::unblock_orbit, effect->getid());
            release_unit(g_model, effect);
        }
    }
}
