#ifndef __iCombiner_h__
#define __iCombiner_h__

#include "api.h"

class iUnit;

class iCombiner : public iModule {
public:
    virtual ~iCombiner() {}

    virtual iUnit * MergeToCombiner(iUnit * orbit, iUnit * toper, iUnit * bottomer) = 0;
    virtual void RecoverCombiner(iUnit * scene, iUnit * orbit, iUnit * combiner) = 0;
};

#endif //__iCombiner_h__
