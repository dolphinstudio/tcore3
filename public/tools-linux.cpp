#include "tools.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <libgen.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h> 

namespace tools {
    namespace system {
        u64 getCurrentProcessId() {
            return ::getpid();
        }
    }
    
    namespace file {
        const char * getApppath() {
            static char * s_path = NULL;
            if (NULL == s_path) {
                char buff[256];
                char link[256];
                memset(link, 0, sizeof(link));
                memset(buff, 0, sizeof(buff));

                safesprintf(link, sizeof (link), "/proc/self/exe");

                int count = readlink(link, buff, sizeof(buff));
                if (count >= 265) {
                    tassert(false, "system path error");
                    return NULL;
                }
                const char * p = dirname(buff);
                s_path = NEW char[sizeof(buff) + 2];
                safesprintf(s_path, 256, "%s/", p);
            }

            return s_path;
        }

        bool mkdir(const char * path) {
            if (0 == access(path, S_IRUSR)) {
                return true;
            }
            
            int res = ::mkdir(path, S_IRUSR | S_IWUSR | S_IXUSR);
            return 0 == res;
        }
    }
}

