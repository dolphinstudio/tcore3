#ifndef __wbServerDelegate_h__
#define __wbServerDelegate_h__

#include "header.h"

class wbServerDelegate : public iWBServerDelegate {
public:
    virtual ~wbServerDelegate() {}

    virtual iWBSessionDelegate * onMallocSessionDelegate();
};

#endif //__wbServerDelegate_h__
