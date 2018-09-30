#ifndef __logfile_h__
#define __logfile_h__
#include "multisys.h"
#include <string>

namespace tcore {
    class logfile {
    public:
        bool Open(const char * path, const char * name);
        bool IsOpen();
        void Close();

        void Write(const char * data);
        void Flush();

        inline s64 GetOpenTick() {
            tassert(_open_tick != 0, "wtf");
            return _open_tick;
        }

        logfile() : _open_tick(0), _file(nullptr) {}

    private:
        s64 _open_tick;
        FILE * _file;
    };
}
#endif //__logfile_h__
