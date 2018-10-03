#include "storage.h"

api::iCore * g_core = nullptr;

static storage * static_self = nullptr;
static std::string static_host = "";

bool storage::initialize(api::iCore * core) {
    g_core = core;
    static_self = this;
    return true;
}

bool storage::launch(api::iCore * core) {
    std::string path = tools::file::getApppath();
    path += "/server_config/dataconfig.xml";

    TiXmlDocument configdoc;
    if (false == configdoc.LoadFile(path.c_str())) {
        tassert(false, "where is noder.xml");
        return false;
    }

    TiXmlElement * root = configdoc.RootElement();
    TiXmlElement * config = root->FirstChildElement("config");

    tassert(config, "wtf");
    static_host = config->Attribute("host");
    return true;
}

bool storage::destroy(api::iCore * core) {
    return true;
}

const std::string storage::getInterfaceUrl(const std::string & interface) {
    return static_host + interface;
}