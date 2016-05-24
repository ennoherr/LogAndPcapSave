#include "stdafx.h"

#include <sys/timeb.h>
#include <string>
#include <ctime>
#include <chrono>

#include "TimeInfo.h"



CTimeInfo::CTimeInfo(void)
	: m_tSavedDay(NULL)
	, m_tSavedHour(NULL)
{
}


CTimeInfo::~CTimeInfo(void)
{
}

unsigned long CTimeInfo::GetTimestamp(void)
{
	unsigned long ulRet = 0L;

	time_t t = time(0);
	ulRet = (unsigned long) t;

	return ulRet;
}

long long CTimeInfo::GetTimestampMs(void)
{
	// alterantiv ohne chrono
    //struct timeval tp;
    //gettimeofday(&tp, NULL);
    //long long mslong = (long long) tp.tv_sec * 1000L + tp.tv_usec / 1000; //get current timestamp in milliseconds
	
	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	return ms.count();
}

std::string CTimeInfo::GetTimeReadable(std::string DateTimeSep, std::string TimeSep)
{
	std::string sRet = "";

	const int l = 64;

	//wchar_t wc_date[l] = L"\0";
	//wchar_t wc_time_hour[l] = L"\0";
	//wchar_t wc_time_min[l] = L"\0";
	//wchar_t wc_time_sec[l] = L"\0";
	//time_t t = time(0);
	//tm tm = { 0 };
	//localtime_s(&tm, &t);
	////wcsftime(wc, l-1, L"%Y-%m-%d;%H:%M:%S", &tm);
	//wcsftime(wc_date, l-1, L"%Y-%m-%d", &tm);
	//wcsftime(wc_time_hour, l-1, L"%H", &tm);
	//wcsftime(wc_time_min, l-1, L"%M", &tm);
	//wcsftime(wc_time_sec, l-1, L"%S", &tm);
	//
	//sRet = wc_date + DateTimeSep + wc_time_hour + TimeSep + wc_time_min + TimeSep + wc_time_sec;

	char date[l] = "";
	char time_hour[l] = "";
	char time_min[l] = "";
	char time_sec[l] = "";
	time_t t = time(0);
	tm tm = { 0 };
	localtime_s(&tm, &t);
	//wcsftime(wc, l-1, L"%Y-%m-%d;%H:%M:%S", &tm);
	strftime(date, l - 1, "%Y-%m-%d", &tm);
	strftime(time_hour, l - 1, "%H", &tm);
	strftime(time_min, l - 1, "%M", &tm);
	strftime(time_sec, l - 1, "%S", &tm);

	sRet = date + DateTimeSep + time_hour + TimeSep + time_min + TimeSep + time_sec;
	
	return sRet;
}

std::string CTimeInfo::GetTimeReadableMs(std::string DateTimeSep, std::string TimeSep, std::string MSecSep)
{
	std::string sRet = "";
	std::string sMSec = "";
	struct _timeb t = { 0 };
	
	_ftime_s(&t);

	sMSec = std::to_string(t.millitm);
	if (sMSec.length() == 1) sMSec = "00" + sMSec;
	if (sMSec.length() == 2) sMSec = "0" + sMSec;

	sRet = GetTimeReadable(DateTimeSep, TimeSep) + MSecSep + sMSec;

	return sRet;
}

bool CTimeInfo::IsNewDay(void)
{
	bool bRet = false;

	// init
	if (m_tSavedDay == NULL)
	{
		m_tSavedDay = time(0);
	}

	time_t tNow = time(0);
	tm tmSave = { 0 };
	tm tmNow = { 0 };

	localtime_s(&tmSave, &m_tSavedDay);
	localtime_s(&tmNow, &tNow);
	int iSaved = tmSave.tm_yday;
	int iNow = tmNow.tm_yday;

	m_tSavedDay = tNow;

	if (iSaved != iNow)
	{
		bRet = true;
	}

	return bRet;
}

bool CTimeInfo::IsNewHour(void)
{
	bool bRet = false;

	// init
	if (m_tSavedHour == NULL)
	{
		m_tSavedHour = time(0);
	}

	time_t tNow = time(0);
	tm tmSave = { 0 };
	tm tmNow = { 0 };

	localtime_s(&tmSave, &m_tSavedHour);
	localtime_s(&tmNow, &tNow);

	int iSaved = tmSave.tm_hour;
	int iNow = tmNow.tm_hour;

	m_tSavedHour = tNow;

	if (iSaved != iNow)
	{
		bRet = true;
	}

	return bRet;
}