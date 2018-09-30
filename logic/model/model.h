/* 
 * File:   Model.h
 * Author: alax
 *
 * Created on March 3, 2015, 10:44 AM
 */
#ifndef __model_h__
#define __model_h__
#include "header.h"

class model : public iModel, public api::iTimer {
public:
    virtual bool initialize(api::iCore * core);
    virtual bool launch(api::iCore * core);
    virtual bool destroy(api::iCore * core);

    virtual void traceMemory();
    virtual void cleanMemory();


    virtual void onStart(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}
    virtual void onTimer(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick);
    virtual void onEnd(api::iCore * core, const s32 id, const api::iContext & context, bool nonviolent, const s64 tick) {}
    virtual void onPause(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}
    virtual void onResume(api::iCore * core, const s32 id, const api::iContext & context, const s64 tick) {}

    virtual iUnit * create(const char * name, const char * file, const s32 line);
    virtual iUnit * createUnitByID(const char * name, const s64 id, const char * file, const s32 line);
    virtual iUnit * findUnit(const s64 id);
    virtual void recove(iUnit * object, const char * file, const s32 line);

    virtual iTable * findStaticTable(const char * name);

protected:
    //注册对象创建成功回调
    virtual void registerCreater(const char * name, const handler & handle);
    //注册对象销毁回调
    virtual void registerDestoryer(const char * name, const handler & handle);
private:
    const mlayout * queryUnitLayout(api::iCore * core, const char * name);
    const mlayout * createUnitLayout(api::iCore * core, const char * name);

    const table_layout_array * queryTableLayout(api::iCore * core, const char * name);
    const table_layout_array * qreateTableLayout(api::iCore * core, const char * name);

    bool initGloabTable();

private:
    typedef unordered_set<handler, tools::oHashFunction<handler>> any_unit_creater, any_unit_destoryer;
    static any_unit_creater s_any_obj_created_calls;
    static any_unit_destoryer s_any_obj_destory_calls;

    typedef unordered_map<std::string, any_unit_creater> unit_creater, object_destoryer;
    static unit_creater s_obj_created_calls;
    static object_destoryer s_obj_destory_calls;
};

#endif //define __model_h__
