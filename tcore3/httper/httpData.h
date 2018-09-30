#ifndef __httpData_h__
#define __httpData_h__
#include "multisys.h"
#include "tools.h"

#define http_buff_size 1024
class httpData {
public:
    void pushData(const void * data, const s32 size) {
        if (_size + size > http_buff_size) {
            char * data = NEW char[_size + size];
            if (_size != 0) {
                tools::memery::safeMemcpy(data, _size + size, _data, _size);
                if (_data != _temp) {
                    DEL[] _data;
                }
            }
            _data = data;
        }

        tools::memery::safeMemcpy(_data + _size, size, data, size);
        _size += size;
    }

    const void * getBuff(s32 & len) {
        len = _size;
        return _data;
    }

    httpData() : _data(nullptr), _size(0) {
        _data = _temp;
    }

    ~httpData() {
        if (_size > http_buff_size) {
            DEL[] _data;
        }
    }

private:
    char _temp[http_buff_size];
    char * _data;
    s32 _size;
};

#endif //__httpData_h__
