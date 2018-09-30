#ifndef __udper_h__
#define __udper_h__

#include "interface.h"

namespace tcore {
    class udper : public iUdpPipe, public iCompleter {
    public:
        virtual ~udper() {}

        static udper * create(iUdpSession * session, const std::string & ip, const s32 port);

        virtual void cache();
        virtual void load();
        virtual void close();

        virtual void sendto(const char * ip, const s32 port, const void * context, const s32 size);
        virtual void onCompleter(associat * at, const eCompletion type, const struct epoll_event & ev);

        inline u32 getSocket() { return _socket; }

    private:
        friend tlib::tpool<udper>;
        udper(iUdpSession * session, const std::string & ip, const s32 port);
        
        bool asyncRecv();
        bool asyncSend();

    private:
        char _recv_temp[package_max_size];
        std::queue<oPackage> _send_queue;

        api::oAddress _address;
        
        iUdpSession * _session;
        u32 _socket;

        associat _at;
        bool _is_cache;
    };
}

#endif //__udper_h__
