#ifndef __header_h__
#define __header_h__

#include "api.h"
#include "instance.h"
#include "tools.h"
#include "tpool.h"
#include "core.h"
#include "cbuffer.h"

#include <queue>

#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/resource.h>
#include <netinet/tcp.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <netdb.h>
#include "core.h"


#define socket_error -1
#define invalid_fd -1
#define success 0
#define epoller_ev_max_count 512
#define epoller_desc_count 10240
#define max_pipe_size (20 * 1024 * 1024)
#define recv_temp_size 4096

namespace tcore {
    using namespace api;
    
    extern core * g_core;
    extern s32 g_epoller_fd;
    
    
    enum eCompletion {
        doAccept,
        doConnect,
        doIO,
    };
    
    class iCompleter;
    
    struct associat {
        const eCompletion _ev;
        iCompleter * const _completer;

        associat(const eCompletion ev, iCompleter * const p) : _ev(ev), _completer(p) {}
    };
    extern tlib::tpool<associat> g_associat_pool;

    class udper;
    extern tlib::tpool<udper> g_udper_pool;
    
    class tcper;
    extern tlib::tpool<tcper> g_tcper_pool;
    
    class accepter;
    extern tlib::tpool<accepter> g_accepter_pool;
    
#   define close_socket(sock) {\
        ::close(sock); \
        sock = invalid_fd; \
    }
    
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
}

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus__

    inline int setnonblocking(int sockfd) {
        return fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
    }

    inline s32 settcpnodelay(const s32 fd) {
        int val = 1l;
        return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char *) &val, sizeof (val));
    }

    inline s32 settcpquickack(const s32 fd) {
	    int val = 1l;
        return setsockopt(fd, IPPROTO_TCP, TCP_QUICKACK, (const char *)&val, sizeof(val));
    }

    inline s32 setreuse(const s32 fd) {
        int val = 1l;
        return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *) &val, sizeof (val));
    }

    inline int setsocksendbuff(int sock, int size) {
        return setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (const char *) &size, sizeof (size));
    }

    inline int setsockrecvbuff(int sock, int size) {
        return setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (const char *) &size, sizeof (size));
    }

    inline s32 setmaxopenfile(const s32 size) {
        struct rlimit rt;
        rt.rlim_max = rt.rlim_cur = size;
        if (setrlimit(RLIMIT_NOFILE, &rt) == -1) return -1;
        return 0;
    }

    inline s32 setstacksize(const s32 size) {
        struct rlimit rt;
        rt.rlim_max = rt.rlim_cur = size * 1024;
        if (setrlimit(RLIMIT_STACK, &rt) == -1) return -1;
        return 0;
    }

    inline bool getIpByHost(const char * host, OUT std::string & ip) {
        struct hostent * hp;
        if ((hp = gethostbyname(host)) == NULL) {
            return false;
        }

        ip = inet_ntoa(*(struct in_addr*)hp->h_addr);
        return true;
    }
    
#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__header_h__
