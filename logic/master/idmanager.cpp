#include "idmanager.h"

idmanager & idmanager::GetInstance() {
    static idmanager static_manager;
    return static_manager;
}

s32 idmanager::AllocaID(const std::string & name, const std::string & ip, const s32 port) {
    std::string key = name + ip + tools::intAsString(port);

    auto itor = _id_map.find(key);
    if (itor != _id_map.end()) {
        return itor->second;
    }
    
    s32 id = _id_index++;
    _id_map.insert(make_pair(key, id));
    return id;
}
