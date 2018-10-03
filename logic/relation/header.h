#ifndef __RelationHeader_h__
#define __RelationHeader_h__

#include "iNoder.h"
#include "tools.h"
extern iNoder * g_noder;

#include <unordered_map>
using namespace std;

typedef unordered_map<s64, s32> account_gate_map;
extern account_gate_map g_account_gate_map;

#endif //__RelationHeader_h__
