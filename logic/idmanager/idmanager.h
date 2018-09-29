#ifndef __idmanager_h__
#define __idmanager_h__

#include "header.h"

class idmanager : public iIDManager {
public:
    virtual ~idmanager() {}

    virtual bool initialize(api::iCore * core);
    virtual bool launch(api::iCore * core);
    virtual bool destroy(api::iCore * core);

    virtual void setmask(const u16 mask);
    virtual u64 create();
};

#endif //__idmanager_h__
