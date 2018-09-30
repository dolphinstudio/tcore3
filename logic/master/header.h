#ifndef __header_h__
#define __header_h__

#include "iMaster.h"
#include "iSession.h"
#include "Noder.pb.h"
#include "tinyxml/tinyxml.h"

#include "tools.h"

#include <map>
#include <set>

class masterSession;
typedef std::map<s32, masterSession *> id_session_map;
typedef std::map<std::string, std::set<masterSession *>> name_sessions_map;

extern id_session_map g_id_session_map;
extern name_sessions_map g_name_sessions_map;

extern std::string g_master_ip;
extern s32 g_master_port;

extern s32 g_send_pipe_size;
extern s32 g_recv_pipe_size;

namespace timer {
    namespace id {
        enum {
            broadcast_noder_info = 1,
        };
    }

    namespace config {
        const static s32 broadcast_noder_info_interval = 3000;
    }
}

#define invalid_id -1

#endif //__header_h__
