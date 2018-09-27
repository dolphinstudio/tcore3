#ifndef __epoller_h__
#define __epoller_h__

#include "interface.h"

namespace tcore {
    using namespace api;

    class epoller : public iNet {
    public:
        virtual ~epoller() {}

        static epoller * getInstance();
        
        virtual bool launch();
        virtual bool launchUdpSession(iUdpSession * session, const char * ip, const s32 port);
        virtual bool launchTcpSession(iTcpSession * session, const char * host, const int port, int max_ss, int max_rs);
        virtual bool launchTcpServer(iTcpServer * server, const char * ip, const int port, int max_ss, int max_rs);
        virtual void deal();

    };

    iNet * getNetInstance() {
        return epoller::getInstance();
    }
}

#endif //__epoller_h__
