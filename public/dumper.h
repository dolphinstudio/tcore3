#ifndef __dumper_h__
#define __dumper_h__

#ifndef _WINDOWS_  
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif  
#include <ImageHlp.h>

namespace tlib {
    class dumper {
    public:
        static dumper & getInstance(void);
        ~dumper();
        void setDumpFilename(const TCHAR * dumpName);

    protected:
        static void createMinidump(EXCEPTION_POINTERS * pep);
        static long __stdcall snUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo);
        static bool __stdcall miniDumpCallback(PVOID pParam, const PMINIDUMP_CALLBACK_INPUT pInput, PMINIDUMP_CALLBACK_OUTPUT pOutput);

    private:
        dumper(void);
        dumper(const dumper & rhs) {}
    private:
        LPTOP_LEVEL_EXCEPTION_FILTER m_previousFilter;
    };
}

#endif //__dumper_h__
