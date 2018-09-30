#include "table.h"
#include "row.h"
#include "memony.h"

table::table(iUnit * host, const mlayout * layout, const dc::layout * key, const char * file, const s32 line)
    : _host(host),
    _layout(layout), 
    _key(key), 
    _file(file), 
    _line(line) {}

table::~table() {
    clear();
}

iUnit * table::getHoster() {
    return nullptr;
}

s32 table::rowCount() const {
    return _rows.size();
}

void table::clear() {
    for (s32 i = 0; i < _rows.size(); i++) {
        recover_to_pool(g_row_pool, _rows[i]);
    }
    _rows.clear();
    _key_int.clear();
    _key_string.clear();
}

bool table::addRowFromBlob(const void * data, const s32 size) {
    row * row = create_from_pool(g_row_pool, _rows.size(), _layout, __FILE__, __LINE__);
    if (!row->setData(data, size)) {
        recover_to_pool(g_row_pool, row);
        return false;
    }

    if (_key) {
        switch (_key->_type) {
        case dc::type_s64:
            _key_int.insert(make_pair(row->getInt64(_key->_index), _rows.size()));
            break;
        case dc::type_string:
            _key_string.insert(make_pair(row->getString(_key->_index), _rows.size()));
            break;
        default:
            tassert(false, "wtf");
            recover_to_pool(g_row_pool, row);
            return false;
        }
    }

    _rows.push_back(row);
    return true;
}

iRow * table::findRow(const s64 key) const {
    tassert(_key->_type == type_s64, "wtf");
    int_key_map::const_iterator itor = _key_int.find(key);
    if (itor != _key_int.cend()) {
        tassert(_rows.size() > itor->second, "wtf");
        return _rows[itor->second];
    }

    return NULL;
}

iRow * table::findRow(const char * key) const {
    tassert(_key->_type == type_string, "wtf");
    string_key_map::const_iterator itor = _key_string.find(key);
    if (itor != _key_string.cend()) {
        tassert(_rows.size() > itor->second, "wtf");
        return _rows[itor->second];
    }

    return NULL;
}

iRow * table::getRow(const s32 row) const {
    if (row < _rows.size()) {
        return _rows[row];
    }

    return NULL;
}

iRow * table::addRow(const char * file, const s32 line) {
    tassert(_key == NULL, "wtf");
    if (_key != NULL) {
        return NULL;
    }

    row * row = create_from_pool(g_row_pool, _rows.size(), _layout, file, line);
    _rows.push_back(row);
    return row;
}

iRow * table::addRowKeyInt64(const s64 key, const char * file, const s32 line) {
    tassert(_key != NULL && _key->_type == type_s64, "wtf");
    if (_key == NULL || _key->_type != type_s64) {
        return NULL;
    }

    if (_key_int.find(key) != _key_int.end()) {
        tassert(false, "wtf");
        return NULL;
    }

    debug(Core::GetInstance(), "table %llx add row key int %lld, %s:%d", this, key, file, line);

    row * row = create_from_pool(g_row_pool, _rows.size(), _layout, file, line);
    row->setInt64(_key->_index, key);
    _key_int.insert(make_pair(key, _rows.size()));
    _rows.push_back(row);
    return row;
}

iRow * table::addRowKeyString(const char * key, const char * file, const s32 line) {
    tassert(_key != NULL && _key->_type == type_string, "wtf");
    if (_key == NULL || _key->_type != type_s64) {
        return NULL;
    }

    if (_key_string.find(key) != _key_string.end()) {
        tassert(false, "wtf");
        return NULL;
    }

    row * row = create_from_pool(g_row_pool, _rows.size(), _layout, file, line);
    row->setString(_key->_index, key);
    _key_string.insert(make_pair(key, _rows.size()));
    _rows.push_back(row);

    return row;
}

