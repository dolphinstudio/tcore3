#include "kcper.h"

iCore * g_core = nullptr;

bool kcper::initialize(tcore::api::iCore * core) {
    g_core = core;
    return true;
}

bool kcper::launch(tcore::api::iCore * core) {

    return true;
}

bool kcper::destroy(tcore::api::iCore * core) {
    return true;
}
