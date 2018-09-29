#include "Unit.h"
#include "Memony.h"
#include "Table.h"
using namespace dc;

Unit::Unit(const char * name, const s64 id, const MemonyLayout * layout, const char * file, const s32 line) 
    : _name(name), _id(id), _layout(layout), _file(file), _line(line), _create_tick(tools::time::GetTimeMillisecond()) {
    tassert(layout, "wtf");
    _memony = MemonyPool::GetInstance().Create(layout);

    object_table_layouts::iterator itor = g_object_table_layouts.find(name);
    if (itor != g_object_table_layouts.end()) {
        for (s32 i = 0; i < itor->second.size(); i++) {
            Table * table = CREATE_FROM_POOL(g_table_pool, this, itor->second[i]._layout, itor->second[i]._key, file, line);
            _tables.push_back(table);
        }
    }
}

Unit::~Unit() {
    for (s32 i = 0; i < _tables.size(); i++) {
        g_table_pool.Recover(_tables[i]);
    }
    _any_call_list.clear();
    _attr_handle_map.clear();
    MemonyPool::GetInstance().Recover(_memony);
}

const s64 Unit::GetID() const {
    return _id;
}

const char * Unit::GetName() const {
    return _name.c_str();
}

const std::vector<dc::layout>& Unit::GetLayout() const {
    return _layout->GetLayout();
}

bool Unit::SetAttrInt64(const dc::layout & info, const s64 value, const bool sync) {
    tassert(info._type == type_s64, "wtf");
    s64 old = 0;
    const void * p = _memony->Get(&info);
    if (p) {
        old = *(s64 *)p;
    }

    if (value == old) {
        return true;
    }

    if (!_memony->Set(&info, &value, sizeof(value))) {
        return false;
    }

    EveryAttrChangedCall(info, &old, &value, sync);
    AttrChangedCall(info, &old, &value, sync);
    return true;
}

bool Unit::SetAttrFloat(const dc::layout & info, const float value, const bool sync) {
    tassert(info._type == type_float, "wtf");
    float old = 0;
    const void * p = _memony->Get(&info);
    if (p) {
        old = *(float *)p;
    }

    if (!_memony->Set(&info, &value, sizeof(value))) {
        return false;
    }

    EveryAttrChangedCall(info, &old, &value, sync);
    AttrChangedCall(info, &old, &value, sync);
    return true;
}

bool Unit::SetAttrBool(const dc::layout & info, const bool value, const bool sync) {
    tassert(info._type == type_bool, "wtf");
    bool old = 0;
    const void * p = _memony->Get(&info);
    if (p) {
        old = *(bool *)p;
    }

    if (!_memony->Set(&info, &value, sizeof(value))) {
        return false;
    }

    EveryAttrChangedCall(info, &old, &value, sync);
    AttrChangedCall(info, &old, &value, sync);
    return true;
}


bool Unit::SetAttrString(const dc::layout & info, const char * value, const bool sync) {
    tassert(info._type == type_string, "wtf");
    const s32 len = strlen(value);
    if (len >= info._size) {
        ((char *)value)[len - 1] = 0;
    }

    static std::string s_old;
    const char * p = (const char *)_memony->Get(&info);
    if (p) {
        s_old = p;
    } else {
        s_old = "";
    }

    if (!_memony->Set(&info, value, info._size)) {
        return false;
    }

    EveryAttrChangedCall(info, s_old.c_str(), value, sync);
    AttrChangedCall(info, s_old.c_str(), value, sync);
    return true;
}

s64 Unit::GetAttrInt64(const dc::layout & info) const {
    tassert(info._type == type_s64, "wtf");
    return *(s64*)_memony->Get(&info);
}

float Unit::GetAttrFloat(const dc::layout & info) const {
    tassert(info._type == type_float, "wtf");
    return *(float*)_memony->Get(&info);
}

bool Unit::GetAttrBool(const dc::layout & info) const {
    tassert(info._type == type_bool, "wtf");
    return *(bool*)_memony->Get(&info);
}

const char * Unit::GetAttrString(const dc::layout & info) const {
    tassert(info._type == type_string, "wtf");
    return (const char *)_memony->Get(&info);
}

ITable * Unit::FindTable(const s32 index) const {
    tassert(index < _tables.size(), "wtf");
    if (index >= _tables.size()) {
        return NULL;
    }

    return _tables[index];
}

bool Unit::RegAttrModifyCall(const dc::layout & info, const AttributeHandle & handle) {
    ATTR_HANDLE_MAP::iterator itor = _attr_handle_map.find(info._index);
    if (itor == _attr_handle_map.end()) {
        itor = _attr_handle_map.insert(make_pair(info._index, ATTR_HANDLE_LIST())).first;
    }

    itor->second.push_back(handle);
    return true;
}

bool Unit::RegAttrModifyAnyCall(const AttributeHandle & handle) {
    _any_call_list.push_back(handle);
    return true;
}

void Unit::EveryAttrChangedCall(const dc::layout & l, const void * oldvalue, const void * newvalue, const bool sync) {
    ATTR_HANDLE_LIST::const_iterator itor = _any_call_list.begin();
    while (itor != _any_call_list.end()) {
        (*itor).OnModify(g_core, this, l, oldvalue, newvalue, sync);
        itor++;
    }
}

void Unit::AttrChangedCall(const dc::layout & l, const void * oldvalue, const void * newvalue, const bool sync) {
    ATTR_HANDLE_MAP::const_iterator ifind = _attr_handle_map.find(l._index);
    if (ifind != _attr_handle_map.end()) {
        ATTR_HANDLE_LIST::const_iterator itor = ifind->second.begin();
        while (itor != ifind->second.end()) {
            (*itor).OnModify(g_core, this, l, oldvalue, newvalue, sync);
            itor++;
        }
    }
}
