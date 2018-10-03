#ifndef __scene_h__
#define __scene_h__

#include "header.h"

class scene : public iScene {
public:
    virtual bool initialize(api::iCore * core);
    virtual bool launch(api::iCore * core);
    virtual bool destroy(api::iCore * core);

    virtual iUnit * createGameScene();
    virtual iUnit * queryGameScene(iUnit * player);
    virtual void broadcast(iUnit * scene, const s16 id, const void * data, const s32 size);
private:
    static void on_login_success(api::iCore * core, const ev::ologin_success & body);
};

#endif //__scene_h__
