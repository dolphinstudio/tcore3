#ifndef __redis_def_h__
#define __redis_def_h__

#include "Tools.h"
using namespace tools;

namespace redis {
    namespace key {
        static const char * auth = "@auth";
        static const char * field_role = "@field_role";
    }

    static const std::string GetValue(const char * key, const s64 account) {
        std::string value;
        value << account << key;
        return value;
    }
}

#endif //__redis_def_h__
