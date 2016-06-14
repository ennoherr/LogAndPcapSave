// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#else
#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>

#endif

// reference additional headers your program requires here


#ifndef STRING_LENGTH
	#define STRING_LENGTH 1024
#endif

#ifndef SAFE_DELETE
	#define SAFE_DELETE(p) {if (p) {delete(p); p = NULL;}}
#endif

#ifndef SAFE_HANDLE
#ifdef _WIN32
	#define SAFE_HANDLE(p) {if (p) {CloseHandle(p); p = NULL;}}
#else
        #define SAFE_HANDLE(p) {}
#endif
#endif



#ifdef _WIN32

#else
#define INT int
#define __int64 int64_t
#define DWORD unsigned long
#define ULONGLONG unsigned long long

#if defined(MIDL_PASS)
typedef struct _ULARGE_INTEGER {
#else // MIDL_PASS
typedef union _ULARGE_INTEGER {
    struct {
        DWORD LowPart;
        DWORD HighPart;
    } DUMMYSTRUCTNAME;
    struct {
        DWORD LowPart;
        DWORD HighPart;
    } u;
#endif //MIDL_PASS
    ULONGLONG QuadPart;
} ULARGE_INTEGER;

#define BYTE int

#define WCHAR wchar_t
#define CHAR char
#define _TCHAR TCHAR

#define HANDLE int

#define _kbhit kbhit
#define GetLastError() -1
#define Sleep usleep

#ifdef UNICODE
#define _T (const wchar_t*)
#define TCHAR wchar_t
#define LPCTSTR const wchar_t*
#else
#define _T (const char*)
#define TCHAR char
#define LPCTSTR const char*

#endif
#endif