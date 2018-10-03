#include "score.h"

bool score::initialize(api::iCore * core) {
    return true;
}

bool score::launch(api::iCore * core) {
    g_event = (iEvent *)core->findModule("event");
    register_event(g_event, ev::id::calc_score, score::on_calc_score);
    return true;
}

bool score::destroy(api::iCore * core) {
    return true;
}

void score::on_calc_score(api::iCore * core, const ev::ocalc_score & ev) {
    const float length = ev.scene->getAttrFloat(dc::scene::attr::orbit_length);

    if ((ev.y <= 0 && ev.side == eSide::top) || (ev.y >= length && ev.side == eSide::bottom)) {
        s64 playerid = ev.role->getAttrInt64(dc::role::attr::player);
        iTable * members = ev.scene->findTable(dc::scene::table::member::tag);
        iRow * member = members->findRow(playerid);
        tassert(member, "wtf");
        if (member) {
            s32 point = member->getInt64(dc::scene::table::member::column_point_int64) + ev.role->getAttrInt64(dc::role::attr::point);
            member->setInt64(dc::scene::table::member::column_point_int64, point);

            oServerPointChangePush push;
            set_oIdentity(push.mutable_account(), playerid);
            //push.set_account(tools::int64AsString(playerid));
            push.set_point(point);
            g_scene->broadcast(ev.scene, eSProtoID::ServerPointChangePush, push);
        }
    }
}
