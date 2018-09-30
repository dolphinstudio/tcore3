#ifndef __interface_h__
#define __interface_h__

#include "header.h"

namespace tcore {

    class iCompleter {
    public:
        virtual ~iCompleter() {}

        virtual void onCompleter(overlappedex * ex, const eCompletion type, const s32 code, const s32 size) = 0;
    };
}

#endif //__interface_h__
