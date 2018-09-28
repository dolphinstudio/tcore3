#ifndef __udptester_h__
#define __udptester_h__

#include "header.h"

class kcper : public iModule {
public:
    virtual ~kcper() {}

    virtual bool initialize(tcore::api::iCore * core);
    virtual bool launch(tcore::api::iCore * core);
    virtual bool destroy(tcore::api::iCore * core);
};

#endif //__udptester_h__