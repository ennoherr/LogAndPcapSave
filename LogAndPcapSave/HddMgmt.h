#pragma once

#include <string>

using namespace std;

class CHddMgmt
{
public:
	CHddMgmt(void);
	~CHddMgmt(void);

	bool ReadHddValues(wstring hddPath = L"");

	__int64 GetFreeBytesAvailable(void);
	__int64 GetTotalNumberOfBytes(void);
	__int64 GetTotalNumberOfFreeBytes(void);

	double GetFreeGBytesAvailable(void);
	double GetTotalNumberOfGBytes(void);
	double GetTotalNumberOfFreeGBytes(void);

private:
	ULARGE_INTEGER m_uliFreeBytesAvailable;
	ULARGE_INTEGER m_uliTotalNumberOfBytes;
	ULARGE_INTEGER m_uliTotalNumberOfFreeBytes;
};

