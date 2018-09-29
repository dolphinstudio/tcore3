#ifndef __iIDManager_h__
#define __iIDManager_h__

#include "api.h"

class iIDManager : public iModule {
public:
    virtual ~iIDManager() {}

    virtual void setmask(const u16 mask) = 0;
    virtual u64 create() = 0;
};

#endif //__iIDManager_h__
