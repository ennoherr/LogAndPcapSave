#include "stdafx.h"

#include <sys/timeb.h>
#include <string>
#include <ctime>
#include <chrono>

#include "TimeInfo.h"


using namespace std;


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
	
	chrono::milliseconds ms = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
	return ms.count();
}

wstring CTimeInfo::GetTimeReadable(wstring DateTimeSep, wstring TimeSep)
{
	wstring sRet = L"";

	const int l = 64;
	wchar_t wc_date[l] = L"\0";
	wchar_t wc_time_hour[l] = L"\0";
	wchar_t wc_time_min[l] = L"\0";
	wchar_t wc_time_sec[l] = L"\0";
	time_t t = time(0);
	tm tm = { 0 };
	localtime_s(&tm, &t);
	//wcsftime(wc, l-1, L"%Y-%m-%d;%H:%M:%S", &tm);
	wcsftime(wc_date, l-1, L"%Y-%m-%d", &tm);
	wcsftime(wc_time_hour, l-1, L"%H", &tm);
	wcsftime(wc_time_min, l-1, L"%M", &tm);
	wcsftime(wc_time_sec, l-1, L"%S", &tm);

	sRet = wc_date + DateTimeSep + wc_time_hour + TimeSep + wc_time_min + TimeSep + wc_time_sec;
	return sRet;
}

wstring CTimeInfo::GetTimeReadableMs(wstring DateTimeSep, wstring TimeSep, wstring MSecSep)
{
	wstring sRet = L"";
	wstring sMSec = L"";
	struct _timeb t = { 0 };
	
	_ftime_s(&t);

	sMSec = to_wstring(t.millitm);
	if (sMSec.length() == 1) sMSec = L"00" + sMSec;
	if (sMSec.length() == 2) sMSec = L"0" + sMSec;

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