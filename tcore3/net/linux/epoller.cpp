#include "epoller.h"
#include "udp/udper.h"
#include "tcp/tcper.h"
#include "tcp/accepter.h"

namespace tcore {
    s32 g_epoller_fd = invalid_fd;
    tlib::tpool<associat> g_associat_pool;
    core * g_core = nullptr;
    
    epoller * epoller::getInstance() {
        static epoller * static_ptr = nullptr;
        if (nullptr == static_ptr) {
            static_ptr = NEW epoller;
            if (!static_ptr->launch()) {
                DEL static_ptr;
                static_ptr = nullptr;
            }
        }
        
        return static_ptr;
    }
    
    bool epoller::launch() {
        g_core = core::getInstance();
        
        tassert(g_core, "wtf");

        if (invalid_fd == (g_epoller_fd = epoll_create(epoller_desc_count))) {
            error(g_core, "create epoller fd error %s", strerror(errno));
            return false;
        }
        return true;
    }

    bool epoller::launchUdpSession(iUdpSession * session, const char * ip, const s32 port) {
        udper * udp = udper::create(session, ip, port);
        return udp != nullptr;
    }

    bool epoller::launchTcpSession(iTcpSession * session, const char * host, const int port, int max_ss, int max_rs) {
        tcper * tcp = tcper::create(session, host, port, max_ss, max_rs);
        return tcp != nullptr;
    }

    bool epoller::launchTcpServer(iTcpServer * server, const char * ip, const int port, int max_ss, int max_rs) {
        accepter * accepter = accepter::create(server, ip, port, max_ss, max_rs);
        return accepter != nullptr;
    }

    void epoller::deal() {
        static struct epoll_event evs[epoller_ev_max_count];
        s64 tick = tools::time::getMicrosecond();
        do {
            errno = 0;
            s32 count = 0;
            if (0 == (count = epoll_wait(g_epoller_fd, evs, epoller_ev_max_count, 1))) {
                return;
            }

            if (count == -1) {
                return;
            }

            for (s32 i = 0; i < count; i++) {
                struct associat * p = (struct associat *) evs[i].data.ptr;
                p->_completer->onCompleter(p, p->_ev, evs[i]);
            }
        } while (tools::time::getMicrosecond() - tick < 1000);
    }
    
}
