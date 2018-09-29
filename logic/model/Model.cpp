#include "Model.h"
#include "Header.h"
#include "Unit.h"
#include "Table.h"

ICore * g_core = nullptr;
IEvent * g_event = nullptr;
IIDManager * g_idmanager = nullptr;

table_map g_table_map;
unit_map g_unit_map;
memonylayout_map g_unit_layout_map;
memonylayout_map g_table_layout_map;
memonylayout_map g_static_table_layout_map;
object_table_layouts g_object_table_layouts;

TPool<Unit, 1, 4096> g_object_pool;
TPool<Row, 1, 512> g_row_pool;
TPool<Table, 1, 4096> g_table_pool;

string_map g_path_map;

Model::ANY_OBJECT_CREATED_CALL Model::s_any_obj_created_calls;
Model::ANY_OBJECT_DESTORY_CALL Model::s_any_obj_destory_calls;

Model::OBJECT_CREATED_CALL Model::s_obj_created_calls;
Model::OBJECT_DESTORY_CALL Model::s_obj_destory_calls;

bool Model::Initialize(ICore * core) {
    g_core = core;
    tassert(g_core, "where is core?");
    return InitGloabTable();
}

bool Model::InitGloabTable() {
    char path[512] = { 0 };
    SafeSprintf(path, sizeof(path), "%s/%s/model/global.xml", tools::GetAppPath(), g_core->GetEnvpath());

    TiXmlDocument xml;
    if (!xml.LoadFile(path)) {
        tassert(false, "prop xml file %s load file", path);
        return false;
    }

    TiXmlElement * root = xml.RootElement();
    tassert(root, "wtf");

    TiXmlElement * table = root->FirstChildElement("table");
    while (table) {
        const char * tablename = table->Attribute("name");
        tassert(tablename, "wtf");
        if (g_table_layout_map[tablename] != nullptr) {
            tassert(false, "table already exists");
            continue;
        }

        TiXmlElement * column = table->FirstChildElement("column");
        MemonyLayout * layout = NEW MemonyLayout;
        s32 index = 0xffff;
        s32 i = 0;
        while (column) {
            const char * name = column->Attribute("name");
            const char * type = column->Attribute("type");
            tassert(name && type, "wtf");

            bool key = (column->Attribute("key")) ? (tools::StringAsBool(column->Attribute("key"))) : false;
            bool visual = (column->Attribute("visual")) ? (tools::StringAsBool(column->Attribute("visual"))) : false;
            bool share = (column->Attribute("share")) ? (tools::StringAsBool(column->Attribute("share"))) : false;
            bool save = (column->Attribute("save")) ? (tools::StringAsBool(column->Attribute("save"))) : false;
            bool important = (column->Attribute("important")) ? (tools::StringAsBool(column->Attribute("important"))) : false;

            s32 size, mask;
            if (!strcmp(type, "s64")) {
                size = sizeof(s64);
                mask = type_s64;
            } else if (!strcmp(type, "float")) {
                size = sizeof(float);
                mask = type_float;
            } else if (!strcmp(type, "bool")) {
                size = sizeof(bool);
                mask = type_bool;
            } else if (!strcmp(type, "string")) {
                size = tools::StringAsInt(column->Attribute("size"));
                mask = type_string;
            } else {
                tassert(false, "what's this");
                return false;
            }

            if (key) {
                tassert(0xffff == index, "wtf");
                (0xffff == index) ? (index = i) : (NULL);
            }
            layout->AddLayout(name, size, mask, visual, share, save, important);
            column = column->NextSiblingElement("column");
            i++;
        }

        if (index != 0xffff) {
            g_table_map.insert(make_pair(tablename, CREATE_FROM_POOL(g_table_pool, (IUnit *)nullptr, layout, layout->Query(index), __FILE__, __LINE__)));
        } else {
            g_table_map.insert(make_pair(tablename, CREATE_FROM_POOL(g_table_pool, (IUnit *)nullptr, layout, (const dc::layout *)nullptr, __FILE__, __LINE__)));
        }
        table = table->NextSiblingElement("table");

        g_table_layout_map.insert(make_pair(tablename, layout));
    }

    return true;
}

