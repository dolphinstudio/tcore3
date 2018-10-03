#include "timermgr.h"
#include "tbase.h"
#include "tgear.h"
#include "tlist.h"
#include "tools.h"
#include "core.h"

#include <unordered_map>

namespace tcore {
    struct TimerContext : public iContext {
        const s32 _id;
        TimerContext(const s32 id, const iContext & context) : iContext(context), _id(id) {}

        operator size_t() const {
            return _context_mark + _id;
        }

        bool operator ==(const TimerContext & target) const {
            return _id == target._id && _context_mark == target._context_mark;
        }
    };

    typedef unordered_map<TimerContext, tbase *, tools::oHashFunction<TimerContext>> TIMEBASE_MAP;
    typedef unordered_map<iTimer *, TIMEBASE_MAP> TIME_MAP;

    static TIME_MAP s_timer_map;

    timermgr * GetTimerMgrInstance() {
        return timermgr::getInstance();
    }

    bool timermgr::launch() {
        _timerGear[4] = NEW tgear(tq_tvn_size, 0);
        _timerGear[3] = NEW tgear(tq_tvn_size, _timerGear[4]);
        _timerGear[2] = NEW tgear(tq_tvn_size, _timerGear[3]);
        _timerGear[1] = NEW tgear(tq_tvn_size, _timerGear[2]);
        _timerGear[0] = NEW tgear(tq_tvr_size, _timerGear[1]);

        _running = NEW tlist;
        _suspended = NEW tlist;
        return true;
    }

    void timermgr::shutdown() {
        for (s32 i = 0; i < 5; ++i) {
            DEL _timerGear[i];
        }
        DEL _running;
        DEL _suspended;
        DEL this;
    }

    void timermgr::update(const s32 overtime) {
        static s64 last = tools::time::getMillisecond();
        s64 tick = tools::time::getMillisecond();

        s32 count = (s32)(tick - last) / jiffies_interval;
        for (s32 i = 0; i < count; ++i)
            update();
        last += count * jiffies_interval;

        while (!_running->empty()) {
            tbase * base = _running->popFront();
            if (!base) {
                //error(core::getInstance(), "where is timer base, _running %x", _running);
            }

            base->onTimer();
            if (!base->isValid())
                del(base);
            else if (base->isPaused())
                _suspended->pushBack(base);
            else {
                base->adjustExpire(_jiffies);
                schedule(base);
            }
        }
    }


    tbase * timermgr::createTimerBase(iTimer * timer, const s32 id, const iContext & context, s32 count, s64 interval, const char * file, const s32 line) {
        TIME_MAP::iterator itor = s_timer_map.find(timer);
        if (itor == s_timer_map.end()) {
            s_timer_map.insert(make_pair(timer, TIMEBASE_MAP()));
            itor = s_timer_map.find(timer);
        }

        TimerContext ct(id, context);
        tbase * base = tbase::Create(timer, id, context, count, interval / jiffies_interval, file, line);
        itor->second.insert(make_pair(ct, base));
        //trace(core::getInstance(), "insert timer base %x, timer %x, id %d, context %lld | %llx", base, timer, id, context._context_mark, context._context_point);
        return base;
    }

    void timermgr::removeTimerBase(tbase * base, iTimer * timer, const s32 id, const iContext & context) {
        TIME_MAP::iterator itor = s_timer_map.find(timer);

        TimerContext ct(id, context);
        if (itor != s_timer_map.end()) {
            TIMEBASE_MAP::iterator ifind = itor->second.find(ct);
            if (ifind != itor->second.end() && ifind->second == base) {
                //debug(core::getInstance(), "remove base %x, timer %x, id %d, context %lld | %llx", ifind->second, timer, id, context._context_mark, context._context_point);
                itor->second.erase(ifind);
                return;
            }
        }

        ////error(core::getInstance(), "cant find timer base, timer %x, id %d context %lld | %llx", timer, id, context._context_mark, context._context_point);
    }

    tbase * timermgr::findTimerBase(iTimer * timer, const s32 id, const iContext & context) {
        TIME_MAP::iterator itor = s_timer_map.find(timer);
        if (itor != s_timer_map.end()) {
            TimerContext ct(id, context);

            TIMEBASE_MAP::iterator ifind = itor->second.find(ct);
            if (ifind != itor->second.end()) {
                return ifind->second;
            }
        }

        return nullptr;
    }

    void timermgr::startTimer(iTimer * timer, const s32 id, const iContext & context, s64 delay, s32 count, s64 interval, const char * file, const s32 line) {
        tbase * base = findTimerBase(timer, id, context);
        if (base) {
            ////error(core::getInstance(), "already has timer base %x %s:%d, remover it", base, base->_file.GetString(), base->_line);
            return;
        }
        tassert(interval > 0 && delay >= 0, "wtf");
        if (interval < min_interval)
            interval = min_interval;

        base = createTimerBase(timer, id, context, count, interval, file, line);
        if (delay > 0) {
            if (delay < min_interval)
                delay = min_interval;
        }
        base->setExpire(_jiffies + delay / jiffies_interval);
        schedule(base);
    }

