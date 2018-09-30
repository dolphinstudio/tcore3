#include "row.h"

row::row(const s32 row, const mlayout * layout, const char * file, const s32 line) : _file(file), _line(line), _layout(layout), _row(row) {
    tassert(_layout, "wtf");
    _memony = memonyPool::getInstance().create(layout);
}

row::~row() {
    memonyPool::getInstance().recover(_memony);
}

bool row::setData(const void * data, const s32 len) {
    return _memony->copy(data, len);
}

s32 row::getIndex() const {
    return _row;
}

void row::setIndex(const s32 index) {
    _row = index;
}

s64 row::getInt64(const s32 column) const {
    const dc::layout * l = _layout->query(column);
    tassert(l->_type == type_s64, "wtf");
    return *(s64*)_memony->get(l);
}

float row::getFloat(const s32 column) const {
    const dc::layout * l = _layout->query(column);
    tassert(l->_type == type_float, "wtf");
    return *(float*)_memony->get(l);
}

bool row::getBool(const s32 column) const {
    const dc::layout * l = _layout->query(column);
    tassert(l->_type == type_bool, "wtf");
    return *(bool*)_memony->get(l);
}

const char * row::getString(const s32 column) const {
    const dc::layout * l = _layout->query(column);
    tassert(l->_type == dc::type_string, "wtf");
    return (const char *)_memony->get(l);
}

bool row::setInt64(const s32 column, const s64 value) {
    const dc::layout * l = _layout->query(column);
    tassert(l->_type == type_s64, "wtf");
    return _memony->Set(l, &value, sizeof(value));
}

bool row::setFloat(const s32 column, const float value) {
    const dc::layout * l = _layout->query(column);
    tassert(l->_type == type_float, "wtf");
    return _memony->Set(l, &value, sizeof(value));
}

bool row::setBool(const s32 column, const bool value) {
    const dc::layout * l = _layout->query(column);
    tassert(l->_type == type_bool, "wtf");
    return _memony->Set(l, &value, sizeof(value));
}

bool row::setString(const s32 column, const char * value) {
    const dc::layout * l = _layout->query(column);
    const s32 len = strlen(value);
    if (len >= l->_size) {
        ((char *)value)[len-1] = 0;
    }
    return _memony->Set(l, value, l->_size);
}
