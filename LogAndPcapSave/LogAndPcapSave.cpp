// LogVsPcapTracer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <conio.h>

#include <queue>
#include <iostream>
#include <mutex>

#include "Settings.h"
#include "LogData.h"
#include "DbgView.h"
#include "Search.h"
#include "NetCapture.h"
#include "Process.h"
#include "HddMgmt.h"

#include "Files.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) {if (p) {delete(p); p = NULL;}}
#endif

// global
settings set;
NetCapture *netCap = NULL;
DbgView *logCap = NULL;
Search *s = NULL;

int loadConfig(int argc, TCHAR** argv)
{
	int res = 0;

	// read cmd line args 
	if (res == 0) res = set.processCmdLineArgs(argv, argc);

	// maybe one day a config file
	if (res == 0) res = set.loadIniFile();

	return res;
}

// select nic or use the one selected
int multipleNic(void)
{
	if (set.getNicIsSet())
	{
		return 0;
	}

	int res = 0;

	if (res == 0 && set.getNicCount() > 1)
	{
		std::cout << "Error: More than one NIC available. Use option -l and select the NIC to listen on" << std::endl;
		std::cout << std::endl;
		res = 1;
	}

	return res;
}

// check if another process is using and blocking DBGWIN_BUFFER
int closeProcesses(void)
{
	int res = 0;
	processes proc;
	
	for each (std::string p in set.getProcRunningList())
	{
		if (proc.isProcessRunning(p))
		{
			std::cout << "Error: Process \"" + p + "\" running. Close it and try again." << std::endl;
			++res;
		}
	}

	return res;
}

// check disk space
int checkHddSpace(void)
{
	int res = 0;
	hddMgmt hdd;

	if (hdd.readHddDiskSpace())
	{
		if (hdd.getFreeMBytesAvailable() < (set.getPcapMax() * 1.5))
		{
			std::cout << "free disk space [MB]: " << std::to_string(hdd.getFreeMBytesAvailable()) << std::endl;
			std::cout << "Not enough space to write files. Exit program..." << std::endl;
			res = 1;
		}
	}

	return res;
}

// stop capture threads
int stopCapture(void)
{
	int res = 0;

	// stop threads
	if (res == 0 && logCap != NULL) logCap->Stop();
	if (res == 0 && netCap != NULL) netCap->stopCaptureThread();

	SAFE_DELETE(logCap);
	SAFE_DELETE(netCap);

	return res;
}

// start captue threads
int startCapture(std::queue<DbgData> &data, std::mutex &mtxData)
{
	int res = 0;
	
	if (res == 0 && netCap != NULL || logCap != NULL) stopCapture();
	
	if (res == 0 && netCap == NULL) netCap = new NetCapture();
	else res = 1;

	if (res == 0 && logCap == NULL) logCap = new DbgView(&data, &mtxData);
	else res = 2;

	// no nic selected -> exit
	if (res == 0 && set.getNicToUse() == 0) res = 1;

	if (res == 0)
	{
		netCap->startCaptureThread(set.getNicToUse(), set.getFilename(), set.getPcapMax());
		logCap->Start();

		std::cout << "Only output from filter will be shown." << std::endl;
		std::cout << "Filter input: \"" << set.getFind() << "\"" << std::endl;
		std::cout << "---------------------------------------" << std::endl;
	}

	return res;
}

int stopAnalyze(void)
{
	int res = 0;

	if (res == 0 && s != NULL)	res = s->stopThread();
	else res = 1;

	SAFE_DELETE(s);

	return res;
}

int startAnalyze(std::queue<DbgData> &data, std::mutex &mtxData)
{
	int res = 0;

	if (res == 0 && s == NULL)	s = new Search();
	else res = 1;

	if (res == 0)	res = s->startThread(netCap, data, mtxData, set.getFilename(), set.getLogInterval(), set.getFind());

	return res;
}

int _tmain(int argc, _TCHAR* argv[])
{
	//bool exit = false;
	int res = 0;
	std::queue<DbgData> data;
	std::mutex mtxData;

	std::cout << "LogAndPcapSave starting... (please wait)" << std::endl;
	
	// init
	if (res == 0 && loadConfig(argc, argv) != 0)		res = 1;
	if (res == 0 && multipleNic() != 0)					res = 2;
	if (res == 0 && closeProcesses() != 0)				res = 3;
	if (res == 0 && startCapture(data, mtxData) != 0)	res = 4;
	if (res == 0 && startAnalyze(data, mtxData) != 0)	res = 5;

	if (res == 0)
	{
		std::cout << "Version: " << set.getVersion() << std::endl;
		std::cout << "(Press Enter 'q' to quit)" << std::endl;
		std::cout << "---------------------------------------" << std::endl;
	}

	// main loop
	while (res == 0 && _kbhit())
	{
		if (_gettch_nolock() == 'q') break;

		res = checkHddSpace();

		Sleep(1);
	}

	// cleanup
	stopAnalyze();
	stopCapture();
	
	return 0;
}

