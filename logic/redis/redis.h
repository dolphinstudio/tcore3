#ifndef __redis_h__
#define __redis_h__

#include "header.h"

class rediser;

class redis : public iRedis {
public:
    virtual ~redis() {}

    virtual bool initialize(api::iCore * core);
    virtual bool launch(api::iCore * core);
    virtual bool destroy(api::iCore * core);

    virtual iRediser * getMasterRediser();
    virtual iRediser * getRediser(const s64 account);
};

#endif //__redis_h__