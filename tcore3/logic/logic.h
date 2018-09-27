#ifndef __logic_h__
#define __logic_h__

#include "api.h"

namespace tcore {
    class logic {
    public:
        static logic * getInstance();

        virtual bool launch();
        virtual void proces(const s32 overtiem) {}
        virtual void shutdown();

        virtual iModule * findModule(const char * name);
        virtual void loadModule(const char * path);
    };
}


#endif //__logic_h__
