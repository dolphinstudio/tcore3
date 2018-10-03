#ifndef __iBuff_h__
#define __iBuff_h__

#include "api.h"
#include <set>

class iUnit;

class iBuff : public iModule {
public:
    virtual ~iBuff() {}

    virtual void createBuff(iUnit * scene, const s32 configid, const s64 target) = 0;
    virtual void createBuff(iUnit * scene, const s32 configid, const std::set<s64> & targets) = 0;
    virtual void recoverBuff(iUnit * buff) = 0;
};

#endif //__iBuff_h__
