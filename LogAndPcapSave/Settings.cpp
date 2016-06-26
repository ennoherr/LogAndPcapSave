#include "stdafx.h"

#include <string>
#include <iostream>

#include "dbgprint.h"
#include "UniConvert.h"
#include "NetCapture.h"

#include "Settings.h"

using namespace std;


settings::settings(void)
	: version("1.0.0.4 - Build 2016-06-17")
	, fname("out")
	, find("")
	, logfileInterval("day")
	, nicToUse(1)
	, nicCount(1)
	, pcapMaxSize(100)
{
}


settings::~settings(void)
{
}

int settings::processCmdLineArgs(TCHAR** szArglist, int iArgs)
{
	if(!szArglist)
	{
		return -1;
	}

	int i = 0;
	UniConvert uc;

	for (i = 0; i < iArgs; i++)
	{
		dbgtprintf(_T("argv[%i] = %s"), i, szArglist[i]);
	}

	for (i = 0; i < iArgs; i++)
	{
		if (_tcsicmp(szArglist[i], _T("-l")) == 0) // list interfaces
		{
			printNicList();
			
			// we can exit the loop here
			break;
		}
		if (_tcsicmp(szArglist[i], _T("-i") ) == 0 ) // use interface number
		{
			if (_tcslen(szArglist[++i]) > 0)
			{
				nicToUse = _ttoi(szArglist[i]);
				nicIsSet = true;
			}
		}
		if (_tcsicmp(szArglist[i], _T("-o")) == 0) // output filename
		{
			if (_tcslen(szArglist[++i]) > 0)
			{
				fname = uc.ts2s(szArglist[i]);
			}
		}
		if (_tcsicmp(szArglist[i], _T("-oi")) == 0) // new log interval
		{
			if (_tcslen(szArglist[++i]) > 0)
			{
				logfileInterval = uc.ts2s(szArglist[i]);
			}
		}
		if (_tcsicmp(szArglist[i], _T("-f")) == 0) // string to find
		{
			if (_tcslen(szArglist[++i]) > 0)
			{
				find = uc.ts2s(szArglist[i]);
			}
		}
		if (_tcsicmp(szArglist[i], _T("-pcapmax")) == 0) // string to find
		{
			if (_tcslen(szArglist[++i]) > 0)
			{
				pcapMaxSize = _ttoi(szArglist[i]);
			}
		}
		

		if ( 
			_tcsicmp(szArglist[i], _T("-?")) == 0 ||
			_tcsicmp(szArglist[i], _T("/?")) == 0 ||
			_tcsicmp(szArglist[i], _T("-h")) == 0 ||
			_tcsicmp(szArglist[i], _T("/h")) == 0
			) // display help message
		{
			cout << "Usage: " << endl;
			cout << "-l :: List all NICs" << endl;
			cout << "-i <num> :: number of NIC to be captured, default = 1" << endl;
			cout << "-o <string> :: prefix for output files, default = out" << endl;
			cout << "-oi <string> :: interval for log file, values = none, hour, day, default = day" << endl;
			cout << "-f <string> :: string to search (if empty only debug output to file, dump will be deleted), default = \"\"" << endl;
			cout << "-pcapmax <int> :: max size of pcap file [in MB], default = 100" << endl;
			cout << endl;

			cout << "Usage example: LogVsPcapTracer -f error" << endl;
			cout << endl;
			
			// we can exit the loop here
			break;
		}

	} // end - for loop

	return 0;
}

int settings::loadIniFile(void)
{
	int res = 0;
	
	// nothing todo yet

	return res;
}

string settings::getVersion(void)
{
	return version;
}

string settings::getFilename(void)
{
	return fname;
}

string settings::getFind(void)
{
	return find;
}

string settings::getLogInterval(void)
{
	return logfileInterval;
}

std::vector<std::string> settings::getProcRunningList(void)
{
	// todo: find a better way than hardcode it
	std::vector<std::string> proc;
	proc.push_back("DbgView.exe");

	return proc;
}

int settings::getNicToUse(void)
{
	return nicToUse;
}

int settings::getNicCount(void)
{
	NetCapture nc;
	return nc.getInterfacesCount();
}

bool settings::getNicIsSet(void)
{
	return nicIsSet;
}


void settings::printNicList(void)
{
	std::vector<std::string> adapters;
	NetCapture nc;
	nc.getInterfaces(adapters);
	
	for (unsigned i = 0; i < adapters.size(); i++)
	{
		cout << to_string(i + 1) << " - " << adapters.at(i) << endl;
	}
}

int settings::getPcapMax(void)
{
	return pcapMaxSize;
}

