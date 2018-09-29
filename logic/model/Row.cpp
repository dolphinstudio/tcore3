#include "Row.h"

Row::Row(const s32 row, const MemonyLayout * layout, const char * file, const s32 line) : _file(file), _line(line), _layout(layout), _row(row) {
    tassert(_layout, "wtf");
    _memony = MemonyPool::GetInstance().Create(layout);
}

Row::~Row() {
    MemonyPool::GetInstance().Recover(_memony);
}

bool Row::SetData(const void * data, const s32 len) {
    return _memony->Copy(data, len);
}

s32 Row::GetRowIndex() const {
    return _row;
}

void Row::SetRowIndex(const s32 index) {
    _row = index;
}

s64 Row::GetDataInt64(const s32 column) const {
    const dc::layout * l = _layout->Query(column);
    tassert(l->_type == type_s64, "wtf");
    return *(s64*)_memony->Get(l);
}

float Row::GetDataFloat(const s32 column) const {
    const dc::layout * l = _layout->Query(column);
    tassert(l->_type == type_float, "wtf");
    return *(float*)_memony->Get(l);
}

bool Row::GetDataBool(const s32 column) const {
    const dc::layout * l = _layout->Query(column);
    tassert(l->_type == type_bool, "wtf");
    return *(bool*)_memony->Get(l);
}

const char * Row::GetDataString(const s32 column) const {
    const dc::layout * l = _layout->Query(column);
    tassert(l->_type == dc::type_string, "wtf");
    return (const char *)_memony->Get(l);
}

bool Row::SetDataInt64(const s32 column, const s64 value) {
    const dc::layout * l = _layout->Query(column);
    tassert(l->_type == type_s64, "wtf");
    return _memony->Set(l, &value, sizeof(value));
}

bool Row::SetDataFloat(const s32 column, const float value) {
    const dc::layout * l = _layout->Query(column);
    tassert(l->_type == type_float, "wtf");
    return _memony->Set(l, &value, sizeof(value));
}

bool Row::SetDataBool(const s32 column, const bool value) {
    const dc::layout * l = _layout->Query(column);
    tassert(l->_type == type_bool, "wtf");
    return _memony->Set(l, &value, sizeof(value));
}

bool Row::SetDataString(const s32 column, const char * value) {
    const dc::layout * l = _layout->Query(column);
    const s32 len = strlen(value);
    if (len >= l->_size) {
        ((char *)value)[len-1] = 0;
    }
    return _memony->Set(l, value, l->_size);
}
