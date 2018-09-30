#ifndef __tqueue_h__
#define __tqueue_h__
#include "multisys.h"

enum {
    not_exists_data = 0,
    exists_data = 1
};

namespace tlib {
    template <typename type, const s32 size = 4096>
    class tqueue {
    public:
        tqueue() {
            _rindex = 0;
            _windex = 0;
            _rcount = 0;
            _wcount = 0;

            memset(_sign, 0, sizeof(_sign));
        }

        ~tqueue() {}

        inline void push(type src) {
            while (_sign[_windex] != not_exists_data) {
                msleep(1);
            }

            _queue[_windex] = src;
            _sign[_windex++] = exists_data;
            _wcount++;
            if (_windex >= size) {
                _windex = 0;
            }
        }

        inline bool tryPush(type src) {
            while (_sign[_windex] != not_exists_data) {
                return false;
            }

            _queue[_windex] = src;
            _sign[_windex++] = exists_data;
            _wcount++;
            if (_windex >= size) {
                _windex = 0;
            }
            return true;
        }

        inline void pull(type & value) {
            while (_sign[_rindex] != exists_data) {
                msleep(1);
            }

            value = _queue[_rindex];
            _sign[_rindex++] = not_exists_data;
            _rcount++;

            if (_rindex >= size) {
                _rindex = 0;
            }
        }


        inline bool tryPull(type & value) {
            while (_sign[_rindex] != exists_data) {
                return false;
            }

            value = _queue[_rindex];
            _sign[_rindex++] = not_exists_data;
            _rcount++;

            if (_rindex >= size) {
                _rindex = 0;
            }

            return true;
        }

        inline bool isEmpty() {
            return (_rcount == _wcount);
        }

    private:
        type _queue[size];
        s8 _sign[size];
        u32 _rindex;
        u32 _windex;
        u32 _rcount;
        u32 _wcount;
    };
}
#endif //CQUEUE_H