bool Model::Launch(ICore * core) {
    g_event = (IEvent *)core->FindModule("Event");
    g_idmanager = (IIDManager *)core->FindModule("IDManager");

    char path[512] = {0};
    SafeSprintf(path, sizeof(path), "%s/%s/model/object", tools::GetAppPath(), core->GetEnvpath());
    tools::VECTOR_PATH oPaths;
    tools::VECTOR_NAME oNames;
    s32 nCount = 0;

    if (tools::ListFileInDircetory(path, ".xml", oPaths, oNames, nCount)) {
        tassert(oPaths.size() == nCount && oNames.size() == nCount, "wtf");
        for (s32 i=0; i<nCount; i++) {
            if (g_path_map.end() != g_path_map.find(oNames[i].c_str())) {
                tassert(false, "prop xml name repeated");
                return false;
            }

            g_path_map.insert(make_pair(oNames[i].c_str(), oPaths[i].c_str()));
        }

        for (s32 i = 0; i < nCount; i++) {
            QueryUnitLayout(core, oNames[i].c_str());
        }

        for (s32 i = 0; i < nCount; i++) {
            QueryTableLayout(core, oNames[i].c_str());
        }
    }

    start_timer(core, this, 0, HOUR, forever, HOUR, 0);
    return true;
}

bool Model::Destroy(ICore * core) {
    {
        unit_map::const_iterator itor = g_unit_map.begin();
        while (itor != g_unit_map.end()) {
            TraceLog(g_core, "%s %d object leak", itor->second->_file.c_str(), itor->second->_line);
            g_object_pool.Recover(itor->second);
            itor++;
        }
    }

    {
        memonylayout_map::const_iterator itor = g_unit_layout_map.begin();
        while (itor != g_unit_layout_map.end()) {
            DEL itor->second;
            itor++;
        }
    }
    {
        memonylayout_map::const_iterator itor = g_table_layout_map.begin();
        while (itor != g_table_layout_map.end()) {
            DEL itor->second;
            itor++;
        }
    }
    {
        memonylayout_map::const_iterator itor = g_static_table_layout_map.begin();
        while (itor != g_static_table_layout_map.end()) {
            DEL itor->second;
            itor++;
        }
    }
    {
        table_map::const_iterator itor = g_table_map.begin();
        while (itor != g_table_map.end()) {
            g_table_pool.Recover(itor->second);
            itor++;
        }
    }

    return false;
}

const MemonyLayout * Model::QueryUnitLayout(ICore * core, const char * name) {
    memonylayout_map::const_iterator itor = g_unit_layout_map.find(name);
    if (itor != g_unit_layout_map.end()) {
        return itor->second;
    }

    return CreateUnitLayout(core, name);
}

const MemonyLayout * Model::CreateUnitLayout(ICore * core, const char * name) {
    const char * path = nullptr; {
        string_map::const_iterator itor = g_path_map.find(name);
        if (itor == g_path_map.cend()) {
            tassert(false, "wtf");
            return nullptr;
        }
        path = itor->second.c_str();
    }
    MemonyLayout * layout = nullptr;
    TiXmlDocument xmlDocument;
    if (!xmlDocument.LoadFile(path)) {
        tassert(false, "prop xml file %s load file", path);
        return nullptr;
    }

    TiXmlElement * root = xmlDocument.RootElement();
    tassert(root, "%s format error", path);
    const char * parent = root->Attribute("parent");
    if (parent != nullptr) {
        const MemonyLayout * parent_layout = QueryUnitLayout(core, parent);
        tassert(parent_layout, "where is parent %s xml", parent);
        if (nullptr == parent_layout) {
            return nullptr;
        }

        layout = NEW MemonyLayout(parent_layout);
    }
    else {
        layout = NEW MemonyLayout;
    }

    TiXmlElement * attr = root->FirstChildElement("attribute");
    while (attr) {
        const char * name = attr->Attribute("name");
        s32 size, mask;
        const char * type = attr->Attribute("type");
        if (!strcmp(type, "s64")) {
            size = sizeof(s64);
            mask = type_s64;
        } else if (!strcmp(type, "float")) {
            size = sizeof(float);
            mask = type_float;
        } else if (!strcmp(type, "bool")) {
            size = sizeof(bool);
            mask = type_bool;
        } else if (!strcmp(type, "string")) {
            size = tools::StringAsInt(attr->Attribute("size"));
            mask = type_string;
        } else {
            tassert(false, "what's this");
            return nullptr;
        }

        bool visual = tools::StringAsBool(attr->Attribute("visual"));
        bool share = tools::StringAsBool(attr->Attribute("share"));
        bool save = tools::StringAsBool(attr->Attribute("save"));
        bool important = tools::StringAsBool(attr->Attribute("important"));

        layout->AddLayout(name, size, mask, visual, share, save, important);

        attr = attr->NextSiblingElement("attribute");
    }

    g_unit_layout_map.insert(make_pair(name, layout));
    DebugLog(g_core, "============== %s prop enum start =================", name);
    layout->EchoTemplate(g_core);
    DebugLog(g_core, "============== %s prop enum end =================", name);

    return layout;
}

const table_layout_array * Model::QueryTableLayout(ICore * core, const char * name) {
    object_table_layouts::iterator itor = g_object_table_layouts.find(name);
    if (itor != g_object_table_layouts.end()) {
        return &itor->second;
    }

    return CreateTableLayout(core, name);
}

