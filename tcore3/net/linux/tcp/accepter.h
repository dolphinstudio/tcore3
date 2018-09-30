#ifndef __accepter_h__
#define __accepter_h__

#include "interface.h"

namespace tcore {
    class accepter : public iCompleter, public api::iAccepter {
    public:
        virtual ~accepter() {}
        
        static accepter * create(iTcpServer * server, const char * ip, const s32 port, s32 s_size, s32 r_size);
        
        virtual void onCompleter(associat * at, const eCompletion type, const struct epoll_event & ev);
        virtual void release();
        
    private:
        friend tlib::tpool<accepter>;
        accepter(iTcpServer * server, const char * ip, const s32 port, s32 s_size, s32 r_size);
        
    private:
        iTcpServer * const _server;
        const s32 _ssize;
        const s32 _rsize;
        api::oAddress _address;
        s32 _socket;
        sockaddr_in _addr;
        associat _at;
    };
}

#endif //__accepter_h__
