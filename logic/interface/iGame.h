#ifndef __iGame_h__
#define __iGame_h__

#include "api.h"

class iGame : public iModule {
public:
    virtual ~iGame() {}

    virtual void AiPushSoldiersGoToBattlePush(const s64 account, const s32 orbit_number) = 0;
};

#endif //__iGame_h__
