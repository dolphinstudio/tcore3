#ifndef __ilock_h__
#define __ilock_h__

class ilock {
public:
    virtual ~ilock() {}
    virtual void clear() = 0;
    virtual void lock() = 0;
    virtual void unlock() = 0;
};

#endif // __ilock_h__
