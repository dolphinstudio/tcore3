#ifndef __Memony_h__
#define __Memony_h__

#include "IModel.h"
#include "ICore.h"
#include <list>
#include <map>
using namespace std;

class MemonyLayout {
public:
    MemonyLayout() : _memory_size(0) {}

    explicit MemonyLayout(const MemonyLayout * layout) : _layouts(layout->_layouts), _memory_size(layout->_memory_size) {}

    const dc::layout * AddLayout(const char * name, const s32 size, const s8 type, const bool visual, const bool share, const bool save, const bool important) {
        const dc::layout info(name, _layouts.size(), _memory_size, type, size, visual, share, save, important);
        _layouts.push_back(info);
        _memory_size += size;
        return &_layouts[_layouts.size() - 1];
    }

    const dc::layout * Query(const s32 index) const {
        if (index >= _layouts.size()) {
            tassert(false, "column index over flow");
            return NULL;
        }
        return &_layouts[index];
    }

    inline s32 CalcMemorySize() const {
        return _memory_size;
    }
    
    inline void EchoTemplate(ICore * core) const {
        for (s32 i = 0; i < _layouts.size(); i++) {
            DebugLog(core, "%s, %d", _layouts[i]._name.c_str(), _layouts[i]._index);
        }
    }

    inline const std::vector<dc::layout> & GetLayout() const {
        return _layouts;
    }

private:
    s32 _memory_size;
    std::vector<dc::layout> _layouts;
};

class Memony {
public:
    bool _in_use;

    Memony(const MemonyLayout * layout) : _layout(layout), _in_use(true), _size(layout->CalcMemorySize()) {
        _buff = NEW char[_size];
        tools::SafeMemset(_buff, _layout->CalcMemorySize(), 0, _layout->CalcMemorySize());
    }

    ~Memony() {
        DEL[] _buff;
    }

    void SetLayout(const MemonyLayout * layout) {
        tassert(layout->CalcMemorySize() == _size, "wtf");
        if (layout->CalcMemorySize() != _size) {
            return;
        }
        _layout = layout;
    }

    void Clear() {
        tools::SafeMemset(_buff, _size, 0, _size);
    }

    const void * GetData() const {
        return _buff;
    }

    const s32 GetSize() const {
        return _size;
    }

    bool Copy(const void * data, const s32 len) {
        tassert(len == _layout->CalcMemorySize(), "wtf");
        if (len != _layout->CalcMemorySize()) {
            return false;
        }

        tools::SafeMemcpy(_buff, _size, data, len);
        return true;
    }

    const dc::layout * QueryLayout(const s32 index) {
        return _layout->Query(index);
    }

    bool Set(const dc::layout * l, const void * data, const s32 size) const {
        if (_layout->Query(l->_index)->_size != l->_size
            || _layout->Query(l->_index)->_offset != l->_offset
            || _layout->Query(l->_index)->_type != l->_type) {
            tassert(false, " wtf");
            return false;
        }

        if (l->_type == dc::type_string) {
            if (_layout->Query(l->_index)->_size < size) {
                tassert(false, "wtf");
                return false;
            }
        }
        else if (_layout->Query(l->_index)->_size != size) {
            tassert(false, "wtf");
            return false;
        }


        tools::SafeMemcpy(_buff + l->_offset, _size - l->_offset, data, size);
        return true;
    }
    
    const void * Get(const dc::layout * l) const {
        if (_layout->Query(l->_index)->_size != l->_size
            || _layout->Query(l->_index)->_offset != l->_offset
            || _layout->Query(l->_index)->_type != l->_type) {
            tassert(false, " wtf");
            return NULL;
        }

        return _buff + l->_offset;
    }

private:
    char * _buff;
    const s32 _size;
    const MemonyLayout * _layout;
};

class MemonyPool {
    typedef list<Memony*> MEMONY_LIST;
    typedef map<s32, MEMONY_LIST> MEMONY_MAP;
public:
    Memony * Create(const MemonyLayout * layout) {
        Memony * memony = NULL;
        map<s32, list<Memony*>>::iterator itor = _memony_map.find(layout->CalcMemorySize());
        if (itor != _memony_map.end() && !itor->second.empty()) {
            memony = *(itor->second.begin());
            tassert(!memony->_in_use, "wtf");
            memony->_in_use = true;
            memony->SetLayout(layout);
            memony->Clear();
            itor->second.erase(itor->second.begin());
        } else {
            memony = NEW Memony(layout);
        }

        return memony;
    }

    void Recover(Memony * memony) {
        tassert(memony->_in_use, "wtf");
        MEMONY_MAP::iterator itor = _memony_map.find(memony->GetSize());
        if (itor == _memony_map.end()) {
            itor = _memony_map.insert(make_pair(memony->GetSize(), MEMONY_LIST())).first;
        }

        memony->_in_use = false;
        itor->second.push_back(memony);
    }

    void Clean(ICore * core) {
        MEMONY_MAP::iterator itor = _memony_map.begin();
        while (itor != _memony_map.end()) {
            MEMONY_LIST::iterator lst_itor = itor->second.begin();
            while (lst_itor != itor->second.end()) {
                if (core) {
                    TraceLog(core, "clean memony size %d", (*lst_itor)->GetSize());
                }
                DEL *lst_itor;
                lst_itor++;
            }
            itor++;
        }
        _memony_map.clear();
    }

    static MemonyPool & GetInstance() {
        static MemonyPool s_pool;
        return s_pool;
    }

    ~MemonyPool() {
        Clean(NULL);
    }

private:
    MemonyPool() {}

private:
    MEMONY_MAP _memony_map;
};

#endif //__Memony_h__
