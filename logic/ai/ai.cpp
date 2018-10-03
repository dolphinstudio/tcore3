#include "ai.h"

iEvent * g_event = nullptr;
iModel * g_model = nullptr;
iGame * g_game = nullptr;
iConfig * g_config = nullptr;
iOrbit * g_orbit = nullptr;

static ai * static_self = nullptr;

bool ai::initialize(api::iCore * core) {
    static_self = this;
    return true;
}

bool ai::launch(api::iCore * core) {
    g_event = (iEvent *)core->findModule("event");
    g_model = (iModel *)core->findModule("model");
    g_game = (iGame *)core->findModule("game");
    g_config = ((iConfiger *)core->findModule("configer"))->getConfig();
    g_orbit = (iOrbit *)core->findModule("orbit");

    register_event(g_event, ev::id::game_start, ai::on_game_start);
    register_event(g_event, ev::id::game_end, ai::on_game_end);
    return true;
}

bool ai::destroy(api::iCore * core) {
    return true;
}

void ai::onTimer(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {
    switch (id) {
    case timer::id::ai_play: {
        iUnit * player = g_model->findUnit(context._context_mark);
        tassert(player, "wtf");
        if (player) {
            iUnit * scene = g_model->findUnit(player->getAttrInt64(dc::player::attr::scene));
            tassert(scene, "wtf");
            if (scene) {
                s32 orbit_count = scene->getAttrInt64(dc::scene::attr::orbit_count);
                g_game->AiPushSoldiersGoToBattlePush(player->getid(), tools::rand(orbit_count));
            }
        }
        break;
    }
    }
}

void ai::on_game_start(api::iCore * core, iUnit * scene) {
    iTable * member_tab = scene->findTable(dc::scene::table::member::tag);
    for (s32 i = 0; i < member_tab->rowCount(); i++) {
        iRow * row = member_tab->getRow(i);
        iUnit * player = g_model->findUnit(row->getInt64(dc::scene::table::member::column_id_int64_key));
        tassert(player, "wtf");
        if (player && player->getAttrBool(dc::player::attr::isai)) {
            start_timer(core, static_self, timer::id::ai_play, 4000, forever, timer::config::ai_play, player->getid());
        }
    }
}

void ai::on_game_end(api::iCore * core, iUnit * scene) {
    iTable * member_tab = scene->findTable(dc::scene::table::member::tag);
    for (s32 i = 0; i < member_tab->rowCount(); i++) {
        iRow * row = member_tab->getRow(i);
        iUnit * player = g_model->findUnit(row->getInt64(dc::scene::table::member::column_id_int64_key));
        tassert(player, "wtf");
        if (player && player->getAttrBool(dc::player::attr::isai)) {
            core->killTimer(static_self, timer::id::ai_play, player->getid());
        }
    }
}
