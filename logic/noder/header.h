#ifndef __header_h__
#define __header_h__

#include "iNoder.h"
#include "iSession.h"
#include "iIdmanager.h"

#include "noder.pb.h"

#include "tinyxml/tinyxml.h"
#include "tools.h"
#include "tqcsort.h"

#include <map>
#include <set>
#include <string>
#include <list>

typedef std::set<std::string> target_set;
extern target_set g_target_set;

extern std::string g_master_ip;
extern s32 g_master_port;

extern std::string g_noder_name;
extern std::string g_noder_ip;
extern s32 g_noder_port;
extern s32 g_noder_id;
extern s32 g_noder_area;
extern bool g_is_all_area;

extern s32 g_send_pipe_size;
extern s32 g_recv_pipe_size;

class noderSession;
typedef std::map<s32, noderSession *> id_session_map;
typedef std::map<std::string, std::map<s32, noderSession *>> name_id_session_map;

extern id_session_map g_id_session_map;
extern name_id_session_map g_name_id_sessions_map;

extern api::iCore * g_core;
extern iIdmanager * g_idmanager;

namespace timer {
    namespace id {
        enum {
            try_connect_master_again = 0,
        };
    }

    namespace config {
        const static s32 try_connect_master_again_interval = 1000;
    }
}

void noderReport(noderSession * session, const char * file, const s32 line);
void noderDropout(noderSession * session, const char * file, const s32 line);
#define noder_report(session) noderReport(session, __FILE__, __LINE__);
#define noder_dropout(session) noderDropout(session, __FILE__, __LINE__);

struct oNoderEvent {
    const fNoderEvent _ev;
    const std::string _debug;

    oNoderEvent(const fNoderEvent ev, const std::string & debug) : _ev(ev), _debug(debug) {}
};
extern std::list<oNoderEvent> g_noder_events[eNoderEvent::count];

inline void PushNoderEvent(const eNoderEvent id, iNoderSession * session) {
    for (auto itor = g_noder_events[id].begin(); itor != g_noder_events[id].end(); itor++) {
        itor->_ev(session);
    }
}

#define invalid_id -1

typedef std::map<std::string, tlib::sort_pool> noder_load_sort_pool_map;
extern noder_load_sort_pool_map g_noder_load_sort_pool_map;

extern std::map<s16, iProtoer<iNoderSession>> g_noder_c_protoer_map;

extern fAllClientProtoCallback g_all_client_proto_call;

#endif //__header_h__
