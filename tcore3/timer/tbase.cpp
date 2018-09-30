#include "tbase.h"
#include "core.h"
#include "tools.h"

namespace tcore {
    static tbase::TIMER_BASE_POOL s_pool;

    tbase * tbase::Create(iTimer * timer, const s32 id, const iContext & context, s32 count, s64 interval, const char * file, const s32 line) {
        return create_from_pool(s_pool, timer, id, context, count, interval, file, line);
    }

    tbase::tbase(iTimer * timer, const s32 id, const iContext & context, s32 count, s64 interval, const char * file, const s32 line)
        : _timer(timer), _id(id), _interval(interval), _context(context), _file(file), _line(line) {
        _list = nullptr;
        _next = nullptr;
        _prev = nullptr;

        _valid = true;
        _polling = false;

        _expire = 0;
        _count = count;
        _started = false;

        _pauseTick = 0;
        _paused = false;
    }

    void tbase::onTimer() {
        tassert(_timer, "where is timer ???");

        _polling = true;
        if (!_started) {
            _timer->onStart(core::getInstance(), _id, _context, tools::time::getMillisecond());
            _started = true;
            tassert(_valid, "wtf");
        }

        if (_valid) {
            _timer->onTimer(core::getInstance(), _id, _context, tools::time::getMillisecond());
            if (_count > 0) {
                _count--;
            }
        }

        _polling = false;
        _expire += _interval;

        if (_count == 0 && _valid) {
            _valid = false;
            _timer->onEnd(core::getInstance(), _id, _context, true, tools::time::getMillisecond());
        }
    }

    void tbase::forceEnd() {
        tassert(_valid, "timer is already invalid");

        _valid = false;
        _timer->onEnd(core::getInstance(), _id, _context, false, tools::time::getMillisecond());
    }

    void tbase::pause(u64 jiffies) {
        if (_valid) {
            tassert(!_paused, "timer already is paused");

            _pauseTick = jiffies;
            _paused = true;
            _timer->onPause(core::getInstance(), _id, _context, tools::time::getMillisecond());
        }
    }

    void tbase::resume(u64 jiffies) {
        if (_valid) {
            tassert(_paused, "timer is not paused");

            _expire = jiffies + _expire - _pauseTick;
            _paused = false;
            _timer->onResume(core::getInstance(), _id, _context, tools::time::getMillisecond());
        }
    }

    void tbase::release() {
        recover_to_pool(s_pool, this);
    }

    void tbase::adjustExpire(u64 now) {
        long long live = (long long)(_expire - now);
        if (live < 0 && abs(live) > _interval) {
            _expire += (abs(live) / _interval) * _interval;
        }
    }
}