    void timermgr::killTimer(iTimer * timer, const s32 id, const iContext & context) {
        tbase * base = findTimerBase(timer, id, context);
        if (base == nullptr || !base->isValid()) {
            //trace(core::getInstance(), "timer base maybe deleted timer %x, id %d", timer, id);
            return;
        }

        base->forceEnd();
        if (!base->isPolling()) {
            if (!base->getList()) {
                ////error(core::getInstance(), "base is not in a timer list, base %x, timer %x, id %d", base, timer, id);
            }

            base->getList()->remove(base);
            del(base);
        } else {
            removeTimerBase(base, timer, id, context);
        }
    }

    void timermgr::pauseTimer(iTimer * timer, const s32 id, const iContext & context) {
        tbase * base = findTimerBase(timer, id, context);
        if (base == nullptr) {
            ////error(core::getInstance(), "where is timer base, timer %x, id %d", timer, id);
            return;
        }

        if (!base->isValid())
            return;

        base->pause(_jiffies);
        if (base->isValid() && !base->isPolling()) {
            if (!base->getList()) {
                //error(core::getInstance(), "base is not in a pause timer list, timer %x, id %d", timer, id);
            }

            base->getList()->remove(base);
            _suspended->pushBack(base);
        }
    }

    void timermgr::resumeTimer(iTimer * timer, const s32 id, const iContext & context) {
        tbase * base = findTimerBase(timer, id, context);
        if (base == nullptr) {
            //error(core::getInstance(), "where is timer base, timer %x, id %d", timer, id);
            return;
        }

        if (base->getList() != _suspended) {
            //error(core::getInstance(), "base %x is not in supended list, timer %x, id %d", base, timer, id);
        }

        if (!base->isValid())
            return;

        base->resume(_jiffies);
        if (base->isValid() && !base->isPolling()) {
            base->getList()->remove(base);

            schedule(base);
        }

        //trace(core::getInstance(), "base %x resume, timer %x, id %d, context %lld | %llx", base, timer, id, context._context_mark, context._context_point);
    }

    void timermgr::traceTimer() {
//         core::getInstance()->LogAsync("==============================trace timer start==========================", true);
//         TIME_MAP::iterator itor = s_timer_map.begin();
//         for (TIME_MAP::iterator i = s_timer_map.begin(); i != s_timer_map.end(); i++) {
//             for (TIMEBASE_MAP::iterator j = i->second.begin(); j != i->second.end(); j++) {
//                 std::string trace;
//                 trace << j->second->_file.GetString() << ":" << j->second->_line;
//                 core::getInstance()->LogAsync(trace.GetString(), true);
//             }
//         }
//         core::getInstance()->LogAsync("==============================trace timer end==========================", true);
    }

    void timermgr::schedule(tbase * base) {
        tlist * vec = findTimerList(base->getExpire());
        tassert(vec, "find timer list failed");

        vec->pushBack(base);
    }

    void timermgr::moveToRunning(tbase * base) {
        _running->pushBack(base);
    }

    tlist* timermgr::findTimerList(u64 expire) {
        u64 live = expire - _jiffies;
        tlist* vec = 0;
        if (live < tq_tvr_size)
            vec = _timerGear[0]->getTimerList(expire & tq_tvr_mask);
        else if (live < (1 << (tq_tvr_bits + tq_tvn_bits)))
            vec = _timerGear[1]->getTimerList((expire >> tq_tvr_bits) & tq_tvn_mask);
        else if (live < (1 << (tq_tvr_bits + 2 * tq_tvn_bits)))
            vec = _timerGear[2]->getTimerList((expire >> (tq_tvr_bits + tq_tvn_bits)) & tq_tvn_mask);
        else if (live < (1 << (tq_tvr_bits + 3 * tq_tvn_bits)))
            vec = _timerGear[3]->getTimerList((expire >> (tq_tvr_bits + 2 * tq_tvn_bits)) & tq_tvn_mask);
        else if ((long long)live < 0)
            vec = _running;
        else
            vec = _timerGear[4]->getTimerList((expire >> (tq_tvr_bits + 3 * tq_tvn_bits)) & tq_tvn_mask);

        return vec;
    }

    void timermgr::update() {
        tassert(_timerGear[0], "where is timer gear");

        _timerGear[0]->checkHighGear();
        ++_jiffies;
        _timerGear[0]->update();
    }

    void timermgr::del(tbase * base) {
        TIME_MAP::iterator itor = s_timer_map.find(base->getTimer());
        if (itor != s_timer_map.end()) {
            TIMEBASE_MAP::iterator ifind = itor->second.find(TimerContext(base->_id, base->_context));
            if (ifind != itor->second.end() && ifind->second == base) {
                itor->second.erase(ifind);
            }
        }

        base->release();
    }
}
