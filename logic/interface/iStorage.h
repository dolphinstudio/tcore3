#ifndef __iStorage_h__
#define __iStorage_h__

#include "api.h"
#include "http_def.h"

class iStorage : public iModule {
public:
    virtual ~iStorage() {}

    virtual const std::string getInterfaceUrl(const std::string & interface) = 0;
};

#endif //__iStorage_h__
