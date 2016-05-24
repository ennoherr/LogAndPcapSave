#include "stdafx.h"

#include <string>
#include <iostream>

#include "dbgprint.h"
#include "UnicodeConv.h"
#include "NetCapture.h"

#include "Settings.h"

using namespace std;


CSettings::CSettings(void)
	: version("1.0.0.0 - Build 2016-05-24")
	, fname("out")
	, find("")
	, logfileInterval("day")
	, nicToUse(1)
	, nicCount(1)
	, pcapMaxSize(100)
{
}


CSettings::~CSettings(void)
{
}

int CSettings::ProcessCmdLineArgs(TCHAR** szArglist, int iArgs)
{
	if(!szArglist)
	{
		return -1;
	}

	int i = 0;
	CUnicodeConv UC;

	for (i = 0; i < iArgs; i++)
	{
		dbgtprintf(_T("argv[%i] = %s"), i, szArglist[i]);
	}

	for (i = 0; i < iArgs; i++)
	{
		if (_tcsicmp(szArglist[i], _T("-l")) == 0) // list interfaces
		{
			PrintNicList();
			
			// we can exit the loop here
			break;
		}
		if (_tcsicmp(szArglist[i], _T("-i") ) == 0 ) // use interface number
		{
			if (_tcslen(szArglist[++i]) > 0)
			{
				nicToUse = _ttoi(szArglist[i]);
			}
		}
		if (_tcsicmp(szArglist[i], _T("-o")) == 0) // output filename
		{
			if (_tcslen(szArglist[++i]) > 0)
			{
				fname = UC.ts2s(szArglist[i]);
			}
		}
		if (_tcsicmp(szArglist[i], _T("-oi")) == 0) // new log interval
		{
			if (_tcslen(szArglist[++i]) > 0)
			{
				logfileInterval = UC.ts2s(szArglist[i]);
			}
		}
		if (_tcsicmp(szArglist[i], _T("-f")) == 0) // string to find
		{
			if (_tcslen(szArglist[++i]) > 0)
			{
				find = UC.ts2s(szArglist[i]);
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

string CSettings::GetVersion(void)
{
	return version;
}

string CSettings::GetFilename(void)
{
	return fname;
}

string CSettings::GetFind(void)
{
	return find;
}

string CSettings::GetLogInterval(void)
{
	return logfileInterval;
}

std::vector<std::string> CSettings::GetProcRunningList(void)
{
	// todo: find a better way than hardcode it
	std::vector<std::string> proc;
	proc.push_back("DbgView.exe");

	return proc;
}

int CSettings::GetNicToUse(void)
{
	return nicToUse;
}

int CSettings::GetNicCount(void)
{
	CNetCapture NC;
	return NC.GetInterfacesCount();
}


void CSettings::PrintNicList(void)
{
	std::vector<std::string> adapters;
	CNetCapture NC;
	NC.GetInterfaces(adapters);
	
	for (unsigned i = 0; i < adapters.size(); i++)
	{
		cout << to_string(i + 1) << " - " << adapters.at(i) << endl;
	}
}

int CSettings::GetPcapMax(void)
{
	return pcapMaxSize;
}

