/* 
 * File:   Header.h
 * Author: alax
 *
 * Created on March 3, 2015, 10:44 AM
 */

#ifndef __Header_h__
#define __Header_h__

#include "ICore.h"
#include "IModel.h"
#include "IEvent.h"
#include "IIDManager.h"

#include "Tools.h"
#include "TPool.h"
#include "Memony.h"
#include "Table.h"
#include "Row.h"
#include "define.h"
#include "tinyxml/tinyxml.h"
#include <vector>
#include <map>
#include <list>
#include <unordered_set>
#include <unordered_map>
using namespace tcore;
using namespace dc;
using namespace std;
using namespace tlib;

class Unit;
class Row;
class Table;
class AttributeHandle;

typedef unordered_map<s64, Unit *> unit_map;
extern unit_map g_unit_map;
; 
typedef map<std::string, const MemonyLayout *> memonylayout_map;
extern memonylayout_map g_unit_layout_map;
extern memonylayout_map g_table_layout_map;

typedef vector<Table *> tables;
typedef unordered_map<std::string, Table *> table_map;
extern table_map g_table_map;

typedef map<string, string> string_map;
extern string_map g_path_map;

struct oTableLayout {
    const dc::layout * _key;
    const MemonyLayout * _layout;

    oTableLayout(const dc::layout * key, const MemonyLayout * layout) : _key(key), _layout(layout) {}
};
typedef vector<oTableLayout> table_layout_array;
typedef map<std::string, table_layout_array> object_table_layouts;

extern ICore * g_core;
extern IEvent * g_event;
extern IIDManager * g_idmanager;

extern object_table_layouts g_object_table_layouts;

extern TPool<Unit, 1, 4096> g_object_pool;
extern TPool<Row, 1, 512> g_row_pool;
extern TPool<Table, 1, 4096> g_table_pool;

typedef void(*call)(ICore *, IUnit *);
typedef unordered_map<std::string, Table *> table_map;

#endif //__Header_h__
