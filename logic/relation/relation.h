#ifndef __relation_h__
#define __relation_h__

#include "header.h"

class relation : public iModule {
public:
    virtual ~relation() {}

    virtual bool initialize(api::iCore * core);
    virtual bool launch(api::iCore * core);
    virtual bool destroy(api::iCore * core);

private:
    static void onNoderReport(iNoderSession * session);
    static void onNoderDropout(iNoderSession * session);

private:
    static void onAccountReport(api::iCore * core, iSession * session, s64 context, const oAccountReport & body);
    static void onAccountLogout(api::iCore * core, iSession * session, s64 context, const oAccountLogout & body);
};

#endif //__relation_h__
