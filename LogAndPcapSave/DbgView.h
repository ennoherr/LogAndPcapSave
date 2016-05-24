#pragma once

#include <queue>
#include <thread>
#include <atomic>
#include <mutex>

#include "TimeInfo.h"
#include "UnicodeConv.h"

using namespace std;

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

class CDbgView
{
public:
	CDbgView(queue<DBG_DATA> *qData, mutex *mtxData, CTimeInfo *pTI, CUnicodeConv *pUC);
	~CDbgView(void);

	int Start(void);
	int Stop(void);

private:
	CTimeInfo *m_pTI;
	CUnicodeConv *m_pUC;

	queue<DBG_DATA> *m_qData;
	thread m_thWorker;
	mutex *m_mtxData;
	atomic<bool> m_bThreadRunning;

	HANDLE m_hReadyEvent;

	void EventThreadRoutine(void);
	wstring RemoveCRLF(wstring str);
};

