#include "reciver.h"

reciver & reciver::getInstance() {
    static reciver static_recver;
    return static_recver;
}

bool reciver::getSkillRecivers(iUnit * role, const s32 type, std::set<s64> & recivers) {
    const s64 orbit_id = role->getAttrInt64(dc::role::attr::orbit);
    iUnit * orbit = g_model->findUnit(orbit_id);
    tassert(orbit, "wtf");
    if (nullptr == orbit) {
        error(g_core, "get orbit %lld from role %lld error", orbit_id, role->getid());
        return false;
    }

    switch (type) {
    case eTargetType::self: {
        recivers.insert(role->getid());
        break;
    }
    case eTargetType::own_team: {
        iTable * team = nullptr;
        switch (role->getAttrInt64(dc::role::attr::side)) {
        case eSide::top: {
            team = orbit->findTable(dc::orbit::table::top::tag);
            break;
        }
        case eSide::bottom: {
            team = orbit->findTable(dc::orbit::table::bottom::tag);
            break;
        }
        }
        
        for (s32 i = 0; i < team->rowCount(); i++) {
            iRow * row = team->getRow(i);
            recivers.insert(row->getInt64(dc::orbit::table::top::column_id_int64_key));
        }
        break;
    }
    case eTargetType::enemy_team: {
        iTable * team = nullptr;
        switch (role->getAttrInt64(dc::role::attr::side)) {
        case eSide::top: {
            team = orbit->findTable(dc::orbit::table::bottom::tag);
            break;
        }
        case eSide::bottom: {
            team = orbit->findTable(dc::orbit::table::top::tag);
            break;
        }
        }

        for (s32 i = 0; i < team->rowCount(); i++) {
            iRow * row = team->getRow(i);
            recivers.insert(row->getInt64(dc::orbit::table::top::column_id_int64_key));
        }
        break;
    }
    case eTargetType::first_enemy: {
        iTable * team = nullptr;
        switch (role->getAttrInt64(dc::role::attr::side)) {
        case eSide::top: {
            team = orbit->findTable(dc::orbit::table::bottom::tag);
            break;
        }
        case eSide::bottom: {
            team = orbit->findTable(dc::orbit::table::top::tag);
            break;
        }
        }

        iRow * row = team->getRow(0);
        if (row) {
            recivers.insert(row->getInt64(dc::orbit::table::top::column_id_int64_key));
        }
        break;
    }
    case eTargetType::orbit: {
        iTable * team = orbit->findTable(dc::orbit::table::top::tag);
        for (s32 i = 0; i < team->rowCount(); i++) {
            iRow * row = team->getRow(i);
            recivers.insert(row->getInt64(dc::orbit::table::top::column_id_int64_key));
        }

        team = orbit->findTable(dc::orbit::table::bottom::tag);
        for (s32 i = 0; i < team->rowCount(); i++) {
            iRow * row = team->getRow(i);
            recivers.insert(row->getInt64(dc::orbit::table::bottom::column_id_int64_key));
        }

        {
            const s64 combinerid = orbit->getAttrInt64(dc::orbit::attr::combiner);
            iUnit * combiner = g_model->findUnit(combinerid);
            if (combiner) {
                iTable * toper = combiner->findTable(dc::combiner::table::top_role::tag);
                for (s32 i = 0; i < toper->rowCount(); i++) {
                    iRow * row = toper->getRow(i);
                    recivers.insert(row->getInt64(dc::combiner::table::top_role::column_id_int64_key));
                }

                iTable * bottomer = combiner->findTable(dc::combiner::table::bottom_role::tag);
                for (s32 i = 0; i < bottomer->rowCount(); i++) {
                    iRow * row = bottomer->getRow(i);
                    recivers.insert(row->getInt64(dc::combiner::table::bottom_role::column_id_int64_key));
                }

                recivers.insert(combinerid);
            }
        }

        break;
    }
    case eTargetType::first_enemy_and_teammate: {

        iTable * team = orbit->findTable(dc::orbit::table::top::tag);
        iRow * row = team->getRow(0);
        if (row) {
            recivers.insert(row->getInt64(dc::orbit::table::top::column_id_int64_key));
        }

        team = orbit->findTable(dc::orbit::table::bottom::tag);
        row = team->getRow(0);
        if (row) {
            recivers.insert(row->getInt64(dc::orbit::table::bottom::column_id_int64_key));
        }

        break;
    }
    }

    return true;
}
