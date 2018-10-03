#ifndef __define_h__
#define __define_h__

#include "multisys.h"

#define SECOND 1000
#define MIN (60 * SECOND)
#define HOUR (60 * MIN)
#define DAY (24 * HOUR)

#define float_is_zero(f) ((f) > -0.000001f && (f) < 0.000001f)

namespace define {
    enum {
        invalid_id = -1,
    };
}

#define tmin(a,b) (((a) < (b)) ? (a) : (b))

struct oIdentity64 {
    union {
        struct {
            u32 _up;
            u32 _down;
        };

        u64 _identity;
    };
};

#define set_oIdentity(o, id) {\
    oIdentity64 temp; \
    temp._identity = (id); \
    (o)->set_up(temp._up); \
    (o)->set_down(temp._down); \
}

#endif //__define_h__
