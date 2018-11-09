#ifndef __header_h__
#define __header_h__

#include "multisys.h"
#include "tools.h"

#include <string>
#include <unordered_map>
using namespace std;
typedef unordered_map<string, string> input_args_map;

extern input_args_map g_input_args;
extern std::string g_out_path;

inline void parse(int argc, const char ** argv) {
    for (int i = 1; i < argc; ++i) {
        if (strncmp(argv[i], "--", 2) == 0) {
            const char * start = argv[i] + 2;
            const char * equal = strstr(start, "=");
            if (equal != nullptr) {
                std::string name(start, equal);
                std::string val(equal + 1);
                g_input_args[name] = val;
            }
            else if (strlen(argv[i]) > 2) {
                g_input_args[argv[i] + 2] = "";
            }
        }
        else {
            //errorlog
        }
    }
}

inline const char * getarg(const char * name) {
    input_args_map::const_iterator itor = g_input_args.find(name);
    if (itor == g_input_args.end()) {
        return NULL;
    }

    return itor->second.c_str();
}

#endif //__header_h__
