#include "udptester.h"
#include "udpsession.h"

iCore * g_core = nullptr;

bool udptester::initialize(tcore::api::iCore * core) {
    g_core = core;
    return true;
}

bool udptester::launch(tcore::api::iCore * core) {

//    udpSession * session1 = NEW udpSession("172.17.1.23", 32000);
//    core->launchUdpSession(session1, any_eth, 32001);
//
//    udpSession * session2 = NEW udpSession("172.17.1.23", 32001);
//    core->launchUdpSession(session2, any_eth, 32000);

    return true;
}

bool udptester::destroy(tcore::api::iCore * core) {
    return true;
}
