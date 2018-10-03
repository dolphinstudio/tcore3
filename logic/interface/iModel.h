/* 
 * File:   IHarbor.h
 * Author: ooeyusea
 *
 * Created on March 3, 2015, 10:46 AM
 */

#ifndef __iModel_h__
#define __iModel_h__

#include "api.h"
#include "attribute.h"
#include <vector>

using namespace tcore;

#define INVALID_OBJECT_ID 0

class iUnit;

class iRow {
public:
    virtual ~iRow() {}
    
    virtual s32 getIndex() const = 0;

    virtual s64 getInt64(const s32 column) const = 0;
    virtual float getFloat(const s32 column) const = 0;
    virtual bool getBool(const s32 column) const = 0;
    virtual const char * getString(const s32 column) const = 0;

    virtual bool setInt64(const s32 column, const s64 value) = 0;
    virtual bool setFloat(const s32 column, const float value) = 0;
    virtual bool setBool(const s32 column, const bool value) = 0;
    virtual bool setString(const s32 column, const char * value) = 0;
};

class iTable {
public:
    virtual ~iTable() {}

    virtual iUnit * getHoster() = 0;

    virtual s32 rowCount() const = 0;
    virtual void clear() = 0;

    virtual iRow * findRow(const s64 key) const = 0;
    virtual iRow * findRow(const char * key) const = 0;
    virtual iRow * getRow(const s32 row) const = 0;

    virtual iRow * addRow(const char * file, const s32 line) = 0;
    virtual iRow * addRowKeyInt64(const s64 key, const char * file, const s32 line) = 0;
    virtual iRow * addRowKeyString(const char * key, const char * file, const s32 line) = 0;

    virtual bool delRow(const s32 index) = 0;
    virtual bool swapRow(const s32 src, const s32 dst) = 0;
};

class iUnit {
protected:
    class modifyer {
        typedef void(*fun)(const void *, api::iCore *, iUnit *, const dc::layout &, const void *, const void *, const bool);
    public:
        virtual ~modifyer() {}
        modifyer(const fun f, const void * call, const char * debug) : _fun(f), _call(call), _debug(debug) {}
        const std::string _debug;
        const fun _fun;
        const void * _call;

        void onModify(api::iCore * core, iUnit * object, const dc::layout & info, const void * oldvalue, const void * newvalue, const bool sync) const {
            _fun(_call, core, object, info, oldvalue, newvalue, sync);
        }
    };

    template<typename t>
    class tModifyer {
        typedef void(*fun)(api::iCore *, iUnit *, const dc::layout &, const t &, const t &, const bool);
    public:
        static void onModify(const void * f, api::iCore * core, iUnit * object, const dc::layout & info, const void * oldvalue, const void * newvalue, const bool sync) {
            (*(fun)f)(core, object, info, *(t*)oldvalue, *(t*)newvalue, sync);
        }
    };

    class cModifyer {
        typedef void(*fun)(api::iCore *, iUnit *, const dc::layout &, const void *, const void *, const bool);
    public:
        static void onModify(const void * f, api::iCore * core, iUnit * object, const dc::layout & info, const void * oldvalue, const void * newvalue, const bool sync) {
            (*(fun)f)(core, object, info, oldvalue, newvalue, sync);
        }
    };

public:
    virtual ~iUnit() {}

    virtual const s64 getid() const = 0;
    virtual const char * getname() const = 0;
    virtual const std::vector<dc::layout> & getLayout() const = 0;

    virtual bool setAttrInt64(const dc::layout & info, const s64 value, const bool sync = false) = 0;
    virtual bool setAttrFloat(const dc::layout & info, const float value, const bool sync = false) = 0;
    virtual bool setAttrBool(const dc::layout & info, const bool value, const bool sync = false) = 0;
    virtual bool setAttrString(const dc::layout & info, const char * value, const bool sync = false) = 0;

    virtual bool setAttrInt64ByIndex(const s32 index, const s64 value, const bool sync = false) {
        const dc::layout & info = getLayout()[index];
        return setAttrInt64(info, value, sync);
    }

    virtual bool setAttrFloatByIndex(const s32 index, const float value, const bool sync = false) {
        const dc::layout & info = getLayout()[index];
        return setAttrFloat(info, value, sync);
    }

    virtual bool setAttrBoolByIndex(const s32 index, const bool value, const bool sync = false) {
        const dc::layout & info = getLayout()[index];
        return setAttrBool(info, value, sync);
    }

    virtual bool setAttrStringByIndex(const s32 index, const char * value, const bool sync = false) {
        const dc::layout & info = getLayout()[index];
        return setAttrString(info, value, sync);
    }

    virtual s64 getAttrInt64(const dc::layout & info) const = 0;
    virtual float getAttrFloat(const dc::layout & info) const = 0;
    virtual bool getAttrBool(const dc::layout & info) const = 0;
    virtual const char * getAttrString(const dc::layout & info) const = 0;

