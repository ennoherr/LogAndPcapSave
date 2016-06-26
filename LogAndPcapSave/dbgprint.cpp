/*+===================================================================
  File:      dbgprint.cpp

  Summary:   contains funtions to write output to the debugger.

  Classes:   no classes.

===================================================================+*/

// maybe some includes necessary

#include "stdafx.h"
#include <time.h>
#include "dbgprint.h"


#ifdef _WIN32
HANDLE g_hMutex;
#else
        // todo
#endif


//-----------------------------------------------------------------------------------
// output debug via printf
//
void InitDebugOut(void)
{
#ifdef _WIN32
	g_hMutex = CreateMutex(NULL,		// default security attributes
						  FALSE,	// initially not owned
						  NULL);	// unnamed mutex
  
	if (g_hMutex == NULL) 
        {
		printf("CreateMutex error: %d\n", GetLastError());
        }
#else
        // todo
#endif
}

void DeInitDebugOut(void)
{
#ifdef _WIN32
	CloseHandle(g_hMutex);
#else
        // todo
#endif
}

//-----------------------------------------------------------------------------------
// output debug via outputdebugstring
//
void dbgtprintf(LPCTSTR format, ...)
{
#ifdef _WIN32
	if (!format)
	{
		return;
	}

	const int iLen = 1024;

	if (_tcslen(format) > (iLen/2))
	{
		return;
	}

	int nSize = 0;
	TCHAR szBuffer[iLen] = _T("\0");
	va_list args;
        		
	// get input and make an output string
	try
	{
		va_start(args, format);
		nSize = _vsntprintf_s(szBuffer, iLen-5, format, args);
		va_end(args);

		// on error - nSize should be length of string
		if(nSize <= 0)
			_tcscpy_s(szBuffer, _T("ERROR: Unable to process args for debug information!"));

		_tcscat_s(szBuffer, _T("\r\n\0"));

		OutputDebugString(szBuffer);
	}
	catch(...)
	{
		OutputDebugString(_T("ERROR: Exception while processing debug information!"));
	}
#else
        // todo
#endif
}

void dbgprintf(const char *format, ...)
{
#ifdef _WIN32
	if(!format)
	{
		return;
	}

	const int iLen = 1024;

	if (strlen(format) > (iLen/2))
	{
		return;
	}
        
	int nSize = 0;
	char szBuffer[iLen] = "\0";
	va_list args;

	// get input and make an output string
	try
	{
		va_start(args, format);
		nSize = _vsnprintf_s(szBuffer, iLen-5, format, args);
		va_end(args);

		// on error - nSize should be length of string
		if(nSize <= 0)
			strcpy_s(szBuffer, "ERROR: Unable to process args for debug information!");
	
		strcat_s(szBuffer, "\r\n\0");

		OutputDebugStringA(szBuffer);
	}
	catch(...)
	{
		OutputDebugStringA("ERROR: Exception while processing debug information!");
	}
#else
        // todo
#endif
}

void dbgwprintf(const wchar_t *format, ...)
{
#ifdef _WIN32
	if(!format)
	{
		return;
	}

	const int iLen = 1024;

	if (wcslen(format) > (iLen/2))
	{
		return;
	}

	int nSize = 0;
	wchar_t szBuffer[iLen] = L"\0";
	va_list args;

	// get input and make an output string
	try
	{
		va_start(args, format);
		nSize = _vsnwprintf_s(szBuffer, iLen-5, format, args);
		va_end(args);

		// on error - nSize should be length of string
		if(nSize <= 0)
			wcscpy_s(szBuffer, L"ERROR: Unable to process args for debug information!");
	
		wcscat_s(szBuffer, L"\r\n\0");

		OutputDebugStringW(szBuffer);
	}
	catch(...)
	{
		OutputDebugStringW(L"ERROR: Exception while processing debug information!");
	}
#else
        // todo
#endif
}


