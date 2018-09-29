#include "idmanager.h"

bool idmanager::initialize(api::iCore * core) {
    return true;
}

bool idmanager::launch(api::iCore * core) {
    return true;
}

bool idmanager::destroy(api::iCore * core) {
    return true;
}

void idmanager::setmask(const u16 mask) {
    uuid::getInstance().setmask(mask);
}

u64 idmanager::create() {
    return uuid::getInstance().create();
}
