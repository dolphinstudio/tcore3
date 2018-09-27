#ifndef __multisys_h__
#define __multisys_h__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef WIN32
typedef __int8  s8;
typedef __int16 s16;
typedef __int32 s32;
typedef __int64 s64;

typedef unsigned __int8  u8;
typedef unsigned __int16 u16;
typedef unsigned __int32 u32;
typedef unsigned __int64 u64;
#else
typedef unsigned char u8;
typedef unsigned short u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef char s8;
typedef short s16;
typedef int32_t s32;
typedef int64_t s64;
#endif //WIN32

#pragma execution_character_set("utf-8")
#ifdef __cplusplus
extern "C" {
#endif 
    void _assertionfail(const char * file, int line, const char * funname, const char * debug);
#ifdef __cplusplus
};
#endif

// //为了找到内存泄露的元凶，我要记录下每一处new所在的文件名和所在行。于是再次重载了operator new：
// void * operator new(size_t size, const char* file, const size_t line);
// void * operator new[](size_t size, const char* file, const size_t line);
// //为了避免编译时出现warning C4291(没有与operator new(unsigned int,const char *,const unsigned int) 匹配的delete)，又重载了
// void operator delete(void * pointer, const char* file, const size_t line);
// void operator delete[](void * pointer, const char* file, const size_t line);

#if defined _DEBUG
#define MALLOC malloc
#define FREE free
#define NEW new
#define DEL delete

#define tassert(p, format, ...) { \
    char debug[4096] = {0}; \
    safesprintf(debug, sizeof(debug), format, ##__VA_ARGS__); \
    ((p) ? (void)0 : (void)_assertionfail(__FILE__, __LINE__, __FUNCTION__, debug)); \
}

#else
#define MALLOC malloc
#define FREE free
#define NEW new
#define DEL delete

#define tassert

#endif //defined _DEBUG

#define OUT
#define IN

#ifdef WIN32 
#define safesprintf sprintf_s
#define msleep(n) Sleep(n)
#define atoi64 _atoi64
#endif //WIN32

#ifdef linux
#include <unistd.h>
#define safesprintf snprintf
#define msleep(n) usleep(n * 1000)
#define atoi64 atoll
#endif

#endif //__multisys_h__