const table_layout_array * Model::CreateTableLayout(ICore * core, const char * name) {
    const char * path = nullptr; {
        string_map::const_iterator itor = g_path_map.find(name);
        if (itor == g_path_map.cend()) {
            tassert(false, "wtf");
            return nullptr;
        }
        path = itor->second.c_str();
    }
    
    table_layout_array layouts;
    TiXmlDocument xmlDocument;
    if (!xmlDocument.LoadFile(path)) {
        tassert(false, "prop xml file %s load file", path);
        return nullptr;
    }

    TiXmlElement * root = xmlDocument.RootElement();
    tassert(root, "%s format error", path);
    const char * parent = root->Attribute("parent");
    if (parent != nullptr) {
        const table_layout_array * parent_layouts = QueryTableLayout(core, parent);
        tassert(parent_layouts, "where is parent %s xml", parent);
        if (nullptr == parent_layouts) {
            return nullptr;
        }

        layouts.insert(layouts.end(), parent_layouts->begin(), parent_layouts->end());
    }

    TiXmlElement * table = root->FirstChildElement("table");
    while (table) {
        const char * tablename = table->Attribute("name");
        tassert(tablename, "wtf");
        if (g_table_layout_map[tablename] != nullptr) {
            tassert(false, "table already exists");
            continue;
        }

        TiXmlElement * column = table->FirstChildElement("column");
        MemonyLayout * layout = NEW MemonyLayout;
        s32 index = 0xffff;
        s32 i = 0;
        while (column) {
            const char * name = column->Attribute("name");
            const char * type = column->Attribute("type");
            tassert(name && type, "wtf");

            bool key = (column->Attribute("key")) ? (tools::StringAsBool(column->Attribute("key"))) : false;
            bool visual = (column->Attribute("visual")) ? (tools::StringAsBool(column->Attribute("visual"))) : false;
            bool share = (column->Attribute("share")) ? (tools::StringAsBool(column->Attribute("share"))) : false;
            bool save = (column->Attribute("save")) ? (tools::StringAsBool(column->Attribute("save"))) : false;
            bool important = (column->Attribute("important")) ? (tools::StringAsBool(column->Attribute("important"))) : false;

            s32 size, mask;
            if (!strcmp(type, "s64")) {
                size = sizeof(s64);
                mask = type_s64;
            }
            else if (!strcmp(type, "float")) {
                size = sizeof(float);
                mask = type_float;
            }
            else if (!strcmp(type, "bool")) {
                size = sizeof(bool);
                mask = type_bool;
            }
            else if (!strcmp(type, "string")) {
                size = tools::StringAsInt(column->Attribute("size"));
                mask = type_string;
            } else {
                tassert(false, "what's this");
                return nullptr;
            }

            if (key) {
                tassert(0xffff == index, "wtf");
                (0xffff == index) ? (index = i) : (NULL);
            } 
            layout->AddLayout(name, size, mask, visual, share, save, important);
            column = column->NextSiblingElement("column");
            i++;
        }

        if (0xffff == index) {
            layouts.push_back(oTableLayout(nullptr, layout));
        } else {
            layouts.push_back(oTableLayout(layout->Query(index), layout));
        }
        table = table->NextSiblingElement("table");
        g_table_layout_map.insert(make_pair(tablename, layout));
    }

//     for (s32 i = 0; i < layouts.size(); i++) {
//         DebugLog(g_core, "============== object %s table enum start =================", name);
//         layouts[i]._layout->EchoTemplate(g_core);
//         DebugLog(g_core, "============== object %s table enum end =================", name);
//     }

    g_object_table_layouts.insert(make_pair(name, layouts));
    return &g_object_table_layouts[name];
}

IUnit * Model::FindUnit(const s64 id) {
    unit_map::iterator itor = g_unit_map.find(id);
    if (itor == g_unit_map.end()) {
        return nullptr;
    }

    return itor->second;
}

void Model::TraceMemory() {
    TraceLog(g_core, "==============================trace memory start==========================");
    s64 tick = tools::time::GetTimeMillisecond();
    for (unit_map::iterator itor = g_unit_map.begin(); itor != g_unit_map.end(); itor++) {
        TraceLog(g_core, "Memory trace : object %s create from %s:%d live %d:%d:%d",
            itor->second->_name.c_str(), itor->second->_file.c_str(), itor->second->_line,
            ((tick - itor->second->_create_tick) / (60 * 60 * 1000)),
            ((tick - itor->second->_create_tick) / (60 * 1000)) % 60,
            ((tick - itor->second->_create_tick) / 1000) % 60);
    }
    TraceLog(g_core, "==============================trace memory end==========================");
}

