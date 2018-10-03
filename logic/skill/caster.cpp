#include "caster.h"

caster & caster::getInstance() {
    static caster static_recver;
    return static_recver;
}

bool caster::decideCastAdvantageInferiority(iUnit * role, const eAdvantageInferiority type, iUnit * faceenemy) {
    if (type == eAdvantageInferiority::default_cast) {
        return true;
    }

    switch (type) {
    case eAdvantageInferiority::default_cast: {
        return true; 
    }
    case eAdvantageInferiority::advantage_over_face_enemy: {
        const float role_weight = role->getAttrFloat(dc::role::attr::weight);
        const float enemy_weight = faceenemy->getAttrFloat(dc::role::attr::weight);
        return role_weight > enemy_weight && fabs(enemy_weight - role_weight) > 0.01f;
    }
    default:
        tassert(false, "wtf");
        return false;
    }


    return false;
}
