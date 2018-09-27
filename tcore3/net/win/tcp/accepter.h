#ifndef __accepter_h__
#define __accepter_h__

#include "interface.h"

namespace tcore {
#define accept_temp_size 512

    class accepter : public iCompleter, public api::iAccepter {
    public:
        virtual ~accepter() {}

        static accepter * create(api::iTcpServer * server, const std::string & ip, const s32 port, const s32 ssize, const s32 rsize);

        virtual void onCompleter(overlappedex * ex, const eCompletion type, const s32 code, const s32 size);
        virtual void release();
        bool async_accept();

        api::iTcpServer * const _server;
        const s32 _ssize;
        const s32 _rsize;
        const oAddress _listen_addr;

    private:
        friend tlib::tpool<accepter>;
        accepter(api::iTcpServer * server, const std::string & ip, const s32 port, const s32 s_size, const s32 r_size);

    private:
        char _temp[accept_temp_size];

    private:
        u32 _socket;
        sockaddr_in _addr;
        overlappedex _ex;
    };
}

#endif //__accepter_h__


