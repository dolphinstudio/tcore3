#ifndef __spinlock_h__
#define __spinlock_h__

#include "ilock.h"
#include <atomic>

namespace tlib {
    class spinlock : public ilock {
        std::atomic_flag _flag;
    public:
#ifdef WIN32
        spinlock() {_flag._My_flag = 0;}
#else
        spinlock() : _flag(ATOMIC_FLAG_INIT) {}
#endif //WIN32

        virtual void clear() {
            _flag.clear(std::memory_order_release);
        }

        virtual void lock() {
            while (_flag.test_and_set(std::memory_order_acquire));
        }

        virtual void unlock() {
            _flag.clear(std::memory_order_release);
        }
    };
}

#endif //__spinlock_h__
