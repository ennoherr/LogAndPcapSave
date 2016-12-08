#include "stdafx.h"

#include "UniConvert.h"

#include "dbgprint.h"
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
#include <sys/statvfs.h>
#define GetCurrentDir getcwd
#endif

#if UNICODE
#define tstring std::wstring
#else
#define tstring std::string
#endif


#include "HddMgmt.h"

// based on the following article with minor changes
// http://www.codeproject.com/Articles/3921/Disk-free-space


hddMgmt::hddMgmt(void)
#ifdef _WIN32
	// see below
#else
        : freeBytes_(0)
        , totalBytes_(0)
        , totalFreeBytes_(0)
#endif
{
#ifdef _WIN32
	// bytes available to caller
	m_uliFreeBytesAvailable.QuadPart = 0L;
	// bytes on disk
	m_uliTotalNumberOfBytes.QuadPart = 0L;
	// free bytes on disk
	m_uliTotalNumberOfFreeBytes.QuadPart = 0L;
#endif // _WIN32
}


hddMgmt::~hddMgmt(void)
{
}

bool hddMgmt::readHddDiskSpace(std::string hddPath)
{
	UniConvert conv;
	tstring hddToCheck = conv.s2ts(hddPath);

	if (hddToCheck.length() == 0)
	{
		TCHAR path[FILENAME_MAX];
		if (!GetCurrentDir(path, FILENAME_MAX))
		{
			return false;
		}

		hddToCheck = path;
	}

#ifdef _WIN32
	if(!GetDiskFreeSpaceExW(
		hddToCheck.c_str(),				// directory name
		&m_uliFreeBytesAvailable,		// bytes available to caller
		&m_uliTotalNumberOfBytes,		// bytes on disk
		&m_uliTotalNumberOfFreeBytes))	// free bytes on disk
	{
		return false;
	}
#else
        struct statvfs fs;
        if (statvfs(hddToCheck.c_str(), &fs) < 0)
        {
            return false;
        }
        
        totalBytes_ = fs.f_bsize * fs.f_blocks;
        freeBytes_ = fs.f_bsize * fs.f_bavail;
        totalFreeBytes_ = fs.f_bsize * fs.f_bfree;

#ifdef _DEBUG
        //dbgtprintf(_T("totalBytes = %ld, freeBytes = %ld, totalFreeBytes = %ld"), totalBytes_, freeBytes_, totalFreeBytes_);
        //dbgtprintf(_T("totalGBytes = %4.2f, freeGBytes = %4.2f, totalFreeGBytes = %4.2f"), getTotalNumberOfGBytes(), getFreeGBytesAvailable(), getTotalNumberOfFreeGBytes());
#endif       
        
#endif

	return true;
}

__int64 hddMgmt::getFreeBytesAvailable(void)
{ 
#if _WIN32
	return m_uliFreeBytesAvailable.QuadPart;
#else
        return freeBytes_;
#endif

}

__int64 hddMgmt::getTotalNumberOfBytes(void)
{ 
#if _WIN32
        return m_uliTotalNumberOfBytes.QuadPart;
#else
        return totalBytes_;
#endif
	
}

__int64 hddMgmt::getTotalNumberOfFreeBytes(void)
{ 
#if _WIN32
	return m_uliTotalNumberOfFreeBytes.QuadPart;
#else
        return totalFreeBytes_;
#endif

}

__int64 hddMgmt::getFreeMBytesAvailable(void)
{
	return getFreeBytesAvailable() / (1024 * 1024);
}

__int64 hddMgmt::getTotalNumberOfMBytes(void)
{
	return getTotalNumberOfBytes() / (1024 * 1024);
}

__int64 hddMgmt::getTotalNumberOfFreeMBytes(void)
{
	return getTotalNumberOfFreeBytes() / (1024 * 1024);
}

double hddMgmt::getFreeGBytesAvailable(void)
{ 
	return (double)( (signed __int64)(getFreeBytesAvailable())/1.0e9 );
}

double hddMgmt::getTotalNumberOfGBytes(void)
{ 
	return (double)( (signed __int64)(getTotalNumberOfBytes())/1.0e9 );     
}

double hddMgmt::getTotalNumberOfFreeGBytes(void)
{ 
	return (double)( (signed __int64)(getTotalNumberOfFreeBytes())/1.0e9 ); 
}


