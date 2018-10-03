#include "orbit.h"

iOrbit * g_orbit = nullptr;

bool orbit::initialize(api::iCore * core) {
    g_orbit = this;
    return true;
}

bool orbit::launch(api::iCore * core) {
    g_event = (iEvent *)core->findModule("event");
    g_scene = (iScene *)core->findModule("scene");
    g_model = (iModel *)core->findModule("model");
    g_cards = (iCards *)core->findModule("cards");
    g_config = ((iConfiger *)core->findModule("configer"))->getConfig();

    register_event(g_event, ev::id::game_end, orbit::on_game_end);
    return true;
}

bool orbit::destroy(api::iCore * core) {
    return true;
}

iUnit * orbit::GetOrbit(iUnit * scene, const s32 orbit_number) {
    if (scene) {
        iTable * orbits = scene->findTable(dc::scene::table::orbit::tag);
        tassert(orbits, "wtf");
        iRow * row = orbits->getRow(orbit_number);
        tassert(row, "wtf");
        if (row) {
            iUnit * orbit = g_model->findUnit(row->getInt64(dc::scene::table::orbit::column_id_int64));
            tassert(orbit, "where is orbit");
            return orbit;
        }
    }

    return nullptr;
}

void orbit::onTimer(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {
    switch (id) {
        
    }
}

void orbit::on_game_end(api::iCore * core, iUnit * scene) {
    iTable * orbits = scene->findTable(dc::scene::table::orbit::tag);
    for (s32 i = 0; i < orbits->rowCount(); i++) {
        iRow * row = orbits->getRow(i);
        iUnit * orbit = g_model->findUnit(row->getInt64(dc::scene::table::orbit::column_id_int64));
        tassert(orbit, "wtf");
        if (orbit) {
            iUnit * combiner = g_model->findUnit(orbit->getAttrInt64(dc::orbit::attr::combiner));
            if (combiner) {
                g_combiner->RecoverCombiner(scene, orbit, combiner);
            }

            g_event->trigger(ev::id::recover_orbit, orbit);
            release_unit(g_model, orbit);
        }
    }
    orbits->clear();
}
