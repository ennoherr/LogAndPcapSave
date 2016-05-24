#pragma once

#include <string>
#include "TimeInfo.h"

using namespace std;

class CFiles
{
public:
	CFiles(CTimeInfo *pTI);
	~CFiles(void);

	int WriteToFile(wstring fname, wstring line, wstring interval = L"none", bool bAppend = true);

private:
	CTimeInfo *m_pTI;
	wstring m_Logfile;
};

