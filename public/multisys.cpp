#include "multisys.h"

#include <stdio.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif 
    void _assertionfail(const char * file, int line, const char * funname, const char * debug) {
#ifdef _DEBUG
        fflush(stdout);
        printf("assert : %s:%s:%d\nassert info:%s\n", file, funname, line, debug);
        fflush(stderr);
        assert(false);
#endif //_DEBUG
    }
#ifdef __cplusplus
};
#endif