#ifndef __Unit_h__
#define __Unit_h__

#include "Header.h"

class Memony;
class MemonyLayout;
class Table;

class Unit : public IUnit {
    typedef list<AttributeHandle> ATTR_HANDLE_LIST;
    typedef map<s16, ATTR_HANDLE_LIST> ATTR_HANDLE_MAP;
public:
    const std::string _file;
    const s32 _line;
    const std::string _name;
    const s64 _create_tick;

    Unit(const char * name, const s64 id, const MemonyLayout * layout, const char * file, const s32 line);
    virtual ~Unit();

    virtual const s64 GetID() const;
    virtual const char * GetName() const;
    virtual const std::vector<dc::layout> & GetLayout() const;

    virtual bool SetAttrInt64(const dc::layout & info, const s64 value, const bool sync);
    virtual bool SetAttrFloat(const dc::layout & info, const float value, const bool sync);
    virtual bool SetAttrBool(const dc::layout & info, const bool value, const bool sync);
    virtual bool SetAttrString(const dc::layout & info, const char * value, const bool sync);

    virtual s64 GetAttrInt64(const dc::layout & info) const;
    virtual float GetAttrFloat(const dc::layout & info) const;
    virtual bool GetAttrBool(const dc::layout & info) const;
    virtual const char * GetAttrString(const dc::layout & info) const;

    //通过名称获取表
    virtual ITable * FindTable(const s32 index) const;

protected:
    virtual bool RegAttrModifyCall(const dc::layout &, const AttributeHandle &);
    virtual bool RegAttrModifyAnyCall(const AttributeHandle &);

    virtual void EveryAttrChangedCall(const dc::layout & l, const void * oldvalue, const void * newvalue, const bool sync);
    virtual void AttrChangedCall(const dc::layout & l, const void * oldvalue, const void * newvalue, const bool sync);

private:
    const s64 _id;
    const MemonyLayout * _layout;
    Memony * _memony;
    tables _tables;

    ATTR_HANDLE_LIST _any_call_list;
    ATTR_HANDLE_MAP _attr_handle_map;
};

#endif //__Unit_h__
