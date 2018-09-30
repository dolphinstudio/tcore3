#include "iocper.h"
#include "udp/udper.h"
#include "tcp/accepter.h"
#include "tcp/tcper.h"

namespace tcore {
    void * g_complate_port = nullptr;
    tlib::tpool<overlappedex> g_overlappedex_pool;

    bool iocper::launch() {
        WSADATA wsaData;
        s32 err = WSAStartup(MAKEWORD(1, 1), &wsaData);
        if (err != 0) {
            return false;
        }

        if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) {
            WSACleanup();
            return false;
        }

        return nullptr != (g_complate_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0));
    }

    bool iocper::launchUdpSession(iUdpSession * session, const char * ip, const s32 port) {
        udper * udp = udper::create(session, ip, port);
        if (nullptr == udp) {
            session->onCreate(false);
            return false;
        }

        iUdpPipe * pipe = udp;
        tools::memery::safeMemcpy((void *)&session->_pipe, sizeof(session->_pipe), &pipe, sizeof(pipe));
        session->onCreate(true);
        return true;
    }

    bool iocper::launchTcpSession(iTcpSession * session, const char * host, const int port, const int max_ss, const int max_rs) {
        SetLastError(0);
        DWORD ul = 1;
        int len = 0;
        LINGER linger = { 1,0 };

        bool initiative = true;
        tools::memery::safeMemcpy((void *)&session->_initiative, sizeof(bool), &initiative, sizeof(initiative));

        std::string ip;
        session->_address._ip = host;
        session->_address._port = port;
        if (!getIpByHost(host, ip)) {
            session->onConnectFailed(core::getInstance());
            return false;
        }

        u32 socket = INVALID_SOCKET;
        sockaddr_in addr;
        tools::memery::safeMemset(&addr, sizeof(addr), 0, sizeof(addr));
        addr.sin_family = AF_INET;
        if (INVALID_SOCKET == (socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED))
            || SOCKET_ERROR == setsockopt(socket, SOL_SOCKET, SO_SNDBUF, (char *)&len, sizeof(int))
            || SOCKET_ERROR == setsockopt(socket, SOL_SOCKET, SO_RCVBUF, (char *)&len, sizeof(int))
            || setsockopt(socket, SOL_SOCKET, SO_LINGER,
            (char *)&linger, sizeof(linger))
            || SOCKET_ERROR == ioctlsocket(socket, FIONBIO, &ul)
            || (SOCKET_ERROR == bind(socket, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)))
            || CreateIoCompletionPort((HANDLE)socket, (HANDLE)g_complate_port, (u_long)socket, 0) != g_complate_port) {
            close_socket(socket);
            session->onConnectFailed(core::getInstance());
            return false;
        }

        session->_address._ip = ip;
        session->_address._port = port;
        tcper * pipe = tcper::create(session, max_ss, max_rs, socket, true);
        if (nullptr == pipe) {
            close_socket(socket);
            session->onConnectFailed(core::getInstance());
            return false;
        }

        iTcpPipe * ipipe = pipe;
        tools::memery::safeMemcpy((void*)&session->_pipe, sizeof(session->_pipe), &ipipe, sizeof(ipipe));
        return true;
    }

    bool iocper::launchTcpServer(iTcpServer * server, const char * ip, const int port, const int max_ss, const int max_rs) {
        accepter * ac = accepter::create(server, ip, port, max_ss, max_rs);
        return ac != nullptr;
    }

    void iocper::deal() {
        s64 tick = tools::time::getMicrosecond();
        do {
            DWORD bytes = 0;
            SOCKET sock = INVALID_SOCKET;
            overlappedex * plus = nullptr;
            SetLastError(0);

            BOOL res = GetQueuedCompletionStatus(g_complate_port, &bytes, (PULONG_PTR)&sock, (LPOVERLAPPED *)&plus, 1);
            s32 err = GetLastError();
            if (!res) { 
                if (WAIT_TIMEOUT == err) {
                    return;
                }
            }

            plus->_completer->onCompleter(plus, plus->_type, err, bytes);
        } while (tools::time::getMicrosecond() - tick < 1000);
    }


}
