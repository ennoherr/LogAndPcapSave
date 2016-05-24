#include "stdafx.h"

#include <string>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#ifdef UNICODE
#define GetCurrentDir _wgetcwd
#else
#define GetCurrentDir _getcwd
#endif
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

#include "HddMgmt.h"

// based on the following article with minor changes
// http://www.codeproject.com/Articles/3921/Disk-free-space


CHddMgmt::CHddMgmt(void)
{
	// bytes available to caller
	m_uliFreeBytesAvailable.QuadPart     = 0L;
	// bytes on disk
	m_uliTotalNumberOfBytes.QuadPart     = 0L;
	// free bytes on disk
	m_uliTotalNumberOfFreeBytes.QuadPart = 0L;
}


CHddMgmt::~CHddMgmt(void)
{
}

bool CHddMgmt::ReadHddValues(wstring hddPath)
{
	if (hddPath.length() == 0)
	{
		wchar_t path[FILENAME_MAX] = L"\0";
		if (!GetCurrentDir(path, FILENAME_MAX))
		{
			return false;
		}

		hddPath = path;
	}

	if(!GetDiskFreeSpaceExW(
		hddPath.c_str(),				// directory name
		&m_uliFreeBytesAvailable,		// bytes available to caller
		&m_uliTotalNumberOfBytes,		// bytes on disk
		&m_uliTotalNumberOfFreeBytes))	// free bytes on disk
	{
		return false;
	}

	return true;
}

__int64 CHddMgmt::GetFreeBytesAvailable(void)
{ 
	return m_uliFreeBytesAvailable.QuadPart;
}

__int64 CHddMgmt::GetTotalNumberOfBytes(void)
{ 
	return m_uliTotalNumberOfBytes.QuadPart;
}

__int64 CHddMgmt::GetTotalNumberOfFreeBytes(void)
{ 
	return m_uliTotalNumberOfFreeBytes.QuadPart;
}

double CHddMgmt::GetFreeGBytesAvailable(void)
{ 
	return (double)( (signed __int64)(m_uliFreeBytesAvailable.QuadPart)/1.0e9 );
}

double CHddMgmt::GetTotalNumberOfGBytes(void)
{ 
	return (double)( (signed __int64)(m_uliTotalNumberOfBytes.QuadPart)/1.0e9 );     
}

double CHddMgmt::GetTotalNumberOfFreeGBytes(void)
{ 
	return (double)( (signed __int64)(m_uliTotalNumberOfFreeBytes.QuadPart)/1.0e9 ); 
}


