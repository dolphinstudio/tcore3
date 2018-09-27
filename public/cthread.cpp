#include "cthread.h"
#ifdef WIN32
#include <process.h>
#else
#include <pthread.h>
#endif //WIN32

namespace tlib {
#ifdef WIN32
    static unsigned int __stdcall threadProc(void * lpParam) {
        cthread * p = (cthread *)lpParam;
        tassert(p, "a null thread owner point");
        p->run();
        _endthreadex(0);
        return 0;
    }
#else
    static void* threadProc(void * pParam) {
        cthread * pThis = (cthread *)pParam;
        tassert(pThis, "a null thread owner point");
        pThis->run();
        return pThis;
    }
#endif

    bool cthread::start(s32 threadcount) {
        for (s32 i=0; i<threadcount; i++) {
#ifdef WIN32
            unsigned long long ret = _beginthreadex(nullptr, 0, threadProc, (void *) this, 0, nullptr);
            if (ret == -1 || ret == 1  || ret == 0) {
                return false;
            }
#else
            pthread_t ptid = 0;
            int ret = pthread_create(&ptid, nullptr, threadProc, (void*)this);
            if (ret != 0) {
                return false;
            }
#endif
        }

        return true;
    }
}
