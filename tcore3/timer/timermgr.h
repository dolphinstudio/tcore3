#ifndef __timermgr_h__
#define __timermgr_h__

#include "multisys.h"
#include "api.h"
#include <string.h>

namespace tcore {
    using namespace api;

#define min_interval 10

#define jiffies_interval 2
    class tlist;
    class tbase;
    class tgear;
    class timermgr {
        enum
        {
            tq_tvn_bits = 6,
            tq_tvr_bits = 8,
            tq_tvn_size = 1 << tq_tvn_bits,//64
            tq_tvr_size = 1 << tq_tvr_bits,//256
            tq_tvn_mask = tq_tvn_size - 1,//63
            tq_tvr_mask = tq_tvr_size - 1,//255
        };

    public:
        static timermgr * getInstance() {
            static timermgr * instance = nullptr;

            if (instance == nullptr) {
                instance = NEW timermgr;
                if (!instance->launch()) {
                    DEL instance;
                    instance = nullptr;
                }
            }

            return instance;
        }


        bool launch();
        void update(const s32 overtime);
        void shutdown();

        tbase * createTimerBase(iTimer * timer, const s32 id, const iContext & context, s32 count, s64 interval, const char * file, const s32 line);
        void removeTimerBase(tbase * base, iTimer * timer, const s32 id, const iContext & context);
        tbase * findTimerBase(iTimer * timer, const s32 id, const iContext & context);

        void startTimer(iTimer * timer, const s32 id, const iContext & context, const s64 delay, const s32 count, const s64 interval, const char * file, const s32 line);
        void killTimer(iTimer * timer, const s32 id, const iContext & context);
        void pauseTimer(iTimer * timer, const s32 id, const iContext & context);
        void resumeTimer(iTimer * timer, const s32 id, const iContext & context);
        virtual void traceTimer();

        void schedule(tbase * base);
        void moveToRunning(tbase * base);

        u64 jiffies() const { return _jiffies; }

    private:
        timermgr() : _jiffies(0) {
            memset(_timerGear, 0, sizeof(_timerGear));
        }
        ~timermgr() {}

        tlist * findTimerList(u64 expire);
        void update();
        void del(tbase * base);

    private:
        u64 _jiffies;
        tgear * _timerGear[5];
        tlist * _running;
        tlist * _suspended;
    };
}

#endif //__timermgr_h__
