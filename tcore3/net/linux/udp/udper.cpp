#include "udper.h"

namespace tcore {
    tlib::tpool<udper> g_udper_pool;

    udper * udper::create(iUdpSession * session, const std::string & ip, const s32 port) {
        u32 fd = socket(AF_INET, SOCK_DGRAM, 0);
        
        if (socket_error == setreuse(fd)
            || socket_error == setsocksendbuff(fd, 0)
            || socket_error == setsockrecvbuff(fd, 0)
            || socket_error == setnonblocking(fd)) {
            close_socket(fd);
            session->onCreate(false);
            return nullptr;
        }       
        
        struct sockaddr_in addr;
        tools::memery::safeMemset(&addr, sizeof(addr), 0, sizeof(addr));
        addr.sin_family = AF_INET;
        inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
        addr.sin_port = htons(port);
        
        s32 ret = ::bind(fd, (struct sockaddr *)&addr, sizeof(addr));
        if (socket_error == ret) {
            close_socket(fd);
            session->onCreate(false);
            return nullptr;
        }
        
        udper * p = create_from_pool(g_udper_pool, session, ip, port);
        p->_socket = fd;
        
        struct epoll_event ev;
        ev.data.ptr = (void *) &p->_at;
        ev.events = EPOLLIN;
        if (socket_error == epoll_ctl(g_epoller_fd, EPOLL_CTL_ADD, fd, &ev)) {
            close_socket(fd);
            recover_to_pool(g_udper_pool, p);
            session->onCreate(false);
            //error(g_core, "epoll_ctl error %s", strerror(errno));
            return nullptr;
        }
        
        tools::memery::safeMemcpy((void *)&session->_pipe, sizeof(void *), &p, sizeof(void *));
        session->onCreate(true);
        return p;
    }
    
    udper::udper(iUdpSession * session, const std::string & ip, const s32 port) 
        : _session(session),
        _socket(invalid_fd),
        _is_cache(false), _at(eCompletion::doIO, this), _address(ip, port) {

    }

    void udper::cache() {

    }

    void udper::load() {

    }

    void udper::close() {
        if (invalid_fd != _socket) {
            close_socket(_socket);
            epoll_ctl(g_epoller_fd, EPOLL_CTL_DEL, _socket, nullptr);
            tools::memery::safeMemset((void *)&_session->_pipe, sizeof(void *), 0, sizeof(void *));
            _session->onClose(core::getInstance());
            recover_to_pool(g_udper_pool, this);
        }
    }

    void udper::sendto(const char * ip, const s32 port, const void * context, const s32 size) {
        struct sockaddr_in addr;
        tools::memery::safeMemset(&addr, sizeof(addr), 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_pton(AF_INET, ip, (void *)&addr.sin_addr.s_addr);
        
        s32 rel = ::sendto(_socket, context, size, 0, (sockaddr *)&addr, sizeof(addr));
        debug(core::getInstance(), "udper sendto len %d", rel);
    }

    void udper::onCompleter(associat * at, const eCompletion type, const struct epoll_event & ev) {
        tassert(type == eCompletion::doIO, "udper oncompleter type error");
        s64 tick = tools::time::getMicrosecond();
        do {
            struct sockaddr_in addr;
            tools::memery::safeMemset(&addr, sizeof(addr), 0, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_port = htons(_address._port);
            inet_pton(AF_INET, _address._ip.c_str(), (void *)&addr.sin_addr.s_addr);
            
            s32 addr_len = sizeof(addr);
            
            s32 len = ::recvfrom(_socket, _recv_temp, sizeof(_recv_temp), 0, (struct sockaddr *)&addr, (socklen_t*)&addr_len);
            if (len > 0) {
                _session->onRecv(core::getInstance(), inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), _recv_temp, len);
            } else {
                if (len <= 0 && EAGAIN == errno) {
                    return;
                } else {
                    close();
                }
            }
        } while (tools::time::getMicrosecond() - tick < 1000);
    }
}
