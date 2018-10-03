#ifndef __iEffect_h__
#define __iEffect_h__

#include "iModel.h"

class iUnit;

class iEffect : public iModule {
public:
    virtual ~iEffect() {}

    virtual void createEffect(iUnit * scene, iUnit * orbit, const s32 configid) = 0;
};

#endif //__iEffect_h__
