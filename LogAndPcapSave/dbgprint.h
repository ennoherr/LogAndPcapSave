#pragma once

#include "stdafx.h"

// init critical section

void InitDebugOut(void);
void DeInitDebugOut(void);

void dbgprintf(const char *format, ...);
void dbgwprintf(const wchar_t *format, ...);
void dbgtprintf(LPCTSTR format, ...);

void GetDateTimeNow(char* buffer, bool bDate = true, bool bTime = true);
void GetDateTimeNowT(TCHAR* buffer, bool bDate = true, bool bTime = true);

void DebugInfo(const char* szInfo, bool bTime = true, bool bDebug = true, bool bPrint = true);
void DebugInfoT(const TCHAR* szInfo, bool bTime = true, bool bDebug = true, bool bPrint = true);




//#define DEBUG_ON
//// Es gibt kein __VA_ARGS__ unter VC++
//// => nach http://www.codeguru.com/forum/archive/index.php/t-318438.html
//
//#ifdef DEBUG_ON
// #define dbgtprintf(format,...) dbgtprintf_func(format, __VA_ARGS__)
// #define dbgprintf(format,...) dbgprintf_func(format, __VA_ARGS__)
//#else
// #define dbgtprintf 
// #define dbgprintf
//#endif