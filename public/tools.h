#ifndef __tools_h__
#define __tools_h__

#include "multisys.h"
#include <chrono>
#include <string>
#include <vector>

namespace tools {
    namespace memery {
        inline void safeMemset(void *__restrict __dest, size_t __max, int val, size_t __n) {
            tassert(__n <= __max, "over flow");
            memset(__dest, val, (__max >= __n) ? (__n) : (__max));
        }

        inline void safeMemcpy(void *__restrict __dest, size_t __max, const void *__restrict __src, size_t __n) {
            tassert(__n <= __max, "over flow");
            memcpy(__dest, __src, (__max >= __n) ? (__n) : (__max));
        }
    }
    
    namespace system {
        u64 getCurrentProcessId();
    }

    namespace time {
        inline bool safe_localtime(struct tm & tm, const time_t & tt) {
            const struct tm * p = localtime(&tt);
            if (nullptr == p) {
                return false;
            }
            
            memery::safeMemcpy(&tm, sizeof(tm), p, sizeof(tm));
            return true;
        }
        
        inline u64 getMillisecond() {
            return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        }
        
        inline u64 getMicrosecond() {
            return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        }

        inline std::string getCurrentTimeString(const char * format = "%4d-%02d-%02d %02d:%02d:%02d") {
            char strtime[64] = { 0 };
            auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            struct tm tm;
            safe_localtime(tm, tt);
            safesprintf(strtime, sizeof(strtime), format,
                (int)tm.tm_year + 1900, (int)tm.tm_mon + 1, (int)tm.tm_mday,
                (int)tm.tm_hour, (int)tm.tm_min, (int)tm.tm_sec);
            return strtime;
        }

        inline std::string getTimeString(const s64 tick, const char * format = "%4d-%02d-%02d %02d:%02d:%02d"){
            char strtime[128];
            auto mTime = std::chrono::milliseconds(tick);
            auto tp = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>(mTime);
            auto tt = std::chrono::system_clock::to_time_t(tp);
            struct tm tm;
            safe_localtime(tm, tt);
            safesprintf(strtime, sizeof(strtime), format,
                (int)tm.tm_year + 1900, (int)tm.tm_mon + 1, (int)tm.tm_mday,
                (int)tm.tm_hour, (int)tm.tm_min, (int)tm.tm_sec);
            return strtime;
        }
    }

    namespace file {
        const char * getApppath();
        bool mkdir(const char * path);

        typedef std::vector<std::string> opaths, onames;
        bool getfiles(const char * dir, const char * ex, OUT opaths & paths, OUT onames & names, OUT s32 & count);
    }
    
    template <typename t>
    struct oHashFunction {
        size_t operator()(const t & src) const {
            return (size_t)src;
        }
    };

    typedef std::vector<std::string> osplitres;
    inline s32 split(const std::string & src, const std::string & commas, osplitres & res) {
        if (src == "") {
            return 0;
        }
        //方便截取最后一段数据
        std::string strs = src + commas;

        size_t pos = strs.find(commas);
        size_t size = strs.size();

        while (pos != std::string::npos) {
            std::string x = strs.substr(0, pos);
            if (x != "") {
                res.push_back(x.c_str());
            }
            strs = strs.substr(pos + commas.size(), size);
            pos = strs.find(commas);
        }

        return res.size();
    }

    inline bool stringAsBool(const char * b) {
        return !strcmp(b, "true");
    }

    inline s32 stringAsInt(const char * value) {
        tassert(value, "null point");
        return atoi(value);
    }

    inline float stringAsFloat(const char * value) {
        tassert(value, "null point");
        return atof(value);
    }

    inline s64 stringAsInt64(const char * value) {
        tassert(value, "null point");
        return atoi64(value);
    }

    inline std::string int64AsString(const s64 value) {
        char str[128] = { 0 };
        safesprintf(str, sizeof(str), "%lld", value);
        return str;
    }
    
    inline std::string intAsString(const s32 value) {
        char str[128] = { 0 };
        safesprintf(str, sizeof(str), "%d", value);
        return str;
    }
    
    inline std::string floatAsString(const double value) {
        char str[128] = { 0 };
        safesprintf(str, sizeof(str), "%f", value);
        return str;
    }

    static s32 rand(s32 range) {
        if (0 == range) { return 0; }
        static u64 s_seed = time::getMillisecond();
        s_seed = (((s_seed = s_seed * 214013L + 2531011L) >> 16) & 0x7fff);
        return s_seed % range;
    }
}

#endif //__tools_h__
