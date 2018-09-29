#include "Table.h"
#include "Row.h"
#include "Memony.h"

Table::Table(IUnit * host, const MemonyLayout * layout, const dc::layout * key, const char * file, const s32 line)
    : _host(host),
    _layout(layout), 
    _key(key), 
    _file(file), 
    _line(line) {}

Table::~Table() {
    ClearRows();
}

IUnit * Table::GetHost() {
    return nullptr;
}

s32 Table::RowCount() const {
    return _rows.size();
}

void Table::ClearRows() {
    for (s32 i = 0; i < _rows.size(); i++) {
        g_row_pool.Recover(_rows[i]);
    }
    _rows.clear();
    _key_int.clear();
    _key_string.clear();
}

bool Table::AddRowFromBlob(const void * data, const s32 size) {
    Row * row = CREATE_FROM_POOL(g_row_pool, _rows.size(), _layout, __FILE__, __LINE__);
    if (!row->SetData(data, size)) {
        g_row_pool.Recover(row);
        return false;
    }

    if (_key) {
        switch (_key->_type) {
        case dc::type_s64:
            _key_int.insert(make_pair(row->GetDataInt64(_key->_index), _rows.size()));
            break;
        case dc::type_string:
            _key_string.insert(make_pair(row->GetDataString(_key->_index), _rows.size()));
            break;
        default:
            tassert(false, "wtf");
            g_row_pool.Recover(row);
            return false;
        }
    }

    _rows.push_back(row);
    return true;
}

IRow * Table::FindRow(const s64 key) const {
    tassert(_key->_type == type_s64, "wtf");
    INT_KEY_MAP::const_iterator itor = _key_int.find(key);
    if (itor != _key_int.cend()) {
        tassert(_rows.size() > itor->second, "wtf");
        return _rows[itor->second];
    }

    return NULL;
}

IRow * Table::FindRow(const char * key) const {
    tassert(_key->_type == type_string, "wtf");
    STRING_KEY_MAP::const_iterator itor = _key_string.find(key);
    if (itor != _key_string.cend()) {
        tassert(_rows.size() > itor->second, "wtf");
        return _rows[itor->second];
    }

    return NULL;
}

IRow * Table::GetRow(const s32 row) const {
    if (row < _rows.size()) {
        return _rows[row];
    }

    return NULL;
}

IRow * Table::AddRow(const char * file, const s32 line) {
    tassert(_key == NULL, "wtf");
    if (_key != NULL) {
        return NULL;
    }

    Row * row = CREATE_FROM_POOL(g_row_pool, _rows.size(), _layout, file, line);
    _rows.push_back(row);
    return row;
}

IRow * Table::AddRowKeyInt64(const s64 key, const char * file, const s32 line) {
    tassert(_key != NULL && _key->_type == type_s64, "wtf");
    if (_key == NULL || _key->_type != type_s64) {
        return NULL;
    }

    if (_key_int.find(key) != _key_int.end()) {
        tassert(false, "wtf");
        return NULL;
    }

    DebugLog(Core::GetInstance(), "table %llx add row key int %lld, %s:%d", this, key, file, line);

    Row * row = CREATE_FROM_POOL(g_row_pool, _rows.size(), _layout, file, line);
    row->SetDataInt64(_key->_index, key);
    _key_int.insert(make_pair(key, _rows.size()));
    _rows.push_back(row);
    return row;
}

IRow * Table::AddRowKeyString(const char * key, const char * file, const s32 line) {
    tassert(_key != NULL && _key->_type == type_string, "wtf");
    if (_key == NULL || _key->_type != type_s64) {
        return NULL;
    }

    if (_key_string.find(key) != _key_string.end()) {
        tassert(false, "wtf");
        return NULL;
    }

    Row * row = CREATE_FROM_POOL(g_row_pool, _rows.size(), _layout, file, line);
    row->SetDataString(_key->_index, key);
    _key_string.insert(make_pair(key, _rows.size()));
    _rows.push_back(row);

    return row;
}

bool Table::DelRow(const s32 index) {
    tassert(index < _rows.size() && index >= 0, "index over flow");
    if (index >= _rows.size() || index < 0) {
        return false;
    }
    if (_key) {
        switch (_key->_type) {
        case type_s64: {
            s64 key = _rows[index]->GetDataInt64(_key->_index);
            INT_KEY_MAP::iterator itor = _key_int.find(key);
            tassert(itor != _key_int.end() && itor->second == index, "where is key");
            if (itor != _key_int.end() && itor->second == index) {
                for (s32 i = index + 1; i < _rows.size(); i++) {
                    s64 key = _rows[i]->GetDataInt64(_key->_index);
                    INT_KEY_MAP::iterator ifind = _key_int.find(key);
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
            const char * key = _rows[index]->GetDataString(_key->_index);
            STRING_KEY_MAP::iterator itor = _key_string.find(key);
            tassert(itor != _key_string.end() && itor->second == index, "where is key");
            if (itor != _key_string.end() && itor->second == index) {
                for (s32 i = index + 1; i < _rows.size(); i++) {
                    const char * key = (const char *)_rows[i]->GetDataString(_key->_index);
                    STRING_KEY_MAP::iterator ifind = _key_string.find(key);
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

    g_row_pool.Recover(_rows[index]);
    _rows.erase(_rows.begin() + index);
    return true;
}

bool Table::SwapRow(const s32 src, const s32 dst) {
    tassert(src < _rows.size() && dst < _rows.size(), "wtf");
    if (_key) {
        switch (_key->_type) {
        case type_s64: {
            s64 key_src = _rows[src]->GetDataInt64(_key->_index);
            INT_KEY_MAP::iterator itor_src = _key_int.find(key_src);
            tassert(itor_src != _key_int.end() && itor_src->second == src, "wtf");
            if (itor_src == _key_int.end() || itor_src->second != src) {
                return false;
            }
            itor_src->second = dst;

            s64 key_dst = _rows[dst]->GetDataInt64(_key->_index);
            INT_KEY_MAP::iterator itor_dst = _key_int.find(key_dst);
            tassert(itor_dst != _key_int.end() && itor_dst->second == dst, "wtf");
            if (itor_dst == _key_int.end() || itor_dst->second != dst) {
                return false;
            }
            itor_dst->second = src;
            break;
        }
        case type_string:  {
            const char * key_src = _rows[src]->GetDataString(_key->_index);
            STRING_KEY_MAP::iterator itor_src = _key_string.find(key_src);
            tassert(itor_src != _key_string.end() && itor_src->second == src, "wtf");
            if (itor_src == _key_string.end() || itor_src->second != src) {
                return false;
            }
            itor_src->second = dst;

            const char * key_dst = _rows[dst]->GetDataString(_key->_index);
            STRING_KEY_MAP::iterator itor_dst = _key_string.find(key_dst);
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

    _rows[src]->SetRowIndex(dst);
    _rows[dst]->SetRowIndex(src);

    Row * swap = _rows[src];
    _rows[src] = _rows[dst];
    _rows[dst] = swap;
    return true;
}
