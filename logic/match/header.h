#ifndef __MatchHeader_h__
#define __MatchHeader_h__

#include "iMatch.h"
#include "iEvent.h"
#include "iNoder.h"
#include "iConfiger.h"
#include "iIDManager.h"

#include "iPublic.h"
#include "tpool.h"

#include "ev.h"
#include "define.h"

#include "noder.pb.h"
#include "game.pb.h"

#include "UUID.h"

extern api::iCore * g_core;

extern iEvent * g_event;
extern iNoder * g_noder;
extern iIdmanager * g_idmanager;

extern iNoderSession * g_relation;


struct oWaiterInfo {
    s64 account;
    s32 gateid;
    s32 logicid;
    oWaiterInfo() : account(define::invalid_id), gateid(define::invalid_id), logicid(define::invalid_id) {}

    oWaiterInfo(s64 ac, s32 gate, s32 logic) : account(ac), gateid(gate), logicid(logic) {}

    void clear() {
        account = define::invalid_id;
        gateid = define::invalid_id;
        logicid = define::invalid_id;
    }
};

extern std::map<s64, oWaiterInfo> g_account_logic_map;

struct oChallenge {
    const s64 _id;
    oWaiterInfo _initiator;
    oChallenge(const s64 id) : _id(id) {}
};
extern std::unordered_map<s64, oChallenge *> g_challenge_map;
extern std::unordered_map<s64, oChallenge *> g_account_challenge_map;

extern tlib::tpool<oChallenge> g_challenge_pool;

namespace timer {
    enum id {
        insert_ai,
        challenge_recover,
    };

    namespace config {
        static s32 insert_ai_delay = 5 * SECOND;
        static s32 challenge_recover_delay = 60 * SECOND;
    }
}

#endif //__MatchHeader_h__
