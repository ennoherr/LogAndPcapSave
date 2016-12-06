#include "stdafx.h"

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

#include "dbgprint.h"
#include "UniConvert.h"
#include "NetCapture.h"

#include "Settings.h"


settings::settings(void)
	: version("1.0.0.4 - Build 2016-06-17")
	, fname("out")
	, find("")
        , logfile("")
        , logfileInterval("day")
	, nicToUse(1)
	, nicCount(1)
	, pcapMaxSize(100)
{
}


settings::~settings(void)
{
}

int settings::processCmdLineArgs(TCHAR** argv, int argc)
{
	if(!argv)
	{
		return -1;
	}

	unsigned int i = 0;
        std::vector<tstring> args;
	UniConvert uc;

        // copy argv values to vector, makes life easier...
	for (i = 0; i < argc; i++)
	{
                args.push_back(argv[i]);
        }
        
#ifdef _DEBUG
        for (i = 0; i < args.size(); i++)
        {
		dbgtprintf(_T("args[%d] = \'%s\'"), i, args.at(i).c_str());
	}
#endif

        // parse values 
	for (i = 0; i < args.size(); i++)
	{
                // compare to capitalized params
                tstring argCap = args.at(i);
                std::transform(argCap.begin(), argCap.end(), argCap.begin(), ::toupper);
            
		if (argCap == _T("-L")) // list interfaces
		{
			printNicList();
			
			// we can exit the loop here and tell the program to exit
			return 1;
		}
		if (argCap == _T("-I")) // use interface number
		{
			if (args.at(i+1).length() > 0)
			{
				nicToUse = uc.ts2int(args.at(i+1));
				nicIsSet = true;
			}
		}
		if (argCap == _T("-F")) // input log file, if empty DbgOutput (only WIN32)
		{
			if (args.at(i+1).length() > 0)
			{
				logfile = uc.ts2s(args.at(i+1));
			}
		}
		if (argCap == _T("-O")) // output filename
		{
			if (args.at(i+1).length() > 0)
			{
				fname = uc.ts2s(args.at(i+1));
			}
		}
		if (argCap == _T("-OI")) // new log interval
		{
			if (args.at(i+1).length() > 0)
			{
				logfileInterval = uc.ts2s(args.at(i+1));
			}
		}
		if (argCap == _T("-S")) // string to find
		{
			if (args.at(i+1).length() > 0)
			{
				find = uc.ts2s(args.at(i+1));
			}
		}
		if (argCap == _T("-PCAPMAX")) // string to find
		{
			if (args.at(i+1).length() > 0)
			{
				pcapMaxSize = uc.ts2int(args.at(i+1));
			}
		}
		

		if ( 
			argCap == _T("-?") ||
			argCap == _T("/?") ||
			argCap == _T("-h") ||
			argCap == _T("/h")
			) // display help message
		{
			std::cout << "Usage: " << std::endl;
			std::cout << "-l :: List all NICs" << std::endl;
			std::cout << "-i <num> :: number of NIC to be captured, default = 1" << std::endl;
                        std::cout << "-f <string> :: path to log file, if empty DbgOut (only Windows) will be used, mandatory on Linux, default = \"\"";
			std::cout << "-o <string> :: prefix for output files, default = out" << std::endl;
			std::cout << "-oi <string> :: interval for log file, values = none, hour, day, default = day" << std::endl;
			std::cout << "-s <string> :: string to search (if empty only debug output to file, dump will be deleted), default = \"\"" << std::endl;
			std::cout << "-pcapmax <int> :: max size of pcap file [in MB], default = 100" << std::endl;
			std::cout << std::endl;

			std::cout << "Usage example: LogVsPcapTracer -f error" << std::endl;
			std::cout << std::endl;
			
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

std::string settings::getVersion(void)
{
	return version;
}

std::string settings::getFilename(void)
{
	return fname;
}

std::string settings::getFind(void)
{
	return find;
}

std::string settings::getLogfile(void)
{
        return logfile;
}

std::string settings::getLogInterval(void)
{
	return logfileInterval;
}

std::vector<std::string> settings::getProcRunningList(void)
{
	// todo: find a better way than hardcode it
	std::vector<std::string> proc;
        
#ifdef _WIN32        
        //proc.push_back("LogAndPcapSave.exe");
	proc.push_back("DbgView.exe");
        proc.push_back("Wireshark.exe");
#else        
        //proc.push_back("logandpcapsave");
        proc.push_back("wireshark");
#endif
        
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
		std::cout << std::to_string(i + 1) << " - " << adapters.at(i) << std::endl;
	}
}

int settings::getPcapMax(void)
{
	return pcapMaxSize;
}

