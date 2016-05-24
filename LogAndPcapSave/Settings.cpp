#include "stdafx.h"

#include <string>

#include "dbgprint.h"
#include "UnicodeConv.h"

#include "Settings.h"

using namespace std;


CSettings::CSettings(void)
	: version(L"0.8.4.0 - Build 2015-06-11")
	, fname(L"out")
	, find(L"")
	, logfile_interval(L"day")
	, nic(1)
	, pcapmax(100)
	, list_nics(false)
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
			list_nics = true;
		}
		if (_tcsicmp(szArglist[i], _T("-i") ) == 0 ) // use interface number
		{
			if (_tcslen(szArglist[++i]) > 0)
			{
				nic = _ttoi(szArglist[i]);
			}
		}
		if (_tcsicmp(szArglist[i], _T("-o")) == 0) // output filename
		{
			if (_tcslen(szArglist[++i]) > 0)
			{
				fname = UC.ts2ws(szArglist[i]);
			}
		}
		if (_tcsicmp(szArglist[i], _T("-oi")) == 0) // new log interval
		{
			if (_tcslen(szArglist[++i]) > 0)
			{
				logfile_interval = UC.ts2ws(szArglist[i]);
			}
		}
		if (_tcsicmp(szArglist[i], _T("-f")) == 0) // string to find
		{
			if (_tcslen(szArglist[++i]) > 0)
			{
				find = UC.ts2ws(szArglist[i]);
			}
		}
		if (_tcsicmp(szArglist[i], _T("-pcapmax")) == 0) // string to find
		{
			if (_tcslen(szArglist[++i]) > 0)
			{
				pcapmax = _ttoi(szArglist[i]);
			}
		}
		

		if ( 
			_tcsicmp(szArglist[i], _T("-?")) == 0 ||
			_tcsicmp(szArglist[i], _T("/?")) == 0 ||
			_tcsicmp(szArglist[i], _T("-h")) == 0 ||
			_tcsicmp(szArglist[i], _T("/h")) == 0
			) // display help message
		{
			TCHAR szUsage[2048];

			_tcscpy_s(szUsage, _T("\n--- Help: ---\n"));
			_tcscat_s(szUsage, _T("-l :: List all NICs\n"));
			_tcscat_s(szUsage, _T("-i <num> :: number of NIC to be captured, default = 1\n"));
			_tcscat_s(szUsage, _T("-o <string> :: prefix for output files, default = out\n"));
			_tcscat_s(szUsage, _T("-oi <string> :: interval for log file, values = none, hour, day, default = day\n"));
			_tcscat_s(szUsage, _T("-f <string> :: string to search (if empty only debug output to file, dump will be deleted), default = \"\"\n"));
			_tcscat_s(szUsage, _T("-pcapmax <int> :: max size of pcap file [in MB], default = 100\n"));
			_tcscat_s(szUsage, _T("\n"));

			_tcscat_s(szUsage, _T("Usage example: LogVsPcapTracer -f error \n"));
			_tcscat_s(szUsage, _T("\n"));
			
			//MessageBox(NULL, szUsage, _T("Usage..."), MB_OK);
			_tprintf(szUsage);

			// if we reached this then we can exit the application gracefully
			exit(0);
		}

	} // end - while

	return 0;
}

wstring CSettings::GetVersion(void)
{
	return version;
}

wstring CSettings::GetFilename(void)
{
	return fname;
}

wstring CSettings::GetFind(void)
{
	return find;
}

wstring CSettings::GetLogInterval(void)
{
	return logfile_interval;
}

int CSettings::GetNic(void)
{
	return nic;
}

bool CSettings::GetListNics(void)
{
	return list_nics;
}

int CSettings::GetPcapMax(void)
{
	return pcapmax;
}