#ifndef __header_h__
#define __header_h__

#include "iLogicer.h"
#include "iNoder.h"
#include "iEvent.h"

#include "ev.h"
#include "define.h"

#include <unordered_map>
using namespace std;

extern iNoder * g_noder;
extern iEvent * g_event;

extern std::map<s64, ev::omatcher> g_account_info_map;

typedef unordered_map<s16, lMessager> messager_map;
typedef unordered_map<s64, messager_map> messager_pool;
extern messager_pool g_messager_pool;

#endif //__header_h__
