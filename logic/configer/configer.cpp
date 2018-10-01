#include "configer.h"

config_init_macro

define_oconfig

bool configer::initialize(api::iCore * core) {
    std::string path;
    path << tools::file::getApppath() << "/" << "env/xmlconfig";
    initconfig(path);
    return true;
}

bool configer::launch(api::iCore * core) {
    return true;
}

bool configer::destroy(api::iCore * core) {
    return true;
}

iConfig * configer::getConfig() {
    static OConfig o;
    return &o;
}