    virtual s64 getAttrInt64ByIndex(const s32 index) const {
        const dc::layout & info = getLayout()[index];
        return getAttrInt64(info);
    }

    virtual float getAttrFloatByIndex(const s32 index) const {
        const dc::layout & info = getLayout()[index];
        return getAttrFloat(info);
    }

    virtual bool getAttrBoolByIndex(const s32 index) const {
        const dc::layout & info = getLayout()[index];
        return getAttrBool(info);
    }

    virtual const char * getAttrStringByIndex(const s32 index) const {
        const dc::layout & info = getLayout()[index];
        return getAttrString(info);
    }

    template<typename t>
    bool tRegisterModifyer(const dc::layout & info, void(*fun)(api::iCore *, iUnit *, const dc::layout &, const t &, const t &, const bool), const char * debug) {
        modifyer handle(tModifyer<t>::onModify, (void *)fun, debug);
        return registerModifyer(info, handle);
    }

    virtual bool registerAnyModify(void(*fun)(api::iCore *, iUnit *, const dc::layout &, const void *, const void *, const bool), const char * debug) {
        modifyer handle(cModifyer::onModify, (void *)fun, debug);
        return registerAllModifyer(handle);
    }

    virtual iTable * findTable(const s32 index) const = 0;

protected:
    virtual bool registerModifyer(const dc::layout &, const modifyer &) = 0;
    virtual bool registerAllModifyer(const modifyer &) = 0;
};

class iModel : public iModule {
protected:
    class handler {
        typedef void(*call)(api::iCore *, iUnit *);
    public:
        handler(const call fun, const char * debug) : _fun(fun), _debug(debug) {}

        const call _fun;
        const std::string _debug;
        
        bool operator==(const handler & target) const {
            return _fun == target._fun;
        }

        operator size_t() const {
            return (s64)_fun;
        }
    };
public:
    virtual ~iModel() {}
    virtual void traceMemory() = 0;
    virtual void cleanMemory() = 0;

    virtual iUnit * create(const char * name, const char * file, const s32 line) = 0;
    virtual iUnit * createUnitByID(const char * name, const s64 id, const char * file, const s32 line) = 0;
    virtual iUnit * findUnit(const s64 id) = 0;
    virtual void recove(iUnit * object, const char * file, const s32 line) = 0;

    //通过名称获取静态表
    virtual iTable * findStaticTable(const char * name) = 0;

    virtual void registerAnyCreater(void(*call)(api::iCore *, iUnit *), const char * name) {
        registerCreater(NULL, handler(call, name));
    }

    virtual void registerAnyDestoryedCall(void(*call)(api::iCore *, iUnit *), const char * name) {
        registerDestoryer(NULL, handler(call, name));
    }

    virtual void registerCreatedCall(const char * object_name, void(*call)(api::iCore *, iUnit *), const char * name) {
        registerCreater(object_name, handler(call, name));
    }

    virtual void registerDestoryedCall(const char * object_name, void(*call)(api::iCore *, iUnit *), const char * name) {
        registerDestoryer(object_name, handler(call, name));
    }

protected:
    //注册对象创建成功回调
    virtual void registerCreater(const char * objec_name, const handler &) = 0;

    //注册对象销毁回调
    virtual void registerDestoryer(const char * objec_name, const handler &) = 0;
};

#define reg_unit_create_call(model, name, fun) model->registerCreater(name, fun, #fun);
#define reg_unit_release_call(model, name, fun) model->registerDestoryer(name, fun, #fun);

#define reg_anyunit_created_call(model, fun) model->registerCreater(NULL, fun, #fun);
#define reg_anyunit_release_call(model, fun) model->registerDestoryer(NULL, fun, #fun);

#define del_row(table, row) {\
    tassert(table && table->rowCount() > 0 && row->getIndex() >= 0, "table or row index is invalid");\
    if (row->getIndex() >= 0) { \
        s32 end = table->rowCount() - 1; \
        if (end != row->getIndex()) \
            table->swapRow(row->getIndex(), end); \
        table->delRow(end); \
    } \
}

#define reg_anyattr_call(object, fun) object->registerAllModifyer(fun, #fun)
#define reg_attr_call(object, info, fun) object->tRegisterModifyer(info, fun, #fun)

#define create_unit(mgr, name)  mgr->create(name, __FILE__, __LINE__)
#define create_unit_with_id(mgr, name, id) mgr->createUnitByID(name, id, __FILE__, __LINE__)
#define release_unit(mgr, obj) mgr->recove(obj, __FILE__, __LINE__)

#define add_row(tab) tab->addRow(__FILE__, __LINE__)
#define add_row_key_int(tab, key) tab->addRowKeyInt64(key, __FILE__, __LINE__)
#define add_row_key_string(tab, key) tab->addRowKeyString(key, __FILE__, __LINE__)

#endif //define __iModel_h__
