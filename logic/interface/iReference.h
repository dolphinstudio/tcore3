#ifndef __iReference_h__
#define __iReference_h__

#include "multisys.h"

class iReference {
public:
    virtual ~iReference() {}
    iReference() : _reference(0) {}

    virtual void retain() {
        _reference++;
    }

    virtual void release() {
        _reference--;
        tassert(_reference >= 0, "wtf");
        if (0 == _reference) {
            recover();
        }
    }

protected:
    virtual void recover() = 0;

private:
    int _reference;
};

#endif //__iReference_h__
