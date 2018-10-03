#ifndef __iSkill_h__
#define __iSkill_h__

#include "api.h"

class iUnit;

class iSkill : public iModule {
public:
    virtual ~iSkill() {}

    virtual bool disappearanceDetectionWithRole(iUnit * role1, iUnit * role2) = 0;
    virtual bool disappearanceDetectionWithCombiner(iUnit * role, iUnit * combiner) = 0;
};

#endif //__iSkill_h__
