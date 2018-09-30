#ifndef __memony_h__
#define __memony_h__

#include "iModel.h"
#include "tools.h"
#include <list>
#include <map>
using namespace std;

class mlayout {
public:
    mlayout() : _memory_size(0) {}

    explicit mlayout(const mlayout * layout) : _layouts(layout->_layouts), _memory_size(layout->_memory_size) {}

    const dc::layout * addlayout(const char * name, const s32 size, const s8 type, const bool visual, const bool share, const bool save, const bool important) {
        const dc::layout info(name, _layouts.size(), _memory_size, type, size, visual, share, save, important);
        _layouts.push_back(info);
        _memory_size += size;
        return &_layouts[_layouts.size() - 1];
    }

    const dc::layout * query(const s32 index) const {
        if (index >= _layouts.size()) {
            tassert(false, "column index over flow");
            return NULL;
        }
        return &_layouts[index];
    }

    inline s32 calcMemorySize() const {
        return _memory_size;
    }
    
    inline void echoTemplate(api::iCore * core) const {
        for (s32 i = 0; i < _layouts.size(); i++) {
            debug(core, "%s, %d", _layouts[i]._name.c_str(), _layouts[i]._index);
        }
    }

    inline const std::vector<dc::layout> & getLayout() const {
        return _layouts;
    }

private:
    s32 _memory_size;
    std::vector<dc::layout> _layouts;
};

class memony {
public:
    bool _in_use;

    memony(const mlayout * layout) : _layout(layout), _in_use(true), _size(layout->calcMemorySize()) {
        _buff = NEW char[_size];
        tools::memery::safeMemset(_buff, _layout->calcMemorySize(), 0, _layout->calcMemorySize());
    }

    ~memony() {
        DEL[] _buff;
    }

    void setLayout(const mlayout * layout) {
        tassert(layout->calcMemorySize() == _size, "wtf");
        if (layout->calcMemorySize() != _size) {
            return;
        }
        _layout = layout;
    }

    void clear() {
        tools::memery::safeMemset(_buff, _size, 0, _size);
    }

    const void * getData() const {
        return _buff;
    }

    const s32 getSize() const {
        return _size;
    }

    bool copy(const void * data, const s32 len) {
        tassert(len == _layout->calcMemorySize(), "wtf");
        if (len != _layout->calcMemorySize()) {
            return false;
        }

        tools::memery::safeMemcpy(_buff, _size, data, len);
        return true;
    }

    const dc::layout * query(const s32 index) {
        return _layout->query(index);
    }

    bool Set(const dc::layout * l, const void * data, const s32 size) const {
        if (_layout->query(l->_index)->_size != l->_size
            || _layout->query(l->_index)->_offset != l->_offset
            || _layout->query(l->_index)->_type != l->_type) {
            tassert(false, " wtf");
            return false;
        }

        if (l->_type == dc::type_string) {
            if (_layout->query(l->_index)->_size < size) {
                tassert(false, "wtf");
                return false;
            }
        }
        else if (_layout->query(l->_index)->_size != size) {
            tassert(false, "wtf");
            return false;
        }


        tools::memery::safeMemcpy(_buff + l->_offset, _size - l->_offset, data, size);
        return true;
    }
    
    const void * get(const dc::layout * l) const {
        if (_layout->query(l->_index)->_size != l->_size
            || _layout->query(l->_index)->_offset != l->_offset
            || _layout->query(l->_index)->_type != l->_type) {
            tassert(false, " wtf");
            return NULL;
        }

        return _buff + l->_offset;
    }

private:
    char * _buff;
    const s32 _size;
    const mlayout * _layout;
};

class memonyPool {
    typedef list<memony*> memony_list;
    typedef map<s32, memony_list> memony_map;
public:
    memony * create(const mlayout * layout) {
        memony * mem = NULL;
        map<s32, list<memony*>>::iterator itor = _memony_map.find(layout->calcMemorySize());
        if (itor != _memony_map.end() && !itor->second.empty()) {
            mem = *(itor->second.begin());
            tassert(!mem->_in_use, "wtf");
            mem->_in_use = true;
            mem->setLayout(layout);
            mem->clear();
            itor->second.erase(itor->second.begin());
        } else {
            mem = NEW memony(layout);
        }

        return mem;
    }

    void recover(memony * memony) {
        tassert(memony->_in_use, "wtf");
        memony_map::iterator itor = _memony_map.find(memony->getSize());
        if (itor == _memony_map.end()) {
            itor = _memony_map.insert(make_pair(memony->getSize(), memony_list())).first;
        }

        memony->_in_use = false;
        itor->second.push_back(memony);
    }

    void clean(api::iCore * core) {
        memony_map::iterator itor = _memony_map.begin();
        while (itor != _memony_map.end()) {
            memony_list::iterator lst_itor = itor->second.begin();
            while (lst_itor != itor->second.end()) {
                if (core) {
                    trace(core, "clean memony size %d", (*lst_itor)->getSize());
                }
                DEL *lst_itor;
                lst_itor++;
            }
            itor++;
        }
        _memony_map.clear();
    }

    static memonyPool & getInstance() {
        static memonyPool s_pool;
        return s_pool;
    }

    ~memonyPool() {
        clean(NULL);
    }

private:
    memonyPool() {}

private:
    memony_map _memony_map;
};

#endif //__memony_h__
