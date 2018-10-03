#ifndef __iCards_h__
#define __iCards_h__

#include "api.h"

class iUnit;

class iCards : public iModule {
public:
    virtual ~iCards() {}

    virtual void loadData(iUnit * player, const s64 copyer) = 0;

    virtual void initGameRole(iUnit * scene) = 0;
    virtual iUnit * createRole(iUnit * player, const s64 tick) = 0;
    virtual void recoverRole(iUnit * scene, iUnit * role) = 0;
};

#endif //__iCards_h__
