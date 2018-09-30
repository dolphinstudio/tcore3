#include "dumper.h"
#include "multisys.h"
#include <string>
#include <assert.h>
#include <tchar.h>
using namespace std;
#pragma comment(lib, "dbghelp.lib")

TCHAR m_acDumpName[MAX_PATH * 2] = { _T("\0") };
#define snArraySize(x) sizeof(x) / sizeof(x[0])

namespace tlib {
    TCHAR * snFindLastOfChar(const TCHAR * pSrc, const TCHAR * keyChar) {
        assert(nullptr != pSrc && nullptr != keyChar);
        size_t    nKeyLen = _tcslen(keyChar);
        const TCHAR * pFindPos = nullptr;
        const TCHAR * pLabor     = pSrc;
        while ((pLabor = _tcsstr(pLabor, keyChar)) != nullptr) {
            pLabor += nKeyLen;
            pFindPos = pLabor;
        }

        return    const_cast<TCHAR *>(pFindPos);
    }

    static string wstringToString(const wchar_t* pwText, UINT code ) {
        //assert(pwText);
        int iNeedSize = ::WideCharToMultiByte(code, 0, pwText, -1, nullptr, 0, nullptr, nullptr);

        if (iNeedSize >= 4096) {
            char* pchBuffer = NEW char[iNeedSize+1];
            memset(pchBuffer, 0, (iNeedSize+1)*sizeof(char));
            WideCharToMultiByte(code, 0, pwText, -1, pchBuffer, iNeedSize+1, nullptr, nullptr);

            string strResult(pchBuffer);
            delete []pchBuffer;
            return strResult;
        } else {
            //assert(pwText);
            char pchBuffer[4096] = {'\0'};

            ::WideCharToMultiByte(code, 0, pwText, -1, pchBuffer, sizeof(pchBuffer), nullptr, nullptr);
            return string(pchBuffer);
        }
    }

    bool IsDataSectionNeeded(const wchar_t* pModuleName) {
        if(nullptr == pModuleName) {
            return false;
        }

        string strTemp = wstringToString(pModuleName, CP_ACP);

        char szFileName[MAX_PATH] = { "\0" };

        _tsplitpath_s(strTemp.c_str(), nullptr, 0, nullptr, 0, szFileName, snArraySize(szFileName), nullptr, 0);

        return (0 == _tcsicmp(szFileName, "ntdll")  ? true : false);
    }

    dumper & dumper::getInstance(void) {
        static    dumper    app;
        return app;
    }

    dumper::~dumper(void) {
        if (nullptr != m_previousFilter) {
            SetUnhandledExceptionFilter(m_previousFilter);
            m_previousFilter = nullptr;
        }
    }

    LONG __stdcall dumper::snUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo) {
        dumper::createMinidump(pExceptionInfo);

        exit(pExceptionInfo->ExceptionRecord->ExceptionCode );

        return EXCEPTION_EXECUTE_HANDLER;
    }

    bool CALLBACK dumper::miniDumpCallback(PVOID pParam, const PMINIDUMP_CALLBACK_INPUT pInput, PMINIDUMP_CALLBACK_OUTPUT pOutput) {
        if(pInput == 0 || pOutput == 0) return FALSE;
        switch(pInput->CallbackType) {
        case ModuleCallback:
            if(pOutput->ModuleWriteFlags & ModuleWriteDataSeg)
                if(!IsDataSectionNeeded(pInput->Module.FullPath))
                    pOutput->ModuleWriteFlags &= (~ModuleWriteDataSeg);
            // fall through
        case IncludeModuleCallback:
        case IncludeThreadCallback:
        case ThreadCallback:
        case ThreadExCallback:
            return TRUE;
        default:;
        }
        return FALSE;
    }

    void dumper::createMinidump(EXCEPTION_POINTERS* pep) {
        HANDLE hFile = CreateFile(m_acDumpName,
            GENERIC_READ | GENERIC_WRITE,
            0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

        if((hFile != nullptr) && (hFile != INVALID_HANDLE_VALUE)) {
            MINIDUMP_EXCEPTION_INFORMATION mdei;
            mdei.ThreadId           = GetCurrentThreadId();
            mdei.ExceptionPointers  = pep;
            mdei.ClientPointers     = FALSE;
            MINIDUMP_CALLBACK_INFORMATION mci;
            mci.CallbackRoutine     = (MINIDUMP_CALLBACK_ROUTINE)(dumper::miniDumpCallback);
            mci.CallbackParam       = 0;

            MINIDUMP_TYPE mdt = MiniDumpNormal;

            BOOL bOK = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
                hFile, mdt, (pep != 0) ? &mdei : 0, 0, &mci);

            CloseHandle(hFile);
        }
    }

    dumper::dumper(void) {
        ::GetModuleFileName(0, m_acDumpName, snArraySize(m_acDumpName));

        bool    bModified = false;
        TCHAR * pszDot = snFindLastOfChar(m_acDumpName, _T("."));
        if (pszDot) {
            if (_tcslen(pszDot) >= 3) {
                bModified = true;
                _tcscpy_s(pszDot, sizeof(_T("dmp"))/sizeof(TCHAR) +1, _T("dmp"));
            }
        }

        if (!bModified) {
            _tcscat_s(m_acDumpName, _T(".dmp"));
        }

        m_previousFilter = ::SetUnhandledExceptionFilter(dumper::snUnhandledExceptionFilter);
    }

    void dumper::setDumpFilename(const TCHAR * dumpName) {
        if (nullptr != dumpName) {
            _tcscpy_s(m_acDumpName, snArraySize(m_acDumpName), dumpName);
        }
    }
}
