#ifndef __configer_h__
#define __configer_h__

#include "header.h"

class configer : public iConfiger {
public:
    virtual ~configer() {}

    virtual bool initialize(api::iCore * core);
    virtual bool launch(api::iCore * core);
    virtual bool destroy(api::iCore * core);

    virtual iConfig * getConfig();
};

#endif //__Configer_h__
