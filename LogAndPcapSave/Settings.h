#pragma once

#include <string>

class settings
{
public:
	settings(void);
	~settings(void);

	int processCmdLineArgs(TCHAR** szArglist, int iArgs);
	int loadIniFile(void);

	std::string getVersion(void);
	std::string getFilename(void);
	std::string getFind(void);
	std::string getLogInterval(void);
	std::vector<std::string> getProcRunningList(void);
	int getNicToUse(void);
	int getNicCount(void);
	bool getNicIsSet(void);
	int getPcapMax(void);

private:
	std::string find;
	std::string logfileInterval;
	std::string fname;
	std::string version;

	int nicToUse;
	int nicCount;
	bool nicIsSet;
	int pcapMaxSize;

	void printNicList(void);
};

