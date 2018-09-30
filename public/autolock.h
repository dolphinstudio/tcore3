#ifndef __autolock_h__
#define __autolock_h__

#include "spinlock.h"
#include "mutexlock.h"

namespace tlib {
    class autolock {
    public:
        autolock(ilock * pLock) {
            tassert(pLock != nullptr, "mutexlock point null");
            _lock = pLock;
            _lock->lock();
        }

        void free() {
            tassert(_lock != nullptr, "mutexlock point null");
            _lock->unlock();
        }

        ~autolock() {
            _lock->unlock();
        }

    private:
        ilock * _lock;
    };
}

#endif //defined __CLock_h__

