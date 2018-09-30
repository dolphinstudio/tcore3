#ifndef __header_h__
#define __header_h__

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>
#pragma  comment(lib, "ws2_32.lib")

#include <queue>

#include "api.h"
#include "instance.h"
#include "tools.h"
#include "tpool.h"
#include "core.h"
#include "cbuffer.h"

namespace tcore {
    extern void * g_complate_port;

    enum eCompletion {
        accepted,
        connected,
        sended,
        recved
    };

    class oPackage {
    public:
        void * const _data;
        const s32 _len;
        const std::string _ip;
        const s32 _port;

        oPackage(const void * data, const s32 len, const std::string & ip, const s32 port) : _data(NEW char[len]), _len(len), _ip(ip),_port(port) {
            tools::memery::safeMemcpy(_data, _len, data, len);
        }
        oPackage(const oPackage & package) : _data(package._data), _len(package._len), _ip(package._ip), _port(package._port) {}
    };

    class iCompleter;

    struct overlappedex {
        OVERLAPPED _ol;
        WSABUF _wbuf;
        u32 _socket;
        sockaddr_in _remote;

        const eCompletion _type;
        iCompleter * const _completer;

        overlappedex(const eCompletion type, iCompleter * const completer) : _type(type), _completer(completer) {
            tools::memery::safeMemset(&_ol, sizeof(_ol), 0, sizeof(_ol));
        }
    };

    extern tlib::tpool<overlappedex> g_overlappedex_pool;

    class udper;
    extern tlib::tpool<udper> g_udper_pool;

    static LPFN_ACCEPTEX getAcceptExFun() {
        static LPFN_ACCEPTEX fun = nullptr;
        if (nullptr == fun) {
            GUID guid = WSAID_ACCEPTEX;
            DWORD dwBytes = 0;
            SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), &fun, sizeof(fun), &dwBytes, nullptr, nullptr);

            tassert(fun, "Get AcceptEx fun error, error code : %d", GetLastError());
        }

        return fun;
    }

    static LPFN_CONNECTEX getConnectExFun() {
        static LPFN_CONNECTEX fun = nullptr;
        if (nullptr == fun) {
            GUID guid = WSAID_CONNECTEX;
            DWORD bytes = 0;
            SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), &fun, sizeof(fun), &bytes, nullptr, nullptr);
            tassert(fun, "get connectex fun error %d", GetLastError());
        }

        return fun;
    }

    static bool getIpByHost(const char * host, OUT std::string & ip) {
        struct hostent * hp;
        if ((hp = gethostbyname(host)) == nullptr) {
            return false;
        }

        ip = inet_ntoa(*(struct in_addr*)hp->h_addr);
        return true;
    }

#   define close_socket(sock) {\
        closesocket(sock); \
        sock = INVALID_SOCKET; \
    }
}


#endif //__header_h__
