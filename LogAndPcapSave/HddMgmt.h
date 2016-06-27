#pragma once

#include <string>

class hddMgmt
{
public:
	hddMgmt(void);
	~hddMgmt(void);

	bool readHddDiskSpace(std::string hddPath = "");

	__int64 getFreeBytesAvailable(void);
	__int64 getTotalNumberOfBytes(void);
	__int64 getTotalNumberOfFreeBytes(void);

	__int64 getFreeMBytesAvailable(void);
	__int64 getTotalNumberOfMBytes(void);
	__int64 getTotalNumberOfFreeMBytes(void);

	double getFreeGBytesAvailable(void);
	double getTotalNumberOfGBytes(void);
	double getTotalNumberOfFreeGBytes(void);

private:
#ifdef _WIN32
	ULARGE_INTEGER m_uliFreeBytesAvailable;
	ULARGE_INTEGER m_uliTotalNumberOfBytes;
	ULARGE_INTEGER m_uliTotalNumberOfFreeBytes;
#else
        __int64 freeBytes;
        __int64 totalBytes;
        __int64 totalFreeBytes;
#endif

};

