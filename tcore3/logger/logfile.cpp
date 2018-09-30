#include "logfile.h"
#include "tools.h"

namespace tcore {
    bool logfile::Open(const char * path, const char * name) {
        if (_file) {
            tassert(!_file, "WTF");
            return false;
        }

        std::string filepath;
        filepath += path;
        filepath += name;

        _file = fopen(filepath.c_str(), "ab+");
        if (nullptr == _file) {
            tassert(_file, "wtf");
            return false;
        }
        _open_tick = tools::time::getMillisecond();

        return true;
    }

    bool logfile::IsOpen() {
        return _file != nullptr;
    }

    void logfile::Close() {
        if (!_file) {
            tassert(false, "wtf");
            return;
        }

        fflush(_file);
        fclose(_file);
        _file = nullptr;
        _open_tick = 0;
    }

    void logfile::Write(const char * data) {
        tassert(_file && data, "wtf");
        if (_file) {
            s32 len = strlen(data);
            fwrite(data, len, 1, _file);
        }
    }

    void logfile::Flush() {
        tassert(_file, "wtf");
        if (_file) {
            fflush(_file);
        }
    }
}
