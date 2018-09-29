/* 
 * File:   Model.h
 * Author: alax
 *
 * Created on March 3, 2015, 10:44 AM
 */
#ifndef __Model_h__
#define __Model_h__
#include "Header.h"

class Model : public IModel, public ITimer {
public:
    virtual bool Initialize(ICore * core);
    virtual bool Launch(ICore * core);
    virtual bool Destroy(ICore * core);

    virtual void TraceMemory();
    virtual void CleanMemory();


    virtual void OnStart(ICore * core, const s32 id, const IContext & context, const s64 tick) {}
    virtual void OnTimer(ICore * core, const s32 id, const IContext & context, const s64 tick);
    virtual void OnEnd(ICore * core, const s32 id, const IContext & context, bool nonviolent, const s64 tick) {}
    virtual void OnPause(ICore * core, const s32 id, const IContext & context, const s64 tick) {}
    virtual void OnResume(ICore * core, const s32 id, const IContext & context, const s64 tick) {}

    virtual IUnit * Create(const char * name, const char * file, const s32 line);
    virtual IUnit * CreateUnitByID(const char * name, const s64 id, const char * file, const s32 line);
    virtual IUnit * FindUnit(const s64 id);
    virtual void Recove(IUnit * object, const char * file, const s32 line);

    virtual ITable * FindStaticTable(const char * name);

protected:
    //注册对象创建成功回调
    virtual void RegisterCreatedCall(const char * name, const Handle & handle);
    //注册对象销毁回调
    virtual void RegisterDestoryedCall(const char * name, const Handle & handle);
private:
    const MemonyLayout * QueryUnitLayout(ICore * core, const char * name);
    const MemonyLayout * CreateUnitLayout(ICore * core, const char * name);

    const table_layout_array * QueryTableLayout(ICore * core, const char * name);
    const table_layout_array * CreateTableLayout(ICore * core, const char * name);

    bool InitGloabTable();

private:
    typedef unordered_set<Handle, tools::HashFunc<Handle>> ANY_OBJECT_CREATED_CALL, ANY_OBJECT_DESTORY_CALL;
    static ANY_OBJECT_CREATED_CALL s_any_obj_created_calls;
    static ANY_OBJECT_DESTORY_CALL s_any_obj_destory_calls;

    typedef unordered_map<std::string, ANY_OBJECT_CREATED_CALL> OBJECT_CREATED_CALL, OBJECT_DESTORY_CALL;
    static OBJECT_CREATED_CALL s_obj_created_calls;
    static OBJECT_DESTORY_CALL s_obj_destory_calls;
};

#endif //define __Model_h__
