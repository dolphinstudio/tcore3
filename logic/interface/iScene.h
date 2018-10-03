#ifndef __iScene_h__
#define __iScene_h__

#include "api.h"
#include "google/protobuf/message.h"

#include <vector>

class iUnit;

class iScene : public iModule {
public:
    virtual ~iScene() {}

    virtual iUnit * createGameScene() = 0;
    virtual iUnit * queryGameScene(iUnit * player) = 0;

    virtual void broadcast(iUnit * scene, const s16 id, const void * data, const s32 size) = 0;

    virtual void broadcast(iUnit * scene, const s16 id, const ::google::protobuf::Message & body) {
        const s16 size = body.ByteSize();
        char * buff = (char *)alloca(size);
        if (body.SerializePartialToArray(buff, size)) {
            broadcast(scene, id, buff, size);
        }
    }
};

#endif //__iScene_h__
