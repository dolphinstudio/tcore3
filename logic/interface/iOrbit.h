#ifndef __iOrbit_h__
#define __iOrbit_h__

#include "api.h"

class iUnit;

class iOrbit : public iModule {
public:
    virtual ~iOrbit() {}

    virtual iUnit * GetOrbit(iUnit * scene, const s32 orbit_number) = 0;
};

#endif //__iOrbit_h__
