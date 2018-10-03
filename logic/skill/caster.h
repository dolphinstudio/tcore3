#ifndef __caster_h__
#define __caster_h__

#include "header.h"

class caster {
public:
    static caster & getInstance();
    static bool decideCastAdvantageInferiority(iUnit * role, const eAdvantageInferiority type, iUnit * faceenemy);
};

#endif //__caster_h__
