#ifndef __cbuffer_h__
#define __cbuffer_h__

#include "multisys.h"
#include "tools.h"

namespace tlib{
    class cbuffer {
    public:
        cbuffer(const u32 size) : _size(size), _in(0), _out(0) {
            tassert(size > 0, "wtf");
            _data = NEW char[size];
        }

        ~cbuffer() {
            DEL[] _data;
        }

        inline bool in(const void * data, const u32 size) {
            tassert(_in >= _out && _in <= _size, "wtf");
            if (_size - getLength() < size) {
                return false;
            }

            if (_size - _in < size) {
                if (_in == _out) {
                    _in = 0;
                    _out = 0;
                } else {
                    tools::memery::safeMemcpy(_data, _size, _data + _out, _in - _out);
                    _in -= _out;
                    _out = 0;
                }
            }

            tassert(_in < _size && _out < _size, "wtf");
            tools::memery::safeMemcpy(_data + _in, _size - getLength(), data, size);
            _in += size;
            tassert(_in <= _size, "wtf");
            return true;
        }

        inline bool out(const u32 size) {
            tassert(_out + size <= _in && _in <= _size, "wtf");
            if (_out + size > _in) {
                return false;
            }

            _out += size;
            return true;
        }

        inline const void * getData() const {
            return _data + _out;
        }

        inline const u32 getLength() const {
            tassert(_in >= _out && _in <= _size, "wtf");
            return _in - _out;
        }

    private:
        const u32 _size;
        u32 _in;
        u32 _out;
        char * _data;
    };
}

#endif //__CircularBuffer_h__
