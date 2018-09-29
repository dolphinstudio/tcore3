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

        bool getfiles(const char * dicpath, const char * extension, OUT opaths & paths, OUT onames & names, OUT s32 & count) {
            DIR * dp;
            struct dirent* dirp;
            struct stat st;

            /* open dirent directory */
            if ((dp = opendir(dicpath)) == NULL) {
                perror("opendir");
                return false;
            }

            /**
            * read all files in this dir
            **/
            while ((dirp = readdir(dp)) != NULL) {
                char fullname[255];
                memset(fullname, 0, sizeof(fullname));

                /* ignore hidden files */
                if (dirp->d_name[0] == '.')
                    continue;

                strncpy(fullname, dicpath, sizeof(fullname));
                strncat(fullname, "/", sizeof(fullname));
                strncat(fullname, dirp->d_name, sizeof(fullname));
                /* get dirent status */
                if (stat(fullname, &st) == -1) {
                    perror("stat");
                    fputs(fullname, stderr);
                    return false;
                }

                /* if dirent is a directory, call itself */
                if (S_ISDIR(st.st_mode)) {
                    if (getfiles(fullname, extension, paths, names, count) == -1) {
                        return false;
                    }
                }
                else {
                    /* display file name with proper tab */
                    //printf("%s/%s\n", dirname, dirp->d_name);
                    if (strcmp(get_filename_ext(dirp->d_name), extension) == 0) {
                        paths.push_back(string(dicpath) + "/" + dirp->d_name);
                        char name[255];
                        strncpy(name, dirp->d_name, sizeof(name));
                        char * dot = strrchr(name, '.');
                        tassert(dot, "wtf");
                        *dot = 0;
                        names.push_back(name);
                        count++;
                    }
                }
            }
            return true;
        }
    }
}

