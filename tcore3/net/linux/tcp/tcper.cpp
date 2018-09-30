#include "tcper.h"

namespace tcore {
    tlib::tpool<tcper> g_tcper_pool;
    
    tcper * tcper::create(api::iTcpSession * session, const string & host, const s32 port, s32 ssize, s32 rsize) {
        (ssize <= max_pipe_size) ? : (ssize = max_pipe_size);
        (rsize <= max_pipe_size) ? : (rsize = max_pipe_size);

        const s32 hostlen = host.size() + 1;
        char * temp = (char *)alloca(hostlen);
        tools::memery::safeMemcpy(temp, hostlen, host.c_str(), hostlen);

        static const bool b = true;
        tools::memery::safeMemcpy((void *)&session->_initiative, sizeof(bool), (void*)&b, sizeof(bool));

        std::string ip;
        if (!getIpByHost(host.c_str(), ip)) {
            session->onConnectFailed(g_core);
            return nullptr;
        }
        
        session->_address._ip = ip;
        session->_address._port = port;
        
        struct timeval tv;
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);

        s32 sock = invalid_fd;
        if (invalid_fd == (sock = socket(AF_INET, SOCK_STREAM, 0))
            || socket_error == setnonblocking(sock)
            || inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0) {
            //error(g_core, "socket error %s, sock %lld, %s:%d", strerror(errno), sock, ip.c_str(), port);
            close_socket(sock);
            session->onConnectFailed(g_core);
            return nullptr;
        }

        s32 ret = connect(sock, (struct sockaddr *) &addr, sizeof(addr));
        tcper * p = create_from_pool(g_tcper_pool, session, ip, port, ssize, rsize);
        p->_socket = sock;
        if (success == ret) {
            struct epoll_event ev;
            ev.data.ptr = (void *)&p->_associat;
            ev.events = EPOLLIN;
            if (socket_error == epoll_ctl(g_epoller_fd, EPOLL_CTL_ADD, sock, &ev)) {
                //error(g_core, "epoll_ctl error %s", strerror(errno));
                session->onConnectFailed(g_core);
                close_socket(sock);
                recover_to_pool(g_tcper_pool, p);
                return nullptr;
            }

            p->_connected = true;
            tools::memery::safeMemcpy((void *)&session->_pipe, sizeof(void *), &p, sizeof(p));
            session->onConnected(g_core);  
        } else if (ret < 0 && errno != EINPROGRESS) {
            close_socket(sock);
            session->onConnectFailed(g_core);
            recover_to_pool(g_tcper_pool, p);
            return nullptr;
        } else {
            associat * at = create_from_pool(g_associat_pool, eCompletion::doConnect, p);
            struct epoll_event ev;
            ev.data.ptr = (void *)at;
            ev.events = EPOLLOUT;
            if (socket_error == epoll_ctl(g_epoller_fd, EPOLL_CTL_ADD, sock, &ev)) {
                //error(g_core, "epoll_ctl error %s", strerror(errno));
                session->onConnectFailed(g_core);
                close_socket(sock);
                recover_to_pool(g_tcper_pool, p);
                return nullptr;
            }
        }

