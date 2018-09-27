#include "udpsession.h"

void udpSession::onCreate(bool success) {
    if (success) {
        start_timer(g_core, this, 1, tools::rand(5000), 200, 100, 0);
    }
}

void udpSession::onRecv(iCore * core, const char * ip, const s32 port, const char * context, const int size) {
    trace(core, "recv from %s:%d : %s", ip, port, context);
}

void udpSession::onClose(iCore * core) {
    core->killTimer(this, 1);
    trace(core, "on close");
    DEL this;
}

void udpSession::onStart(iCore * core, const s32 id, const iContext & context, const s64 tick) {

}

void udpSession::onTimer(iCore * core, const s32 id, const iContext & context, const s64 tick) {
    std::string content = "hello ";
    content.append(tools::intAsString(_remote._port));
    sendto(_remote._ip.c_str(), _remote._port, content.c_str(), content.size()+1);
}

void udpSession::onEnd(iCore * core, const s32 id, const iContext & context, bool nonviolent, const s64 tick) {
    close();
}

void udpSession::onPause(iCore * core, const s32 id, const iContext & context, const s64 tick) {

}

void udpSession::onResume(iCore * core, const s32 id, const iContext & context, const s64 tick) {

}
