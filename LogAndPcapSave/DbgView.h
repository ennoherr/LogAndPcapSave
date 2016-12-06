#pragma once

#include <cstring>

#include <queue>
#include <thread>
#include <atomic>
#include <mutex>

#include "LogData.h"
#include "TimeInfo.h"
#include "UniConvert.h"

struct DBG_BUFFER
{
	DWORD dwPid;
	BYTE  abData[4096 - sizeof(DWORD)];

	DBG_BUFFER()
		: dwPid(0)
	{
		memset(abData, (4096 - sizeof(DWORD)), 0);
	}
	~DBG_BUFFER()
	{
	}
};

class DbgView
{
public:
	DbgView(std::queue<DbgData> *dataInOut, std::mutex *mtxInOut);
	~DbgView(void);
        
        void setLogfile(const std::string file);

	int Start(void);
	int Stop(void);

private:
        std::string logfile;
        std::queue<DbgData> *data;
	std::thread worker;
	std::mutex *mtx;
	std::atomic<bool> isThreadRunning;

	HANDLE readyEvent;

	void EventThreadRoutine(void);
	std::string removeCRLF(std::string str);
};

