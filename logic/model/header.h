/* 
 * File:   Header.h
 * Author: alax
 *
 * Created on March 3, 2015, 10:44 AM
 */

#ifndef __header_h__
#define __header_h__

#include "api.h"
#include "iModel.h"
#include "iEvent.h"
#include "iIdmanager.h"

#include "tools.h"
#include "tpool.h"
#include "memony.h"
#include "table.h"
#include "row.h"
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
class row;
class table;
class modifyer;

typedef unordered_map<s64, Unit *> unit_map;
extern unit_map g_unit_map;
; 
typedef map<std::string, const mlayout *> memonylayout_map;
extern memonylayout_map g_unit_layout_map;
extern memonylayout_map g_table_layout_map;

typedef vector<table *> tables;
typedef unordered_map<std::string, table *> table_map;
extern table_map g_table_map;

typedef map<string, string> string_map;
extern string_map g_path_map;

struct oTableLayout {
    const dc::layout * _key;
    const mlayout * _layout;

    oTableLayout(const dc::layout * key, const mlayout * layout) : _key(key), _layout(layout) {}
};
typedef vector<oTableLayout> table_layout_array;
typedef map<std::string, table_layout_array> object_table_layouts;

extern api::iCore * g_core;
extern iEvent * g_event;
extern iIdmanager * g_idmanager;

extern object_table_layouts g_object_table_layouts;

extern tlib::tpool<Unit, 1, 4096> g_object_pool;
extern tlib::tpool<row, 1, 512> g_row_pool;
extern tlib::tpool<table, 1, 4096> g_table_pool;

typedef void(*call)(api::iCore *, iUnit *);
typedef unordered_map<std::string, table *> table_map;

#endif //__header_h__
