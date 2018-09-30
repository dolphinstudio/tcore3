#ifndef __header_h__
#define __header_h__

#include "api.h"
#include "tools.h"
#include "tpool.h"

using namespace tcore;
using namespace api;

extern iCore * g_core;

class tcpSession;
extern tlib::tpool<tcpSession> g_tcpsession_pool;

extern s32 g_connection_count;
extern s32 g_recv_total_size;
extern s32 g_send_total_size;

#pragma pack(1)
struct oHeader {
    s32 _size;
    s64 _tick;
};
#pragma pack(pop)
#endif //__header_h__
