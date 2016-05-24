#pragma once

#include <string>

using namespace std;

class CSettings
{
public:
	CSettings(void);
	~CSettings(void);

	int ProcessCmdLineArgs(TCHAR** szArglist, int iArgs);


	wstring GetVersion(void);
	wstring GetFilename(void);
	wstring GetFind(void);
	wstring GetLogInterval(void);
	int GetNic(void);
	int GetPcapMax(void);
	bool GetListNics(void);

private:
	wstring version;
	wstring fname;
	wstring find;
	wstring logfile_interval;

	int nic;
	int pcapmax;

	bool list_nics;

	
};

