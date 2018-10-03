#ifndef __iRole_h__
#define __iRole_h__

#include "api.h"

class iUnit;

class iRole : public iModule {
public:
    virtual ~iRole() {}

    virtual iUnit * popRoleToOrbit(iUnit * player, iUnit * scene, iUnit * orbit) = 0;
    virtual void recoverRole(iUnit * scene, iUnit * role, const s32 side, const float y) = 0;
};

#endif //__iRole_h__
