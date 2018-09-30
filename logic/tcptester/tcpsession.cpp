#include "tcpsession.h"
tlib::tpool<tcpSession> g_tcpsession_pool;
static s32 static_session_index = 0;

tcpSession::tcpSession() : _index(static_session_index++), _total_size(0), _bron_tick(tools::time::getMillisecond()) {

}

void tcpSession::release() {
    recover_to_pool(g_tcpsession_pool, this);
}

int tcpSession::onRecv(iCore * core, const void * context, const int size) {
    if (size <= sizeof(oHeader)) {
        return 0;
    }
    
    const oHeader * oh = (const oHeader *)context;
    if (size < oh->_size) {
        return 0;
    }
    
    _total_size += oh->_size;
    
    if (core->getArgs("server")) {
        send(context, oh->_size);
        g_send_total_size += oh->_size;
    } else {
        s32 delay = tools::time::getMillisecond() - oh->_tick;
        if (delay > 50) {
            printf("recv delay %d \n", delay);
        }
    }

    g_recv_total_size += oh->_size;
    return oh->_size;
}

void tcpSession::onConnected(iCore * core) {
    ++g_connection_count;
//    if (this->_initiative) {
//        trace(core, "session %d connected, static_alive_count %d initiative true", _index, static_alive_count);
//    } else {
//        trace(core, "session %d connected, static_alive_count %d initiative fals", _index, static_alive_count);
//    }
//    trace(core, "session %d connected, static_alive_count %d", _index, static_alive_count);
    if (core->getArgs("server") == nullptr) {
        start_timer(core, this, 1, tools::rand(10000), tools::rand(500) + 100, tools::rand(200) + 300, 0);
    }
}

void tcpSession::onDisconnect(iCore * core) {
    --g_connection_count;
//    trace(core, "session %d disconnected, g_connection_count %d, recv total size %fKB, live cycle %d", _index, g_connection_count, _total_size/1024.0f, tools::time::getMillisecond() - _bron_tick);

    core->killTimer(this, 1);
    recover_to_pool(g_tcpsession_pool, this);
}

void tcpSession::onConnectFailed(iCore * core) {
    trace(core, "session %d connect failed", _index);
    recover_to_pool(g_tcpsession_pool, this);
}

void tcpSession::onStart(iCore * core, const s32 id, const iContext & context, const s64 tick) {
}

void tcpSession::onTimer(iCore * core, const s32 id, const iContext & context, const s64 tick) {
    s32 len = tools::rand(1024) + 1;
    char * temp = (char *)alloca(len);
    oHeader oh;
    oh._tick = tools::time::getMillisecond();
    oh._size = sizeof(oh) + len;

    send(&oh, sizeof(oh), false);
    send(temp, len);
    g_send_total_size += oh._size;
}

void tcpSession::onEnd(iCore * core, const s32 id, const iContext & context, bool nonviolent, const s64 tick) {
    close();
}

void tcpSession::onPause(iCore * core, const s32 id, const iContext & context, const s64 tick) {
}

void tcpSession::onResume(iCore * core, const s32 id, const iContext & context, const s64 tick) {
}