void Model::CleanMemory() {
    MemonyPool::GetInstance().Clean(g_core);
    TraceLog(g_core, "Clean menory");
}

void Model::OnTimer(ICore * core, const s32 id, const IContext & context, const s64 tick) {
    CleanMemory();
}

IUnit * Model::Create(const char * name, const char * file, const s32 line) {
    memonylayout_map::const_iterator citor = g_unit_layout_map.find(name);
    if (citor == g_unit_layout_map.end()) {
        tassert(false, "what's this, %s", name);
        return nullptr;
    }

    const MemonyLayout * layout = citor->second;
    const s64 id = g_idmanager->CreateID();
    if (g_unit_map.find(id) != g_unit_map.end()) {
        tassert(false, "object id is exists");
        return nullptr;
    }

    Unit * object = CREATE_FROM_POOL(g_object_pool, name, id, layout, file, line);
    tassert(object, "create object error");
    if (nullptr == object) {
        return nullptr;
    }

    g_unit_map.insert(make_pair(id, object));
    for (auto i = s_any_obj_created_calls.cbegin(); i != s_any_obj_created_calls.cend(); i++) {
        i->_fun(g_core, object);
    }

    auto ifind = s_obj_created_calls.find(name);
    if (ifind != s_obj_created_calls.end()) {
        for (auto i = ifind->second.cbegin(); i != ifind->second.cend(); i++) {
            i->_fun(g_core, object);
        }
    }
    return object;
}

IUnit * Model::CreateUnitByID(const char * name, const s64 id, const char * file, const s32 line) {
    unit_map::iterator itor = g_unit_map.find(id);
    if (itor != g_unit_map.end()) {
        tassert(false, "object %s id %lld is exists", itor->second->GetName(), itor->second->GetID());
        return nullptr;
    }

    memonylayout_map::const_iterator citor = g_unit_layout_map.find(name);
    if (citor == g_unit_layout_map.end()) {
        tassert(false, "what's this, %s", name);
        return nullptr;
    }

    const MemonyLayout * info = citor->second;
    Unit * object = CREATE_FROM_POOL(g_object_pool, name, id, info, file, line);
    tassert(object, "create object error");
    if (nullptr == object) {
        return nullptr;
    }

    g_unit_map.insert(make_pair(id, object));
    for (auto i = s_any_obj_created_calls.cbegin(); i != s_any_obj_created_calls.cend(); i++) {
        i->_fun(g_core, object);
    }

    auto ifind = s_obj_created_calls.find(name);
    if (ifind != s_obj_created_calls.end()) {
        for (auto i = ifind->second.cbegin(); i != ifind->second.cend(); i++) {
            i->_fun(g_core, object);
        }
    }
    return object;
}

void Model::Recove(IUnit * object, const char * file, const s32 line) {
    tassert(object, "wtf");
    if (nullptr == object) {
        return;
    }

    unit_map::iterator itor = g_unit_map.find(object->GetID());
    if (itor == g_unit_map.end() || itor->second != object) {
        tassert(false, "where is this object %lld", object->GetID());
        return;
    }

    for (auto i = s_any_obj_destory_calls.cbegin(); i != s_any_obj_destory_calls.cend(); i++) {
        i->_fun(g_core, object);
    }

    auto ifind = s_obj_destory_calls.find(object->GetName());
    if (ifind != s_obj_destory_calls.end()) {
        for (auto i = ifind->second.cbegin(); i != ifind->second.cend(); i++) {
            i->_fun(g_core, object);
        }
    }

    g_object_pool.Recover(itor->second);
    g_unit_map.erase(itor);
}

ITable * Model::FindStaticTable(const char * name) {
    table_map::iterator itor = g_table_map.find(name);
    if (itor == g_table_map.end()) {
        tassert(false, "table is not exists");
        return nullptr;
    }

    return itor->second;
}

//注册对象创建成功回调
void Model::RegisterCreatedCall(const char * name, const Handle & handle) {
    if (nullptr == name) {
        s_any_obj_created_calls.insert(handle);
    } else {
        auto ifind = s_obj_created_calls.find(name);
        if (ifind == s_obj_created_calls.end()) {
            ifind = s_obj_created_calls.insert(make_pair(name, ANY_OBJECT_CREATED_CALL())).first;
        }
        ifind->second.insert(handle);
    }
}
//注册对象销毁回调
void Model::RegisterDestoryedCall(const char * name, const Handle & handle) {
    if (nullptr == name) {
        s_any_obj_destory_calls.insert(handle);
    }
    else {
        auto ifind = s_obj_destory_calls.find(name);
        if (ifind == s_obj_destory_calls.end()) {
            ifind = s_obj_destory_calls.insert(make_pair(name, ANY_OBJECT_DESTORY_CALL())).first;
        }
        ifind->second.insert(handle);
    }
}
