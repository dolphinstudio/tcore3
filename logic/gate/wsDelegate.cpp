#include "wsDelegate.h"
#include "gate.h"
delegate_map g_delegate_map;

void wsDelegate::onConnect() {
    g_event->trigger(ev::id::connected, this);
}

void wsDelegate::onDisconnect() {
    g_core->killTimer(this, timer::id::delay_close);
    g_gate->accountLogoutReport(this);
    g_event->trigger(ev::id::disconnect, this);
    this->release();
}

void wsDelegate::onRecv(const char * data, const s32 size) {
    if (size < sizeof(s8)) {
        return;
    }

    const s16 id = *(const s16 *)data;
    if (id == eCProtoID::ClientLoginReq) {
        if (this->getid() != define::invalid_id) {
            close();
            return;
        }

        oClientLoginReq req;
        if (!req.ParseFromArray(data + sizeof(s16), size - sizeof(s16))) {
            error(g_core, "ParseFromArray error");
            close();
            return;
        }

        s32 account = req.account();
        if (define::invalid_id == account) {
            close();
            return;
        }

        iRediser * rediser = g_redis->getRediser(account);
        tassert(rediser, "wtf");
        if (nullptr == rediser) {
            close();
            return;
        }
        this->setid(req.account());
        this->setToken(req.token());
        this->retain();

        debug(g_core, "delegate login account %lld token %s", req.account(), req.token().c_str());
        std::string key = redis::GetValue(redis::key::auth, account);
        rediser->asyncGet(key.c_str(), (s64)this, [](bool issuccessd, const std::string & key, const redisdata & value, const s64 context) {
            wsDelegate * delegate = (wsDelegate *)context;
            if (issuccessd) {
                s32 len;
                const void * data = value.get(len);
                if (nullptr != data && len > 0) {
                    std::string jsondata;
                    jsondata.append((const char *)data, len);

                    Json::Reader reader;
                    Json::Value root;
                    if (reader.parse(jsondata, root)) {
                        if (root["account"].isString() 
                            && root["token"].isString()
                            && root["username"].isString()
                            && root["icon"].isString()
                            && root["is_newer"].isBool()) {

                            s64 account = tools::stringAsInt64(root["account"].asString().c_str());
                            string token = root["token"].asString();

                            if (account == delegate->getid() && token == delegate->getToken()) {
                                g_gate->accountLoginReport(delegate);
                                ev::ologin_success body;
                                body.deg = delegate;
                                body.name = root["username"].asString();
                                body.icon_url = root["icon"].asString();
                                body.isnewer = root["is_newer"].asBool();
                                g_event->trigger(ev::id::login_success, body);

                                oServerLoginAws aws;
                                set_oIdentity(aws.mutable_account(), delegate->getid());
                                aws.set_code(eError::success);
                                delegate->sendproto(eSProtoID::ServerLoginAws, aws);
                                delegate->setIsLogin(true);
                                return;
                            }
                        }
                    }
                }
            }

            delegate->setid(define::invalid_id);
            oServerLoginAws aws;
            aws.set_code(eError::unknown);
            delegate->sendproto(eSProtoID::ServerLoginAws, aws);

            trace(g_core, "get redis data %s error", key.c_str());
            delegate->delayClose();
            delegate->release();
        });
    } else if (this->isLogin() && !g_gate->pushMessage(this, id, data + sizeof(id), size - sizeof(id)) && this->getLogicNoderID() != define::invalid_id){
        oRelayClientMessageToNoder body;
        body.set_account(getid());
        body.set_messageid(id);
        body.set_body(data + sizeof(id), size - sizeof(id));
        g_noder->sendProtoToNoder(getLogicNoderID(), eNProtoID::RelayClientMessageToNoder, body);
    } else {
        //close();
    }
}

void wsDelegate::setid(const s64 id) {
    _id = id;
}

void wsDelegate::recover() {
    DEL this;
}


void wsDelegate::delayClose() {
    start_timer(g_core, this, timer::id::delay_close, timer::config::delay_close, 1, timer::config::delay_close, 0);
}

void wsDelegate::onTimer(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {
    switch (id) {
    case timer::id::delay_close: {
        close();
        break;
    }
    }
}
