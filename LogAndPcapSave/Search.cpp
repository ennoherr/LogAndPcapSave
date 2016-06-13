#include "stdafx.h"

#include "TimeInfo.h"
#include "dbgprint.h"
#include "UniConvert.h"
#include "Files.h"
#include "DbgView.h"
#include "NetCapture.h"

#include <algorithm>
#include <string>
#include <thread>
#include <atomic>
#include <iostream>
#include <chrono>

#include "Search.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) {if (p) {delete(p); p = NULL;}}
#endif


Search::Search(void)
	: worker()
	, isThreadRunning(false)
{
}

Search::~Search(void)
{
}

int Search::startThread(NetCapture *netCap, std::queue<DbgData> &data, std::mutex &mtxData, std::string filename, std::string interval, std::string find)
{
	if (isThreadRunning)
	{
		dbgtprintf(_T("Search::startThread ERROR: thread already running with ID: 0x%lx"), worker.get_id());
		return -1;
	}

	int res = 0;

	// todo - convert to lamda, since microsoft threads cannot proceed so many params
	//if (res == 0) worker = std::thread(&Search::searchData, this, netCap, data, mtxData, filename, interval, find);
	if (res == 0) worker = std::thread([&]() { searchData(netCap, data, mtxData, filename, interval, find); });

	if (worker.joinable())
	{
		isThreadRunning = true;
		res = 0;

		dbgtprintf(_T("Search::startThread STATUS: thread started with ID: 0x%lx"), worker.get_id());
	}
	else
	{
		dbgtprintf(_T("Search::startThread ERROR: Failed to start thread, error = %d"), GetLastError());
		res = 1;
	}

	return res;
}

int Search::stopThread(void)
{
	int res = 0;

	if (worker.joinable())
	{
		isThreadRunning = false;
		worker.join();

		dbgtprintf(_T("Search::startThread STATUS: thread stopped"));
	}
	else
	{
		dbgtprintf(_T("Search::startThread WARNING: thread not running."));
		res = 1;
	}

	return res;
}

int Search::searchData(NetCapture *netCap, std::queue<DbgData> &data, std::mutex &mtxData, std::string filename, std::string interval, std::string find)
{
	// startup delay
	for (int i = 0; i < 32; i++)
	{
		if (isThreadRunning) break;
		else std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	int res = 0;

	std::string line = "";

	TimeInfo *ti = new TimeInfo();
	FileMgmt *allData = new FileMgmt(ti);
	FileMgmt *filterData = new FileMgmt(ti);

	while (isThreadRunning)
	{
		mtxData.lock();
		if (data.size() > 0)
		{
			DbgData dd = data.front();
			data.pop();
			line = std::to_string(dd.timestamp_ms) + ";" + dd.time + ";" + std::to_string(dd.pid) + ";" + dd.msg;

			//wcout << line << endl;
			allData->writeToFile(filename + "_all", line, interval);

			if (isInString(line, find))
			{
				std::cout << line << std::endl;

				filterData->writeToFile(filename + "_filter", line, interval);
				netCap->safeCurrentDump();
			}
		}
		mtxData.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	SAFE_DELETE(filterData);
	SAFE_DELETE(allData);
	SAFE_DELETE(ti);


	return res;
}

bool Search::isInString(std::string hay, std::string needle)
{
	bool res = false;

	if (hay.length() > 0 && needle.length() > 0)
	{
		transform(hay.begin(), hay.end(), hay.begin(), ::toupper);
		transform(needle.begin(), needle.end(), needle.begin(), ::toupper);

		size_t found = hay.find(needle);
		if (found != std::string::npos)
		{
			res = true;
		}
	}

	return res;
}
