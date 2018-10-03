#include "relation.h"

api::iCore * g_core = nullptr;
iNoder * g_noder = nullptr;
static relation * self = nullptr;

account_gate_map g_account_gate_map;

bool relation::initialize(api::iCore * core) {
    g_core = core;
    self = this;
    return true;
}

bool relation::launch(api::iCore * core) {
    g_noder = (iNoder *)core->findModule("noder");

    register_node_ev(g_noder, eNoderEvent::noder_report, relation::onNoderReport);
    register_node_ev(g_noder, eNoderEvent::noder_dropout, relation::onNoderDropout);
    return true;
}

bool relation::destroy(api::iCore * core) {
    return true;
}

void relation::onNoderReport(iNoderSession * session) {
    if (session->getname() == "gate") {
        register_proto(session, eNProtoID::AccountReport, relation::onAccountReport);
        register_proto(session, eNProtoID::AccountLogout, relation::onAccountLogout);
    }
}

void relation::onNoderDropout(iNoderSession * session) {

}

void relation::onAccountReport(api::iCore * core, iSession * session, s64 context, const oAccountReport & body) {
    iNoderSession * gate = (iNoderSession *)session;

    trace(core, "account %lld login from gate %d", body.account(), gate->getid());

    auto itor = g_account_gate_map.find(body.account());
    if (itor != g_account_gate_map.end()) {
        oAccountRelogin relogin;
        relogin.set_account(body.account());
        relogin.set_gateid(gate->getid());
        g_noder->sendProtoToNoder("match", eNProtoID::AccountRelogin, relogin);

        if (itor->second == gate->getid()) {
            return;
        }
        trace(core, "kick account %lld on gate %d", body.account(), itor->second);

        g_noder->sendProtoToNoder(itor->second, eNProtoID::AccountRelogin, relogin);
        g_account_gate_map.erase(itor);
    }

    g_account_gate_map.insert(std::make_pair(body.account(), body.gateid()));
}

void relation::onAccountLogout(api::iCore * core, iSession * session, s64 context, const oAccountLogout & body) {
    auto itor = g_account_gate_map.find(body.account());
    if (itor != g_account_gate_map.end()) {
        g_account_gate_map.erase(itor);
    }

    trace(g_core, "account %lld logout", body.account());
}
