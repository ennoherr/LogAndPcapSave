#include "stdafx.h"

#include <algorithm>
#include <string>
#include <fstream>

#include "dbgprint.h"
#include "UnicodeConv.h"
#include "TimeInfo.h"

#include "Files.h"

using namespace std;

CFiles::CFiles(CTimeInfo *pTI)
	: m_pTI(NULL)
	, m_Logfile(L"")
{
	if (pTI) m_pTI = pTI;
}


CFiles::~CFiles(void)
{
}

int CFiles::WriteToFile(wstring fname, wstring line, wstring interval, bool bAppend)
{
	if (fname.length() == 0 || line.length() == 0)
	{
		return -1;
	}

	int iRet = 0;
	bool bNewFile = false;
	transform(interval.begin(), interval.end(), interval.begin(), ::toupper);

	if (interval == L"NONE")
	{
		bNewFile = false;
	}
	if (interval == L"HOUR")
	{
		bNewFile = m_pTI->IsNewHour();
	}
	if (interval == L"DAY")
	{
		bNewFile = m_pTI->IsNewDay();
	}
	
	// necessary to create a new file
	if (m_Logfile.length() == 0 || bNewFile)
	{
		//m_Logfile = fname + L"_" + to_wstring(m_pTI->GetTimestampMs()) + L"_" + m_pTI->GetTimeReadableMs(L"_", L"-", L"-") + L".log";
		m_Logfile = fname + L"_" + m_pTI->GetTimeReadableMs(L"_", L"-", L"-") + L".log";
	}
	
	ios_base::openmode om = (bAppend) ? (ios_base::out | ios_base::app) : (ios_base::out);
	wofstream ofs(m_Logfile, om);

	if (ofs.is_open())
	{
		ofs << line << endl;
		ofs.close();
	}
	else
	{
		CUnicodeConv UC;
		dbgtprintf(_T("CFiles::WriteToFile ERROR: Unable to open file \'%s\'"), UC.ws2ts(fname).c_str());
		iRet = 1;
	}

	return iRet;
}