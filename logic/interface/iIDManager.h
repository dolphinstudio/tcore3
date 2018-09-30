#ifndef __iIDManager_h__
#define __iIDManager_h__

#include "api.h"

class iIdmanager : public iModule {
public:
    virtual ~iIdmanager() {}

    virtual void setmask(const u16 mask) = 0;
    virtual u64 create() = 0;
};

#endif //__iIDManager_h__
