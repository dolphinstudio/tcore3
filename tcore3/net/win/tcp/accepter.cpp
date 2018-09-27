#include "accepter.h"
#include "tcp/tcper.h"

namespace tcore {
    static tlib::tpool<accepter> static_accepter_pool;

    accepter * accepter::create(api::iTcpServer * server, const std::string & ip, const s32 port, const s32 ssize, const s32 rsize) {
        accepter * ac = create_from_pool(static_accepter_pool, server, ip, port, ssize, rsize);

        tools::memery::safeMemset(&ac->_addr, sizeof(ac->_addr), 0, sizeof(ac->_addr));
        inet_pton(AF_INET, ip.c_str(), (void *)&ac->_addr.sin_addr.s_addr);
        ac->_addr.sin_family = AF_INET;
        ac->_addr.sin_port = htons(port);

        SetLastError(0);
        int len = 0;
        if (INVALID_SOCKET == (ac->_socket = WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED))
            || SOCKET_ERROR == setsockopt(ac->_socket, SOL_SOCKET, SO_SNDBUF, (char *)&len, sizeof(int))
            || SOCKET_ERROR == setsockopt(ac->_socket, SOL_SOCKET, SO_RCVBUF, (char *)&len, sizeof(int))
            || SOCKET_ERROR == ::bind(ac->_socket, (struct sockaddr*)&(ac->_addr), sizeof(sockaddr_in))
            || listen(ac->_socket, 2048) == SOCKET_ERROR) {
            tassert(false, "socket error %d", ::GetLastError());
            close_socket(ac->_socket);
            recover_to_pool(static_accepter_pool, ac);
            server->onError(core::getInstance(), nullptr);
            return nullptr;
        }

        tassert(ac, "wtf");
        if (g_complate_port != CreateIoCompletionPort((HANDLE)ac->_socket, (HANDLE)g_complate_port, (u_long)ac->_socket, 0)) {
            server->onError(core::getInstance(), nullptr);
            recover_to_pool(static_accepter_pool, ac);
            return nullptr;
        }

        //return ac->DoAccept(sock);

        if (false == ac->async_accept()) {
            recover_to_pool(static_accepter_pool, ac);
            server->onRelease(core::getInstance());
            return nullptr;
        }


        api::iAccepter * iac = ac;
        tools::memery::safeMemcpy((void *)&server->_ac, sizeof(server->_ac), &iac, sizeof(iac));
        return ac;
    }

    accepter::accepter(api::iTcpServer * server, const std::string & ip, const s32 port, const s32 s_size, const s32 r_size)
    : _server(server), _listen_addr(ip, port), _ssize(s_size), _rsize(r_size), _socket(INVALID_SOCKET), _ex(eCompletion::accepted, this) {
    }

    void accepter::onCompleter(overlappedex * ex, const eCompletion type, const s32 code, const s32 size) {
        tassert(type == eCompletion::accepted, "accepter error on completer type");
        if (0 == code) {
            DWORD ul = 1;
            if (SOCKET_ERROR == setsockopt(_ex._socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char *)&_socket, sizeof(SOCKET))
                || CreateIoCompletionPort((HANDLE)_ex._socket, (HANDLE)g_complate_port, (u_long)_ex._socket, 0) != g_complate_port
                || SOCKET_ERROR == ioctlsocket(_ex._socket, FIONBIO, &ul)
                || SOCKET_ERROR == setsockopt(_ex._socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&ul, sizeof(ul))) {
                close_socket(_ex._socket);
            } else {
                struct sockaddr_in sa;
                int len = sizeof(sa);
                getpeername(_ex._socket, (struct sockaddr *)&sa, &len);
                const std::string ip = inet_ntoa(sa.sin_addr);
                const s32 port = ntohs(sa.sin_port);

                api::iTcpSession * session = _server->onMallocConnection(core::getInstance(), ip.c_str(), port);
                tassert(session, "malloc session nullptr");
                if (session) {
                    tcper * pipe = tcper::create(session, _ssize, _rsize, _ex._socket, false);
                    if (pipe) {
                        iTcpPipe * ipipe = pipe;
                        tools::memery::safeMemcpy((void*)&session->_pipe, sizeof(void *), &ipipe, sizeof(ipipe));
                        session->onConnected(core::getInstance());
                        session->_address._ip = ip;
                        session->_address._port = port;
                    } else {
                        close_socket(_ex._socket);
                        _server->onError(core::getInstance(), session);
                    }
                } else {
                    close_socket(_ex._socket);
                }
            }
        } else {
            close_socket(_ex._socket);
        }

        if (false == async_accept()) {
            _server->onRelease(core::getInstance());
            recover_to_pool(static_accepter_pool, this);
        }
    }

    void accepter::release() {

    }

    bool accepter::async_accept() {
        static LPFN_ACCEPTEX acceptex = getAcceptExFun();

        DWORD ul = 1;
        int len = 0;
        if (INVALID_SOCKET == (_ex._socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED))
            || SOCKET_ERROR == setsockopt(_ex._socket, SOL_SOCKET, SO_SNDBUF, (char *)&len, sizeof(int))
            || SOCKET_ERROR == setsockopt(_ex._socket, SOL_SOCKET, SO_RCVBUF, (char *)&len, sizeof(int))
            || SOCKET_ERROR == ioctlsocket(_ex._socket, FIONBIO, &ul)) {
            return false;
        }

        s32 err = GetLastError();
        DWORD bytes;
        s32 res = acceptex(
            _socket,
            _ex._socket,
            _temp,
            0,
            sizeof(struct sockaddr_in) + 16,
            sizeof(struct sockaddr_in) + 16,
            &bytes,
            (LPOVERLAPPED)&_ex
        );

        LINGER linger = { 1,0 };
        if (res == FALSE && err != WSA_IO_PENDING
            && SOCKET_ERROR != setsockopt(_socket, SOL_SOCKET, SO_SNDBUF, (char *)&len, sizeof(int))
            && SOCKET_ERROR != setsockopt(_socket, SOL_SOCKET, SO_RCVBUF, (char *)&len, sizeof(int))
            && setsockopt(_socket, SOL_SOCKET, SO_LINGER,
            (char *)&linger, sizeof(linger))) {
            close_socket(_ex._socket);
            close_socket(_socket);
            return false;
        }

        return true;
    }

}
