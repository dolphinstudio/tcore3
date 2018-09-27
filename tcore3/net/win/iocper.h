#ifndef __iocper_h__
#define __iocper_h__

#include "header.h"

namespace tcore {
    using namespace api;

    class iocper : public iNet {
    public:
        virtual ~iocper() {}

        virtual bool launch();
        virtual bool launchUdpSession(iUdpSession * session, const char * ip, const s32 port);
        virtual bool launchTcpSession(iTcpSession * session, const char * host, const int port, const int max_ss, const int max_rs);
        virtual bool launchTcpServer(iTcpServer * server, const char * ip, const int port, const int max_ss, const int max_rs);
        virtual void deal();

    };

    iNet * getNetInstance() {
        static iNet * instance = nullptr;
        if (nullptr == instance) {
            instance = NEW iocper;
            if (!instance->launch()) {
                DEL instance;
            }
        }

        return instance;
    }
}


#endif //__iocper_h__
