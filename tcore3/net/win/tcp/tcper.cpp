#include "tcper.h"
#include <set>

namespace tcore {
    static tlib::tpool<tcper> static_tcper_pool;

    tcper::tcper(api::iTcpSession * session, const s32 sendsize, const s32 recvsize, u32 socket)
        : _session(session),
        _send_buff(sendsize),
        _recv_buff(recvsize),
        _socket(socket),
        _send_ex(eCompletion::sended, this),
        _recv_ex(eCompletion::recved, this),
        _recving(false),
        _sending(false),
        _caching(false) {

        tassert(_recving == false && _sending == false && _caching == false, "wtf");
        _recv_ex._wbuf.buf = _recv_temp;
        _recv_ex._wbuf.len = sizeof(_recv_temp);
    }

    bool tcper::async_recv() {
        tassert(false == _recving, "wtf");
        SetLastError(0);
        DWORD bytes, flag = 0;
        tools::memery::safeMemset(&_recv_ex._ol, sizeof(_recv_ex._ol), 0, sizeof(_recv_ex._ol));
        s32 res = WSARecv(_socket, &_recv_ex._wbuf, 1, &bytes, &flag, (LPWSAOVERLAPPED)&_recv_ex, nullptr);
        s32 err = GetLastError();
        if (SOCKET_ERROR == res && ERROR_IO_PENDING != err) {
            return false;
        }
        _recving = true;
        return true;
    }

    bool tcper::async_send() {
        tassert(_sending == false, "tcper async_send state error");
        if (_send_buff.getLength() == 0) {
            return _recving;
        }

        tools::memery::safeMemset(&_send_ex._ol, sizeof(_send_ex._ol), 0, sizeof(_send_ex._ol));
        _send_ex._wbuf.buf = (char *)_send_buff.getData();
        _send_ex._wbuf.len = _send_buff.getLength();

        s32 res = WSASend(_socket, &_send_ex._wbuf, 1, nullptr, 0, (LPWSAOVERLAPPED)&_send_ex, nullptr);
        s32 err = GetLastError();
        if (SOCKET_ERROR == res && WSA_IO_PENDING != err) {
            return false;
        }

        _sending = true;
        return true;
    }

    tcper * tcper::create(api::iTcpSession * session, const s32 sendsize, const s32 recvsize, u32 sock, bool initiative) {
        tcper * pipe = create_from_pool(static_tcper_pool, session, sendsize, recvsize, sock);

        if (initiative) {
            static LPFN_CONNECTEX connectex = getConnectExFun();
            overlappedex * ex = create_from_pool(g_overlappedex_pool, eCompletion::connected, pipe);
            tools::memery::safeMemset(&ex->_ol, sizeof(ex->_ol), 0, sizeof(ex->_ol));
            tools::memery::safeMemset(&ex->_remote, sizeof(ex->_remote), 0, sizeof(ex->_remote));
            ex->_remote.sin_addr.s_addr = inet_addr(session->_address._ip.c_str());
            ex->_remote.sin_port = htons(session->_address._port);
            ex->_remote.sin_family = AF_INET;
            ex->_socket = sock;
            DWORD bytes = 0;
            s32 res = connectex(pipe->_socket, (struct sockaddr *)&ex->_remote, sizeof(struct sockaddr_in), nullptr, 0, &bytes, (LPOVERLAPPED)ex);
            s32 err = GetLastError();
            if (SOCKET_ERROR == res && err != WSA_IO_PENDING
                && SOCKET_ERROR != setsockopt(pipe->_socket, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, nullptr, 0)) {
                recover_to_pool(g_overlappedex_pool, ex);
                recover_to_pool(static_tcper_pool, pipe);
                return nullptr;
            }
        } else {
            if (false == pipe->async_recv()) {
                close_socket(pipe->_socket);
                recover_to_pool(static_tcper_pool, pipe);
                return nullptr;
            }
        }

        return pipe;
    }

    void tcper::onCompleter(overlappedex * ex, const eCompletion type, const s32 code, const s32 size) {
        tassert(type == eCompletion::recved || type == eCompletion::sended || type == eCompletion::connected, "tcper on completer type error");
        switch (type) {
        case eCompletion::connected: {
            recover_to_pool(g_overlappedex_pool, ex);
            DWORD ul = 1;
            if (ERROR_SUCCESS == code && SOCKET_ERROR != setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&ul, sizeof(ul))) {
                if (async_recv()) {
                    api::iTcpPipe * pipe = this;
                    tools::memery::safeMemcpy((void *)&_session->_pipe, sizeof(_session->_pipe), &pipe, sizeof(pipe));
                    _session->onConnected(core::getInstance());
                    return;
                }
            }

            close_socket(_socket);
            _session->onConnectFailed(core::getInstance());
            recover_to_pool(static_tcper_pool, this);
            break;
        }
        case eCompletion::recved: {
            _recving = false;
            if (0 == code && 0 != size) {
                if (_recv_buff.in(_recv_temp, size)) {
                    if (!async_recv()) {
                        close();
                        return;
                    }

                    u32 use = 0;
                    while (_recv_buff.getLength() > 0 && (use = _session->onRecv(core::getInstance(), _recv_buff.getData(), _recv_buff.getLength())) > 0) {
                        _recv_buff.out(use);
                        if (INVALID_SOCKET == _socket) {
                            return;
                        }
                    }
                } else {
                    close();
                }
            } else {
                close();
            }
            break;
        }
        case eCompletion::sended: {
            _sending = false;
            if (0 == code) {
                if (!_send_buff.out(size)) {
                    tassert(false, "send buff out size %d error", size);
                    close();
                    return;
                }
                
                if (!async_send()) {
                    tassert(_sending == false, "send buff out size %d error", size);
                    close();
                }
            } else {
                close();
            }
            break;
        }
        default:
            break;
        }
    }

    void tcper::cache() {

    }

    void tcper::load() {

    }

    void tcper::close() {
        close_socket(_socket);
        if (false == _sending && false == _recving) {
            tools::memery::safeMemset((void *)&_session->_pipe, sizeof(_session->_pipe), 0, sizeof(_session->_pipe));
            if (_session) {
                _session->onDisconnect(core::getInstance());
            }
            recover_to_pool(static_tcper_pool, this);
        }
    }

    void tcper::send(const void * data, const s32 size, bool immediately) {
        if (!_send_buff.in(data, size)) {
            close();
            return;
        }

        if (!_sending && immediately) {
            if (!async_send()) {
                tassert(_sending == false, "wtf");
                close();
            }
        }
    }
}