        return p;
    }
    
    tcper::tcper(api::iTcpSession * session, const string & ip, const s32 port, const s32 ssize, const s32 rsize) 
    : _session(session), _remote(ip, port), _connected(false), _caching(false), _send_buff(ssize), _recv_buff(rsize), _associat(eCompletion::doIO, this), _socket(invalid_fd)
    {

    }
    
    void tcper::onCompleter(associat * at, const eCompletion type, const struct epoll_event & ev) {
        switch(type) {
            case eCompletion::doConnect: {
                //associat * at = (associat *)ev.data.ptr;
                recover_to_pool(g_associat_pool, at);
                
                if (socket_error == settcpnodelay(_socket)) {
                    close_socket(_socket);
                    _session->onConnectFailed(g_core);
                    recover_to_pool(g_tcper_pool, this);
                    return;
                }

                epoll_ctl(g_epoller_fd, EPOLL_CTL_DEL, _socket, nullptr);
                if (ev.events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
                    close_socket(_socket);
                    _session->onConnectFailed(g_core);
                    recover_to_pool(g_tcper_pool, this);
                    return;
                }

                if (ev.events & EPOLLOUT) {
                    struct epoll_event event;
                    event.data.ptr = (void *) &_associat;
                    event.events = EPOLLIN;
                    if (socket_error == epoll_ctl(g_epoller_fd, EPOLL_CTL_ADD, _socket, &event)) {
                        //error(g_core, "epoll ctl add error %s", strerror(errno));
                        close_socket(_socket);
                        _session->onConnectFailed(g_core);
                        recover_to_pool(g_tcper_pool, this);
                        return;
                    }
                    
                    iPipe * p = this;
                    tools::memery::safeMemcpy((void *)&_session->_pipe, sizeof(void *), &p, sizeof(void *));
                    _connected = true;
                    _session->onConnected(g_core);
                } else {                        
                    close_socket(_socket);
                    _session->onConnectFailed(g_core);
                    recover_to_pool(g_tcper_pool, this);
                }
                break;
            }
            case eCompletion::doIO: {
                if (ev.events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {                        
                    //error(g_core, "eCompletion::doIO error %s", strerror(errno));
                    close_socket(_socket);
                    _connected = false;
                    tools::memery::safeMemset((void *)&_session->_pipe, sizeof(void *), 0, sizeof(void *));
                    _session->onDisconnect(g_core);
                    recover_to_pool(g_tcper_pool, this);
                } else {
                    if (ev.events & EPOLLIN) {
                        static char temp[recv_temp_size];
                        tassert(_connected, "wtf");
                        if (_connected) {
                            const s64 tick = tools::time::getMicrosecond();
                            do {
                                s32 len = recv(_socket, temp, sizeof(temp), 0);
                                if (len < 0 && EAGAIN == errno) {
                                    return;
                                } else if (len > 0) {
                                    //debug(g_core, "pipe recv data size %d", len);
                                    if (_recv_buff.in(temp, len)) {
                                        while (_recv_buff.getLength() > 0 && !_caching) {
                                            s32 use = _session->onRecv(g_core, _recv_buff.getData(), _recv_buff.getLength());
                                            if (!_connected) {
                                                epoll_ctl(g_epoller_fd, EPOLL_CTL_DEL, _socket, nullptr);
                                                close_socket(_socket);
                                                if (_session) {
                                                    tools::memery::safeMemset((void *)&_session->_pipe, sizeof(void *), 0, sizeof(void *));
                                                    _session->onDisconnect(g_core);
                                                    recover_to_pool(g_tcper_pool, this);
                                                }
                                                return;
                                            }

                                            if (use <= 0) {
                                                return;
                                            }
                                            _recv_buff.out(use);
                                        }
                                    } else {
                                        close();
                                        return;
                                    }
                                } else {
                                    close();
                                    return;
                                } 
                            } while (_connected && tools::time::getMicrosecond() - tick <= 1000);
                        }
                    }

                    if (ev.events & EPOLLOUT) {
                        tassert(_connected, "wtf");
                        s32 len = ::send(_socket, _send_buff.getData(), _send_buff.getLength(), 0);
                        if (len > 0) {
                            _send_buff.out(len);
                            if (_send_buff.getLength() == 0) {
                                struct epoll_event ev;
                                ev.data.ptr = (void *) &_associat;
                                ev.events = EPOLLIN;
                                if (socket_error == epoll_ctl(g_epoller_fd, EPOLL_CTL_MOD, _socket, &ev)) {                                               
                                    epoll_ctl(g_epoller_fd, EPOLL_CTL_DEL, _socket, nullptr);
                                    close_socket(_socket);
                                    tools::memery::safeMemset((void *)&_session->_pipe, sizeof(void *), 0, sizeof(void *));
                                    _session->onDisconnect(g_core);
                                    tools::memery::safeMemset((void *)&_session, sizeof(void *), 0, sizeof(void *));
                                    recover_to_pool(g_tcper_pool, this);
                                    return;
                                }
                            }
                        } else if (len <= 0 && EAGAIN != errno) {
                            epoll_ctl(g_epoller_fd, EPOLL_CTL_DEL, _socket, nullptr);
                            close_socket(_socket);
                            tools::memery::safeMemset((void *)&_session->_pipe, sizeof(void *), 0, sizeof(void *));
                            _session->onDisconnect(g_core);
                            recover_to_pool(g_tcper_pool, this);
                        }
                    }
                }
                
                break;
            }
        }
    }
    
    void tcper::close() {
        if (_socket != invalid_fd) {
            _connected = false;
            epoll_ctl(g_epoller_fd, EPOLL_CTL_DEL, _socket, nullptr);
            close_socket(_socket);
            if (_session) {
                tools::memery::safeMemset((void *)&_session->_pipe, sizeof(void *), 0, sizeof(void *));
                _session->onDisconnect(g_core);
                tools::memery::safeMemset((void *)&_session, sizeof(void *), 0, sizeof(void *));
            }
            recover_to_pool(g_tcper_pool, this);
        }
    }
    
    void tcper::cache() {
        
    }
    
    void tcper::load() {
        
    }
    
    void tcper::send(const void * data, const s32 size, bool immediately) {
        if (!_connected) {
            return;
        }

        if (_send_buff.in(data, size)) {
            if (immediately) {
                struct epoll_event ev;
                ev.data.ptr = (void *) &_associat;
                ev.events = EPOLLIN | EPOLLOUT;
                epoll_ctl(g_epoller_fd, EPOLL_CTL_MOD, _socket, &ev);
            }
        } else {
            close();
        }
    }
}
