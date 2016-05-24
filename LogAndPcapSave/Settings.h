#pragma once

#include <string>

class CSettings
{
public:
	CSettings(void);
	~CSettings(void);

	int ProcessCmdLineArgs(TCHAR** szArglist, int iArgs);

	std::string GetVersion(void);
	std::string GetFilename(void);
	std::string GetFind(void);
	std::string GetLogInterval(void);
	std::vector<std::string> GetProcRunningList(void);
	int GetNicToUse(void);
	int GetNicCount(void);
	int GetPcapMax(void);

private:
	std::string find;
	std::string logfileInterval;
	std::string fname;
	std::string version;

	int nicToUse;
	int nicCount;
	int pcapMaxSize;

	void PrintNicList(void);
};

