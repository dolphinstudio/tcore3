#ifndef __httper_h__
#define __httper_h__

#include "cthread.h"
#include "httpRequest.h"

namespace tcore {
    class httper : public tlib::cthread {
    public:
        static httper * getInstance();

        virtual bool launch() { return true; }
        virtual void process(const s32 overtiem);
        virtual void shutdown();

        virtual void run();
        virtual void terminate();

        virtual httpRequest * getHttpRequest(const s64 account, const s64 id, const char * url, iHttpResponse * response, const iContext & context);
        virtual void push(httpRequest * request);

    private:
        httper() {}
    };
}

#endif //__Httper_h__
