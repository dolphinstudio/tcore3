#ifndef __udper_h__
#define __udper_h__

#include "interface.h"

namespace tcore {

    class udper : public iUdpPipe, public iCompleter {
    public:
        virtual ~udper() {}
        udper(iUdpSession * session, const std::string & ip, const s32 port);

        static udper * create(iUdpSession * session, const std::string & ip, const s32 port);
        void release();

        virtual void cache();
        virtual void load();
        virtual void close();

        virtual void sendto(const char * ip, const s32 port, const void * context, const s32 size);
        virtual void onCompleter(overlappedex * ex, const eCompletion type, const s32 code, const s32 size);

        virtual u32 getSocket() { return _socket; }

    private:
        bool asyncRecv();
        bool asyncSend();

    private:
        char _recv_temp[package_max_size];
        std::queue<oPackage> _send_queue;

        iUdpSession * _session;
        u32 _socket;

        overlappedex _send_ex;
        overlappedex _recv_ex;
        bool _is_cache;
        bool _is_recving;
        bool _is_sending;
    };
}

#endif //__udper_h__
