#ifndef __score_h__
#define __score_h__

#include "header.h"

class score : public IScore {
public:
    virtual ~score() {}
    virtual bool initialize(api::iCore * core);
    virtual bool launch(api::iCore * core);
    virtual bool destroy(api::iCore * core);

private:
    static void on_calc_score(api::iCore * core, const ev::ocalc_score & ev);
};

#endif //__score_h__
