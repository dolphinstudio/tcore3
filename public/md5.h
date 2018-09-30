#ifndef __md5_h__
#define __md5_h__
#include "multisys.h"
#include <string>
#include <cstring>

#define _CRT_SECURE_NO_WARNINGS
namespace tlib {
    std::string md5(std::string dat);
    std::string md5(const void* dat, size_t len);
    std::string md5file(const char* filename);
    std::string md5file(FILE* file);
    std::string md5sum6(std::string dat);
    std::string md5sum6(const void* dat, size_t len);
}


#endif __md5_h__