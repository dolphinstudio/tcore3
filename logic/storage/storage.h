#ifndef __storage_h__
#define __storage_h__

#include "header.h"

class storage : public iStorage {
public:
    virtual ~storage() {}

    virtual bool initialize(api::iCore * core);
    virtual bool launch(api::iCore * core);
    virtual bool destroy(api::iCore * core);

    virtual const std::string getInterfaceUrl(const std::string & interface);
};

#endif //__storage_h__
