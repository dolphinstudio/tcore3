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

        bool getfiles(const char * dicpath, const char * extension, OUT opaths & paths, OUT onames & names, OUT s32 & count) {
            WIN32_FIND_DATA finder;
            HANDLE error;
            int filecount = 0;
            char path[512] = { 0 };
            count = 0;

            safesprintf(path, sizeof(path), "%s/*.*", dicpath);
            error = FindFirstFile(path, &finder);
            if (INVALID_HANDLE_VALUE == error) {
                return false;
            }

            while (::FindNextFile(error, &finder)) {
                if (strcmp(finder.cFileName, ".") == 0
                    || strcmp(finder.cFileName, "..") == 0) {
                    continue;
                }

                safesprintf(path, "%s/%s", dicpath, finder.cFileName);
                if (finder.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    s32 nCount = 0;
                    getfiles(path, extension, paths, names, nCount);
                    count += nCount;
                }
                else {
                    if (0 == strcmp(extension, PathFindExtension(finder.cFileName))) {
                        paths.push_back(path);
                        PathRemoveExtension(finder.cFileName);
                        names.push_back(finder.cFileName);
                        count++;
                    }
                }
            }

            return true;
        }
    }
}
