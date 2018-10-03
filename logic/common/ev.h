#ifndef __ev_h__
#define __ev_h__

#include "multisys.h"

#include <vector>
#include <set>

class iDelegate;
class iUnit;

struct o_config_skill;

namespace ev {
    enum id {
        connected = 1,
        disconnect,

        login_success = 20,
        load_data,

        match_success = 40,

        load_card_data = 50,

        account_relogin = 65,

        game_start = 100,
        game_end,

        role_join_orbit = 150,
        join_combiner = 160,
        role_thrusting = 165,
        role_collision = 170,

        calc_score = 180,
        remove_role = 200,

        recover_orbit = 210,

        cast_skill = 300,

        player_reenter_game = 350,

        ai_ready = 500,
    };

    struct omatcher {
        s64 account;
        s32 gate;
        s32 match;
        s64 copyer;
        bool is_ai;
    };

    struct omatch_success {
        omatcher matchA;
        omatcher matchB;
    };

    struct oload_card_data {
        bool successd;
        s64 account;
    };

    struct ologin_success {
        iDelegate * deg;
        std::string name;
        std::string icon_url;
        bool isnewer;
    };
    
    struct oload_data {
        iUnit * player;
        bool isnewer;
    };

    struct orole_join_orbit {
        iUnit * orbit;
        iUnit * player;
        iUnit * role;
    };

    struct ojoin_combiner {
        iUnit * orbit;
        iUnit * combiner;
        iUnit * role;
    };

    struct orole_collision {
        iUnit * orbit;
        iUnit * role1;
        iUnit * role2;
    };

    struct ocalc_score {
        float y;
        int side;
        iUnit * role;
        iUnit * scene;
    };

    struct ocast_skill {
        iUnit * role;
        const o_config_skill * config;
        std::set<s64> targets;
    };
}

#endif //__ev_h__
