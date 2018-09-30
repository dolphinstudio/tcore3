#ifndef __interface_h__
#define __interface_h__

#include "header.h"

namespace tcore {

    class iCompleter {
    public:
        virtual ~iCompleter() {}

        virtual void onCompleter(associat * at, const eCompletion type, const struct epoll_event & ev) = 0;
    };
}

#endif //__interface_h__
