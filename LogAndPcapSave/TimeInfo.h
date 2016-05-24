#pragma once

#include <string>

class CTimeInfo
{
public:
	CTimeInfo(void);
	~CTimeInfo(void);

	unsigned long GetTimestamp(void);
	long long GetTimestampMs(void);
	
	std::string GetTimeReadable(std::string DateTimeSep = ";", std::string TimeSep = ":");
	std::string GetTimeReadableMs(std::string DateTimeSep = ";", std::string TimeSep = ":", std::string MSecSep = ".");

	bool IsNewDay(void);
	bool IsNewHour(void);

private:
	time_t m_tSavedDay;
	time_t m_tSavedHour;
};

