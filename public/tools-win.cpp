#include "tools.h"
#include <Shlwapi.h>
#include <direct.h>  
#include <io.h>  
#pragma comment(lib, "shlwapi.lib")

namespace tools {
    namespace system {
        u64 getCurrentProcessId() {
            return ::GetCurrentProcessId();
        }
    }

    namespace file {
        const char * getApppath() {
            static char * s_path = nullptr;

            if (nullptr == s_path) {
                s_path = NEW char[256];
                char link[256];
                ZeroMemory(link, sizeof(link));
                memset(s_path, 0, 256);
                GetModuleFileName(nullptr, s_path, 256);
                PathRemoveFileSpec(s_path);
            }

            return s_path;
        }

        bool mkdir(const char * path) {
            if (::PathIsDirectory(path)) {
                return true;
            }
            return ::CreateDirectory(path, 0);
        }
    }
}
