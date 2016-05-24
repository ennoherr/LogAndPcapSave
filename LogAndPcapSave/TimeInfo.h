#pragma once

#include <string>

using namespace std;

class CTimeInfo
{
public:
	CTimeInfo(void);
	~CTimeInfo(void);

	unsigned long GetTimestamp(void);
	long long GetTimestampMs(void);
	
	wstring GetTimeReadable(wstring DateTimeSep = L";", wstring TimeSep = L":");
	wstring GetTimeReadableMs(wstring DateTimeSep = L";", wstring TimeSep = L":", wstring MSecSep = L".");

	bool IsNewDay(void);
	bool IsNewHour(void);

private:
	time_t m_tSavedDay;
	time_t m_tSavedHour;
};

