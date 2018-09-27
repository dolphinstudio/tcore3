#ifndef __tcper_h__
#define __tcper_h__

#include "interface.h"

namespace tcore {
    class tcper : public iCompleter, public api::iTcpPipe {
    public:
        virtual ~tcper() {}
        
        static tcper * create(api::iTcpSession * session, const s32 sock, const string & ip, const s32 port, s32 ssize, s32 rsize);
        static tcper * create(api::iTcpSession * session, const string & ip, const s32 port, s32 ssize, s32 rsize);
        
        virtual void onCompleter(associat * at, const eCompletion type, const struct epoll_event & ev);
        
        virtual void close();
        virtual void cache();
        virtual void load();
        virtual void send(const void * data, const s32 size, bool immediately);
        
    private:
        friend accepter;
        friend tlib::tpool<tcper>;
        tcper(api::iTcpSession * session, const string & ip, const s32 port, const s32 ssize, const s32 rsize);
    private:
        bool _connected;
        bool _caching;
        s32 _socket;
        const associat _associat;
        const api::oAddress _remote;
        api::iTcpSession * const _session;

        tlib::cbuffer _send_buff;
        tlib::cbuffer _recv_buff;
    };
}

#endif //__tcper_h__
