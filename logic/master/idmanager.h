#ifndef __idmanager_h__
#define __idmanager_h__

#include "header.h"

class idmanager {
public:
    virtual ~idmanager() {}

    static idmanager & GetInstance();
    virtual s32 AllocaID(const std::string & name, const std::string & ip, const s32 port);

private:
    idmanager() : _id_index(0) {}

private:
    std::map<std::string, s32> _id_map;
    s32 _id_index;
};

#endif //__idmanager_h__
