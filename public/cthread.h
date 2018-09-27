#ifndef CTHREAD_H
#define CTHREAD_H

#include "multisys.h"

namespace tlib {
    enum {
        THREAD_STOPED = 0,
        THREAD_WORKING,
        THREAD_STOPPING,
    };

    class cthread {
    public:
        virtual ~cthread() {}

        //interface
    public:
        bool start(s32 threadcount = 1);

        virtual void terminate() = 0; // not safe
        virtual void run() = 0;

    protected:
        s8 _status;
    };
}

#endif //define CTHREAD_H
