#include "unit.h"
#include "memony.h"
#include "table.h"
using namespace dc;

Unit::Unit(const char * name, const s64 id, const mlayout * layout, const char * file, const s32 line) 
    : _name(name), _id(id), _layout(layout), _file(file), _line(line), _create_tick(tools::time::getMillisecond()) {
    tassert(layout, "wtf");
    _memony = memonyPool::getInstance().create(layout);

    object_table_layouts::iterator itor = g_object_table_layouts.find(name);
    if (itor != g_object_table_layouts.end()) {
        for (s32 i = 0; i < itor->second.size(); i++) {
            table * table = create_from_pool(g_table_pool, this, itor->second[i]._layout, itor->second[i]._key, file, line);
            _tables.push_back(table);
        }
    }
}

Unit::~Unit() {
    for (s32 i = 0; i < _tables.size(); i++) {
        recover_to_pool(g_table_pool, _tables[i]);
    }
    _any_call_list.clear();
    _attr_handle_map.clear();
    memonyPool::getInstance().recover(_memony);
}

const s64 Unit::getid() const {
    return _id;
}

const char * Unit::getname() const {
    return _name.c_str();
}

const std::vector<dc::layout>& Unit::getLayout() const {
    return _layout->getLayout();
}

bool Unit::setAttrInt64(const dc::layout & info, const s64 value, const bool sync) {
    tassert(info._type == type_s64, "wtf");
    s64 old = 0;
    const void * p = _memony->get(&info);
    if (p) {
        old = *(s64 *)p;
    }

    if (value == old) {
        return true;
    }

    if (!_memony->Set(&info, &value, sizeof(value))) {
        return false;
    }

    anyattrModifyed(info, &old, &value, sync);
    attrModifyed(info, &old, &value, sync);
    return true;
}

bool Unit::setAttrFloat(const dc::layout & info, const float value, const bool sync) {
    tassert(info._type == type_float, "wtf");
    float old = 0;
    const void * p = _memony->get(&info);
    if (p) {
        old = *(float *)p;
    }

    if (!_memony->Set(&info, &value, sizeof(value))) {
        return false;
    }

    anyattrModifyed(info, &old, &value, sync);
    attrModifyed(info, &old, &value, sync);
    return true;
}

bool Unit::setAttrBool(const dc::layout & info, const bool value, const bool sync) {
    tassert(info._type == type_bool, "wtf");
    bool old = 0;
    const void * p = _memony->get(&info);
    if (p) {
        old = *(bool *)p;
    }

    if (!_memony->Set(&info, &value, sizeof(value))) {
        return false;
    }

    anyattrModifyed(info, &old, &value, sync);
    attrModifyed(info, &old, &value, sync);
    return true;
}


bool Unit::setAttrString(const dc::layout & info, const char * value, const bool sync) {
    tassert(info._type == type_string, "wtf");
    const s32 len = strlen(value);
    if (len >= info._size) {
        ((char *)value)[len - 1] = 0;
    }

    static std::string s_old;
    const char * p = (const char *)_memony->get(&info);
    if (p) {
        s_old = p;
    } else {
        s_old = "";
    }

    if (!_memony->Set(&info, value, info._size)) {
        return false;
    }

    anyattrModifyed(info, s_old.c_str(), value, sync);
    attrModifyed(info, s_old.c_str(), value, sync);
    return true;
}

s64 Unit::getAttrInt64(const dc::layout & info) const {
    tassert(info._type == type_s64, "wtf");
    return *(s64*)_memony->get(&info);
}

float Unit::getAttrFloat(const dc::layout & info) const {
    tassert(info._type == type_float, "wtf");
    return *(float*)_memony->get(&info);
}

bool Unit::getAttrBool(const dc::layout & info) const {
    tassert(info._type == type_bool, "wtf");
    return *(bool*)_memony->get(&info);
}

const char * Unit::getAttrString(const dc::layout & info) const {
    tassert(info._type == type_string, "wtf");
    return (const char *)_memony->get(&info);
}

iTable * Unit::findTable(const s32 index) const {
    tassert(index < _tables.size(), "wtf");
    if (index >= _tables.size()) {
        return NULL;
    }

    return _tables[index];
}

bool Unit::registerModifyer(const dc::layout & info, const modifyer & handle) {
    modify_map::iterator itor = _attr_handle_map.find(info._index);
    if (itor == _attr_handle_map.end()) {
        itor = _attr_handle_map.insert(make_pair(info._index, modifyer_list())).first;
    }

    itor->second.push_back(handle);
    return true;
}

bool Unit::registerAllModifyer(const modifyer & handle) {
    _any_call_list.push_back(handle);
    return true;
}

void Unit::anyattrModifyed(const dc::layout & l, const void * oldvalue, const void * newvalue, const bool sync) {
    modifyer_list::const_iterator itor = _any_call_list.begin();
    while (itor != _any_call_list.end()) {
        (*itor).onModify(g_core, this, l, oldvalue, newvalue, sync);
        itor++;
    }
}

void Unit::attrModifyed(const dc::layout & l, const void * oldvalue, const void * newvalue, const bool sync) {
    modify_map::const_iterator ifind = _attr_handle_map.find(l._index);
    if (ifind != _attr_handle_map.end()) {
        modifyer_list::const_iterator itor = ifind->second.begin();
        while (itor != ifind->second.end()) {
            (*itor).onModify(g_core, this, l, oldvalue, newvalue, sync);
            itor++;
        }
    }
}
