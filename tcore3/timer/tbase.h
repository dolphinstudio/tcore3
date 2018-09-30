#ifndef __tbase_h__
#define __tbase_h__

#include "api.h"
#include "tpool.h"
using namespace tlib;

namespace tcore {
    using namespace api;

    class tlist;
    class tbase {
    public:
        const s32 _id;
        const s64 _interval;
        const iContext _context;
        const std::string _file;
        const s32 _line;

        typedef tpool<tbase> TIMER_BASE_POOL;

        static tbase * Create(iTimer * timer, const s32 id, const iContext & context, s32 count, s64 interval, const char * file, const s32 line);

        void onTimer();
        void forceEnd();

        void pause(u64 jiffies);
        void resume(u64 jiffies);

        void release();

        inline bool isValid() const { return _valid; }
        inline bool isPolling() const { return _polling; }

        inline bool isPaused() const { return _paused; }

        inline u64 getExpire() const { return _expire; }
        inline void setExpire(u64 expire) { _expire = expire; }
        void adjustExpire(u64 now);

        inline void setNext(tbase * next) { _next = next; }
        inline tbase * getNext() const { return _next; }

        inline void setPrev(tbase * prev) { _prev = prev; }
        inline tbase * getPrev() const { return _prev; }

        inline void setList(tlist * list) { _list = list; }
        inline tlist * getList() const { return _list; }

        inline iTimer * getTimer() const { return _timer; }

    private:
        friend TIMER_BASE_POOL;
        tbase(iTimer * timer, const s32 id, const iContext & context, s32 count, s64 interval, const char * file, const s32 line);
        virtual ~tbase() {}

    private:
        tlist * _list;
        tbase * _next;
        tbase * _prev;

        iTimer * const _timer;

        bool _valid;
        bool _polling;

        u64 _expire;
        s32 _count;
        bool _started;

        u64 _pauseTick;
        bool _paused;

    };
}

#endif //__tbase_h__

