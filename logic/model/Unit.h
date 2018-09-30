#ifndef __unit_h__
#define __unit_h__

#include "header.h"

class memony;
class mlayout;
class table;

class Unit : public iUnit {
    typedef list<modifyer> modifyer_list;
    typedef map<s16, modifyer_list> modify_map;
public:
    const std::string _file;
    const s32 _line;
    const std::string _name;
    const s64 _create_tick;

    Unit(const char * name, const s64 id, const mlayout * layout, const char * file, const s32 line);
    virtual ~Unit();

    virtual const s64 getid() const;
    virtual const char * getname() const;
    virtual const std::vector<dc::layout> & getLayout() const;

    virtual bool setAttrInt64(const dc::layout & info, const s64 value, const bool sync);
    virtual bool setAttrFloat(const dc::layout & info, const float value, const bool sync);
    virtual bool setAttrBool(const dc::layout & info, const bool value, const bool sync);
    virtual bool setAttrString(const dc::layout & info, const char * value, const bool sync);

    virtual s64 getAttrInt64(const dc::layout & info) const;
    virtual float getAttrFloat(const dc::layout & info) const;
    virtual bool getAttrBool(const dc::layout & info) const;
    virtual const char * getAttrString(const dc::layout & info) const;

    //通过名称获取表
    virtual iTable * findTable(const s32 index) const;

protected:
    virtual bool registerModifyer(const dc::layout &, const modifyer &);
    virtual bool registerAllModifyer(const modifyer &);

    virtual void anyattrModifyed(const dc::layout & l, const void * oldvalue, const void * newvalue, const bool sync);
    virtual void attrModifyed(const dc::layout & l, const void * oldvalue, const void * newvalue, const bool sync);

private:
    const s64 _id;
    const mlayout * _layout;
    memony * _memony;
    tables _tables;

    modifyer_list _any_call_list;
    modify_map _attr_handle_map;
};

#endif //__unit_h__