//-----------------------------------------------------------------------------------
// wrapper for output via outputdebugstring and via printf
//
void DebugInfo(const char* szInfo, bool bTime, bool bDebug, bool bPrint)
{
#ifdef _WIN32
	if(!g_hMutex)
		return;

	if(WaitForSingleObject(g_hMutex, INFINITE) == WAIT_OBJECT_0)
	{
		// http://support.microsoft.com/kb/315937/en-us
		__try
		{
			if(!szInfo)
				return;

			char szTime[128] = "\0";
			if(bTime)
				GetDateTimeNow(szTime);

			if(bDebug)
				dbgprintf(szInfo);

			if(bPrint)
			{
				if(strlen(szTime) == 0) printf("%s\n", szInfo);
				else printf("[%s] %s\n", szTime, szInfo);
			}

		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			LPBYTE lpPage;
			static SYSTEM_INFO si;
			static MEMORY_BASIC_INFORMATION mi;
			static DWORD dwOldProtect;

			// Get page size of system
			GetSystemInfo(&si);

			// Find SP address
			_asm mov lpPage, esp;

			// Get allocation base of stack
			VirtualQuery(lpPage, &mi, sizeof(mi));

			// Go to page beyond current page
			lpPage = (LPBYTE)(mi.BaseAddress)-si.dwPageSize;

			// Free portion of stack just abandoned
			if (!VirtualFree(mi.AllocationBase,
				(LPBYTE)lpPage - (LPBYTE)mi.AllocationBase,
				MEM_DECOMMIT))
			{
				// If we get here, exit
				return;
			}

			// Reintroduce the guard page
			if (!VirtualProtect(lpPage, si.dwPageSize, 
				PAGE_GUARD | PAGE_READWRITE, 
				&dwOldProtect))
			{
				return;
			}
			printf("Exception handler %lX\n", _exception_code()); 
		}

		ReleaseMutex(g_hMutex);
	} // end if - mutex
#else
        // todo
#endif
}

void DebugInfoT(const TCHAR* szInfo, bool bTime, bool bDebug, bool bPrint)
{
#ifdef _WIN32
	if(!g_hMutex)
		return;

	if(WaitForSingleObject(g_hMutex, INFINITE) == WAIT_OBJECT_0)
	{
		// http://support.microsoft.com/kb/315937/en-us
		__try
		{
			if(!szInfo)
				return;

			TCHAR szTime[128] = _T("\0");
			if(bTime)
				GetDateTimeNowT(szTime);

			if(bDebug)
				dbgtprintf(szInfo);

			if(bPrint)
			{
				if(_tcslen(szTime) == 0) _tprintf(_T("%s\n"), szInfo);
				else _tprintf(_T("[%s] %s\n"), szTime, szInfo);
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			LPBYTE lpPage;
			static SYSTEM_INFO si;
			static MEMORY_BASIC_INFORMATION mi;
			static DWORD dwOldProtect;

			// Get page size of system
			GetSystemInfo(&si);

			// Find SP address
			_asm mov lpPage, esp;

			// Get allocation base of stack
			VirtualQuery(lpPage, &mi, sizeof(mi));

			// Go to page beyond current page
			lpPage = (LPBYTE)(mi.BaseAddress)-si.dwPageSize;

			// Free portion of stack just abandoned
			if (!VirtualFree(mi.AllocationBase,
				(LPBYTE)lpPage - (LPBYTE)mi.AllocationBase,
				MEM_DECOMMIT))
			{
				// If we get here, exit
				return;
			}

			// Reintroduce the guard page
			if (!VirtualProtect(lpPage, si.dwPageSize, 
				PAGE_GUARD | PAGE_READWRITE, 
				&dwOldProtect))
			{
				return;
			}
			printf("Exception handler %lX\n", _exception_code()); 
		}

		ReleaseMutex(g_hMutex);
	} // end if - mutex
#else
        // todo
#endif
}

//-----------------------------------------------------------------------------------
// get current date and time
//
void GetDateTimeNow(char* buffer, bool bDate, bool bTime) 
{
#ifdef _WIN32
	time_t rawtime;
	struct tm timeinfo;
	errno_t err;
	const int rSize = 128 * sizeof(char); // maybe give this as arg
	char szFormat[rSize];

	time(&rawtime);
	//timeinfo = localtime( &rawtime );
	if((err = localtime_s(&timeinfo, &rawtime)) > 0)
		return;
	
	// formatting
	if(bDate && bTime)
		strcpy_s(szFormat, "%Y-%m-%d %H:%M:%S");
	else if(bDate && !bTime)
		strcpy_s(szFormat, "%Y-%m-%d");
	else if(!bDate && bTime)
		strcpy_s(szFormat, "%H:%M:%S");
	else
		return;

	strftime(buffer, rSize, szFormat, &timeinfo);
#else
        // todo
#endif
}

void GetDateTimeNowT(TCHAR* buffer, bool bDate, bool bTime)
{
#ifdef _WIN32
	time_t rawtime;
	struct tm timeinfo;
	errno_t err;
	const int rSize = 128 * sizeof(char); // maybe give this as arg
	TCHAR szFormat[rSize];

	time( &rawtime );
	//timeinfo = localtime( &rawtime );
	if((err = localtime_s(&timeinfo, &rawtime)) > 0)
		return;

	// formatting
	if(bDate && bTime)
		_tcscpy_s(szFormat, _T("%Y-%m-%d %H:%M:%S"));
	else if(bDate && !bTime)
		_tcscpy_s(szFormat, _T("%Y-%m-%d"));
	else if(!bDate && bTime)
		_tcscpy_s(szFormat, _T("%H:%M:%S"));
	else
		return;
	
	_tcsftime(buffer, rSize, szFormat, &timeinfo);
#else
        // todo
#endif
}

