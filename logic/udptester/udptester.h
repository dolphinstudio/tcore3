#ifndef __udptester_h__
#define __udptester_h__

#include "header.h"

class udptester : public iModule {
public:
    virtual ~udptester() {}

    virtual bool initialize(tcore::api::iCore * core);
    virtual bool launch(tcore::api::iCore * core);
    virtual bool destroy(tcore::api::iCore * core);
};

#endif //__udptester_h__