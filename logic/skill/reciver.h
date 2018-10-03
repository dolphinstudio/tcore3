#ifndef __reciver_h__
#define __reciver_h__

#include "header.h"

class reciver {
public:
    static reciver & getInstance();

    bool getSkillRecivers(iUnit * role, const s32 type, std::set<s64> & recivers);
};

#endif //__reciver_h__
