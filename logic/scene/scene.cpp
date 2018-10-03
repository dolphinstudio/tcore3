#include "scene.h"

api::iCore * g_core = nullptr;
iEvent * g_event = nullptr;
iLogicer * g_logicer = nullptr;
iModel * g_model = nullptr;
iConfig * g_config = nullptr;

bool scene::initialize(api::iCore * core) {
    g_core = core;
    return true;
}

bool scene::launch(api::iCore * core) {
    g_event = (iEvent *)core->findModule("event");
    g_logicer = (iLogicer *)core->findModule("logicer");
    g_model = (iModel *)core->findModule("model");
    g_config = ((iConfiger *)core->findModule("configer"))->getConfig();

    register_event(g_event, ev::id::login_success, scene::on_login_success);
    return true;
}

bool scene::destroy(api::iCore * core) {
    return true;
}

iUnit * scene::createGameScene() {
    const std::map<int, o_config_map> & config = g_config->get_map_config();
    iUnit * scene = create_unit(g_model, dc::scene::name);

    const o_config_map & oconfig = config.begin()->second;
    scene->setAttrInt64(dc::scene::attr::config, oconfig._id);
    scene->setAttrInt64(dc::scene::attr::orbit_count, oconfig._path_count);
    scene->setAttrFloat(dc::scene::attr::orbit_length, oconfig._path_length);
    scene->setAttrString(dc::scene::attr::res, oconfig._res.c_str());
    return scene;
}

iUnit * scene::queryGameScene(iUnit * player) {
    return g_model->findUnit(player->getAttrInt64(dc::player::attr::scene));
}

void scene::broadcast(iUnit * scene, const s16 id, const void * data, const s32 size) {
    iTable * members = scene->findTable(dc::scene::table::member::tag);
    std::set<s64> accounts;
    for (s32 i = 0; i < members->rowCount(); i++) {
        iRow * row = members->getRow(i);
        accounts.insert(row->getInt64(dc::scene::table::member::column_id_int64_key));
    }

    g_logicer->send(accounts, id, data, size);
}

void scene::on_login_success(api::iCore * core, const ev::ologin_success & body) {

}
