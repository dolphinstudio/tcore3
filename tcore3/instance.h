#ifndef __instance_h__
#define __instance_h__

namespace tcore {
    using namespace api;

    class iNet {
    public:
        virtual ~iNet() {}

        virtual bool launch() = 0;
        virtual bool launchUdpSession(iUdpSession * session, const char * ip, const s32 port) = 0;
        virtual bool launchTcpSession(iTcpSession * session, const char * host, const int port, int max_ss, int max_rs) = 0;
        virtual bool launchTcpServer(iTcpServer * server, const char * ip, const int port, int max_ss, int max_rs) = 0;
        virtual void deal() = 0;
    };

    iNet * getNetInstance();
}


#endif //__instance_h__
