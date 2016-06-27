#pragma once

#include "NetCapture.h"

#include <string>
#include <thread>
#include <atomic>

class Search
{
public:
	Search(void);
	~Search(void);

	int startThread(NetCapture *netCap, std::queue<DbgData> *data, std::mutex *mtxData, std::string filename, std::string interval, std::string find);
	int stopThread(void);

private:
	std::thread worker;
	std::atomic<bool> isThreadRunning;

	int searchData(NetCapture *netCap, std::queue<DbgData> *data, std::mutex *mtxData, std::string filename, std::string interval, std::string find);
	bool isInString(std::string hay, std::string needle);
};