bool table::delRow(const s32 index) {
    tassert(index < _rows.size() && index >= 0, "index over flow");
    if (index >= _rows.size() || index < 0) {
        return false;
    }
    if (_key) {
        switch (_key->_type) {
        case type_s64: {
            s64 key = _rows[index]->getInt64(_key->_index);
            int_key_map::iterator itor = _key_int.find(key);
            tassert(itor != _key_int.end() && itor->second == index, "where is key");
            if (itor != _key_int.end() && itor->second == index) {
                for (s32 i = index + 1; i < _rows.size(); i++) {
                    s64 key = _rows[i]->getInt64(_key->_index);
                    int_key_map::iterator ifind = _key_int.find(key);
                    tassert(ifind != _key_int.end() && ifind->second == i, "where is key");
                    if (ifind != _key_int.end() && ifind->second == i) {
                        s32 oldIndex = ifind->second;
                        ifind->second -= 1;
                        _rows[i]->_row--;
                    }
                }
                _key_int.erase(itor);
            } else {
                return false;
            }
            break;
        }
        case type_string: {
            const char * key = _rows[index]->getString(_key->_index);
            string_key_map::iterator itor = _key_string.find(key);
            tassert(itor != _key_string.end() && itor->second == index, "where is key");
            if (itor != _key_string.end() && itor->second == index) {
                for (s32 i = index + 1; i < _rows.size(); i++) {
                    const char * key = (const char *)_rows[i]->getString(_key->_index);
                    string_key_map::iterator ifind = _key_string.find(key);
                    tassert(ifind != _key_string.end() && ifind->second == index, "where is key");
                    if (ifind != _key_string.end() && ifind->second == index) {
                        ifind->second -= 1;
                        _rows[i]->_row--;
                    }
                }
                _key_string.erase(itor);
            } else {
                return false;
            }
            break;
        }
        default:
            tassert(false, "wtf");
            break;
        }
    } else {
        for (s32 i = index + 1; i < _rows.size(); i++) {
            _rows[i]->_row--;
        }
    }

    recover_to_pool(g_row_pool, _rows[index]);
    _rows.erase(_rows.begin() + index);
    return true;
}

bool table::swapRow(const s32 src, const s32 dst) {
    tassert(src < _rows.size() && dst < _rows.size(), "wtf");
    if (_key) {
        switch (_key->_type) {
        case type_s64: {
            s64 key_src = _rows[src]->getInt64(_key->_index);
            int_key_map::iterator itor_src = _key_int.find(key_src);
            tassert(itor_src != _key_int.end() && itor_src->second == src, "wtf");
            if (itor_src == _key_int.end() || itor_src->second != src) {
                return false;
            }
            itor_src->second = dst;

            s64 key_dst = _rows[dst]->getInt64(_key->_index);
            int_key_map::iterator itor_dst = _key_int.find(key_dst);
            tassert(itor_dst != _key_int.end() && itor_dst->second == dst, "wtf");
            if (itor_dst == _key_int.end() || itor_dst->second != dst) {
                return false;
            }
            itor_dst->second = src;
            break;
        }
        case type_string:  {
            const char * key_src = _rows[src]->getString(_key->_index);
            string_key_map::iterator itor_src = _key_string.find(key_src);
            tassert(itor_src != _key_string.end() && itor_src->second == src, "wtf");
            if (itor_src == _key_string.end() || itor_src->second != src) {
                return false;
            }
            itor_src->second = dst;

            const char * key_dst = _rows[dst]->getString(_key->_index);
            string_key_map::iterator itor_dst = _key_string.find(key_dst);
            tassert(itor_dst != _key_string.end() && itor_dst->second == dst, "wtf");
            if (itor_dst == _key_string.end() || itor_dst->second != dst) {
                return false;
            }
            itor_dst->second = src;
            break;
        }
        default:
            tassert(false, "wtf");
            break;
        }
    }

    _rows[src]->setIndex(dst);
    _rows[dst]->setIndex(src);

    row * swap = _rows[src];
    _rows[src] = _rows[dst];
    _rows[dst] = swap;
    return true;
}
