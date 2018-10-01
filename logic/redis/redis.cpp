#include "redis.h"
#include "rediser.h"
#include "tinyxml/tinyxml.h"


tpool<redisdata> g_value_pool;
tqueue<oAsyncRedisIO> g_async_io_queue_in;
tqueue<oAsyncRedisIO> g_async_io_queue_out;


rediser * g_master_redis = nullptr;
std::map<section, vector<rediser *>> g_rediser_map;

api::iCore * g_core = nullptr;

static OUT std::string GetHostByName(const char * host) {
    hostent * h = gethostbyname(host);
    if (nullptr == h) {
        return "";
    }

    return inet_ntoa(*((struct in_addr *)h->h_addr));
}

bool redis::initialize(api::iCore * core) {
    g_core = core;

    std::string path;
    path << tools::file::getApppath() << "/server_config/redis.xml";

    TiXmlDocument config;
    if (!config.LoadFile(path.c_str())) {
        tassert(false, "where is %s", path.c_str());
        return false;
    }

    TiXmlElement * root = config.RootElement();
    TiXmlElement * master = root->FirstChildElement("master");
    const char * master_host = master->Attribute("host");
    const s32 master_port = tools::stringAsInt(master->Attribute("port"));
    const char * master_passwd = master->Attribute("passwd");
    g_master_redis = NEW rediser(master_host, master_port, master_passwd);
    if (false == g_master_redis->initialize()) {
        error(core, "initize redis error %s:%d passwd:%s", master_host, master_port, master_passwd);
    }

    TiXmlElement * group = root->FirstChildElement("group");
    while (group) {
        const char * section_str = group->Attribute("section");
        osplitres res;
        tools::split(section_str, "-", res);
        const s64 section_min = tools::stringAsInt64(res[0].c_str());
        const s64 section_max = tools::stringAsInt64(res[1].c_str());

        std::vector<rediser *> rediser_vec;
        TiXmlElement * redis = group->FirstChildElement("redis");
        while (redis) {
            const char * host = redis->Attribute("host");
            const s32 port = tools::stringAsInt(redis->Attribute("port"));
            const char * passwd = redis->Attribute("passwd");

            rediser * r = NEW rediser(host, port, passwd);
            if (false == r->initialize()) {
                error(core, "initize redis error %s:%d passwd:%s", host, port, passwd);
            }
            tassert(r, "wtf");
            rediser_vec.push_back(r);
            redis = redis->NextSiblingElement("redis");
        }

        g_rediser_map.insert(make_pair(section(section_min, section_max), rediser_vec));
        group = group->NextSiblingElement("group");
    }

    return true;
}

bool redis::launch(api::iCore * core) {
    return true;
}

bool redis::destroy(api::iCore * core) {
    return true;
}

iRediser * redis::getMasterRediser() {
    return g_master_redis;
}

iRediser * redis::getRediser(const s64 account) {
    auto itor = g_rediser_map.find(account);
    if (itor->second.size() > 0) {
        return itor->second[account % itor->second.size()];
    }
    return nullptr;
}
