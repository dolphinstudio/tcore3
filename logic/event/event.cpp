#include "event.h"
#include "tools.h"

static api::iCore * s_core = NULL;

bool event::initialize(api::iCore * core) {
    s_core = core;
    return true;
}

bool event::launch(api::iCore * core) {
    return true;
}

bool event::destroy(api::iCore * core) {
    return true;
}

void event::register_ev(const s32 eventid, iEventer * handle) {
    eventer_map::iterator itor = _event_pool.find(eventid);
    if (_event_pool.end() == itor) {
        _event_pool.insert(make_pair(eventid, eventer_list()));
        itor = _event_pool.find(eventid);
    }

    itor->second.push_back(handle);
}

void event::trigger(const s32 eventid, const void * context, const s32 len) {
    eventer_map::iterator itor = _event_pool.find(eventid);
    if (_event_pool.end() != itor) {
        eventer_list::iterator ibegin = itor->second.begin();
        while (ibegin != itor->second.end()) {
            s64 tick = tools::time::getMillisecond();
            (*ibegin)->trigger(s_core, context, len);
            tick = tools::time::getMillisecond() - tick;
            if (tick > 1) {
                trace(s_core, "event logic %s take too long %d", (*ibegin)->_debug.c_str(), tick);
            }

            ibegin++;
        }
    }
}
