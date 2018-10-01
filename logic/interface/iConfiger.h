#ifndef __iConfiger_h__
#define __iConfiger_h__

#include "api.h"
#include "auto_config.h"
class iConfig;

class iConfiger : public iModule {
public:
    virtual ~iConfiger() {}
    virtual iConfig * getConfig() = 0;
};

#endif //__iConfiger_h__
