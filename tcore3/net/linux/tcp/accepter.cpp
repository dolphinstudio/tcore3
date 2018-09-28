#include "accepter.h"
#include "tcper.h"

namespace tcore {
    tlib::tpool<accepter> g_accepter_pool;
    
    accepter * accepter::create(iTcpServer * server, const char * ip, const s32 port, s32 s_size, s32 r_size) {
        tassert(server != NULL, "wtf where is server");
        struct timeval tv;
        struct sockaddr_in addr;
        tools::memery::safeMemset(&addr, sizeof (addr), 0, sizeof (addr));
        addr.sin_family = AF_INET;
        inet_pton(AF_INET, ip, &addr.sin_addr);
        addr.sin_port = htons(port);

        s32 sock = invalid_fd;
        if (invalid_fd == (sock = socket(AF_INET, SOCK_STREAM, 0))
                || socket_error == setreuse(sock)
                || socket_error == setnonblocking(sock)
                || socket_error == bind(sock, (sockaddr *) & addr, sizeof (addr))
                || socket_error == listen(sock, 200)) {
            //error(g_core, "socket error %s", strerror(errno));
            close_socket(sock);
            return nullptr;
        }

        accepter * ac = create_from_pool(g_accepter_pool, server, ip, port, s_size, r_size);
        ac->_socket = sock;
        epoll_event ev;
        ev.data.ptr = (void *) &ac->_at;
        ev.events = EPOLLIN;
        if (socket_error == epoll_ctl(g_epoller_fd, EPOLL_CTL_ADD, sock, &ev)) {
            ac->release();
            //error(g_core, "epoll_ctl error %s", strerror(errno));
            return nullptr;
        }

        return ac;
    }

    void accepter::onCompleter(associat * at, const eCompletion type, const struct epoll_event & ev) {
        tassert(type == eCompletion::doAccept, "accepter on completer error");
        if (ev.events & EPOLLIN) {
            struct sockaddr_in addr;
            socklen_t len = sizeof (addr);
            s64 sock = invalid_fd;
            s32 time = 0;
            while ((sock = accept(_socket, (sockaddr *) & addr, &len)) >= 0 && time++ <= 256) {
                if (socket_error != setnonblocking(sock)
                        || socket_error != settcpnodelay(sock)
                        || socket_error != setreuse(sock)
                        || socket_error != setsocksendbuff(sock, 0)
                        || socket_error != setsockrecvbuff(sock, 0)) {

                    struct sockaddr_in sa;
                    int len = sizeof (sa);
                    getpeername(sock, (struct sockaddr *) &sa, (socklen_t *) & len);
                    const std::string ip = inet_ntoa(sa.sin_addr);
                    const s32 port = ntohs(sa.sin_port);
                    //debug(g_core, "new client %s:%d connected from prot %d", ip.c_str(), port, _address._port);

                    iTcpSession * session = _server->onMallocConnection(g_core, ip.c_str(), port);
                    if (!session) {
                        _server->onError(g_core, session);
                        close_socket(sock);
                        //error(g_core, "server %x malloc a null session point", _server);
                        continue;
                    }

                    session->_address._ip = ip;
                    session->_address._port = port;
                    
                    tcper * tcp = create_from_pool(g_tcper_pool, session, ip, port, _ssize, _rsize);
                    struct epoll_event ev;
                    ev.data.ptr = (void *) &tcp->_associat;
                    ev.events = EPOLLIN;
                    if (socket_error == epoll_ctl(g_epoller_fd, EPOLL_CTL_ADD, sock, &ev)) {
                        _server->onError(g_core, session);
                        //error(g_core, "epoll_ctl error %s", strerror(errno));
                        close_socket(sock);
                        recover_to_pool(g_tcper_pool, tcp);
                        continue;
                    }

                    tcp->_connected = true;
                    tcp->_socket = sock;
                    
                    iTcpPipe * pipe = tcp;
                    tools::memery::safeMemcpy((void *)&session->_pipe, sizeof(void *), &pipe, sizeof(void *));
                    session->onConnected(g_core);
                    continue;
                }
                
                //error(g_core, "socket opt error %s", strerror(errno));
                close_socket(sock);
            }
        } else {
            //error(g_core, "accept error %s", strerror(errno));
        }
    }

    void accepter::release() {
        epoll_ctl(g_epoller_fd, EPOLL_CTL_DEL, _socket, nullptr);
        close_socket(_socket);
        _server->onRelease(g_core);
        recover_to_pool(g_accepter_pool, this);
    }

    accepter::accepter(iTcpServer * server, const char * ip, const s32 port, s32 s_size, s32 r_size) 
    : _server(server), _socket(invalid_fd), _address(ip, port), _ssize(s_size <= max_pipe_size ? s_size : max_pipe_size), _rsize(r_size <= max_pipe_size ? r_size : max_pipe_size), _at(eCompletion::doAccept, this) {
        
    }
}
