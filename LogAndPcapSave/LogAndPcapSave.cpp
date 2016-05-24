// LogVsPcapTracer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <conio.h>

#include <queue>
#include <iostream>
#include <mutex>

#include "Settings.h"
#include "DbgView.h"
#include "Search.h"
#include "NetCapture.h"
#include "Process.h"
#include "UnicodeConv.h"
#include "HddMgmt.h"

#include "Files.h"

using namespace std;

#define SAFE_DELETE(p) {if (p) {delete p; p = NULL;}}


int _tmain(int argc, _TCHAR* argv[])
{
	bool bExit = false;

	int iInterface = 0;
	int iPcapMax = 0;
	wstring line = L"";
	wstring fname = L"";
	wstring find = L"";
	wstring log_interval = L"";
	wstring processes[] = { L"DbgView.exe" };

	queue<DBG_DATA> qData;
	vector<string> vAdapters;
	mutex mtxData;

	CTimeInfo *pTI = new CTimeInfo();
	CUnicodeConv *pUC = new CUnicodeConv();
	
	CDbgView *pDV = new CDbgView(&qData, &mtxData, pTI, pUC);
	CFiles *pFAllData = new CFiles(pTI);
	CFiles *pFFilter = new CFiles(pTI);
	CSearch *pS = new CSearch();
	CNetCapture *pNC = new CNetCapture(pTI, pUC);
	CSettings *pSet = new CSettings();
	CProcess *pProc = new CProcess();
	CHddMgmt *pHM = new CHddMgmt();
	
	// read cmd line args 
	pSet->ProcessCmdLineArgs(argv, argc);

	iInterface = pSet->GetNic();
	iPcapMax = pSet->GetPcapMax();
	fname = pSet->GetFilename();
	find = pSet->GetFind();
	log_interval = pSet->GetLogInterval();

	wcout << L"LogVsPcapTracer started... (Press Enter to quit)" << endl;
	wcout << L"Version: " << pSet->GetVersion() << endl;
	wcout << L"---------------------------------------" << endl;

	// select nic or use the one selected
	// check if another process is using and blocking DBGWIN_BUFFER
	if (pSet->GetListNics())
	{
		pNC->GetInterfaces(vAdapters);
		for (unsigned i = 0; i < vAdapters.size(); i++)
		{
			cout << to_string(i+1) << " - " << vAdapters.at(i) << endl;
		}
		bExit = true;
	}
	else
	{
		for each (wstring p in processes)
		{
			if (pProc->IsProcessRunning(p))
			{
				wcout << L"Process \"" + p + L"\" running. Close it and try again." << endl;
				bExit = true;
			}
		}

		if (iInterface > 0 && !bExit)
		{
			pNC->StartCaptureThread(iInterface, fname, iPcapMax);	
			pDV->Start();

			wcout << L"Only output from filter will be shown." << endl;
			wcout << L"Filter input: \"" << find << L"\"" << endl;
			wcout << L"---------------------------------------" << endl;
		}
		else
		{
			bExit = true;
		}
	}

	// main loop
	while (!bExit)
	{
		mtxData.lock();
		if (qData.size() > 0)
		{
			DBG_DATA dd = qData.front();
			qData.pop();
			line = to_wstring(dd.timestamp_ms) + L";" + dd.time + L";" + to_wstring(dd.pid) + L";" + dd.msg;

			//wcout << line << endl;
			pFAllData->WriteToFile(fname + L"_all", line, log_interval);

			if (pS->IsInString(line, find))
			{
				wcout << line << endl;

				pFFilter->WriteToFile(fname + L"_filter", line, log_interval);
				pNC->SafeCurrentDump();
			}
		}
		mtxData.unlock();

		// check disk space
		if (pHM->ReadHddValues())
		{
			__int64 spaceMB = pHM->GetFreeBytesAvailable() / (1024 * 1024);

			if (spaceMB < (iPcapMax * 1.5))
			{
				wcout << L"free disk space [MB]: " << to_wstring(spaceMB) << endl;
				wcout << L"Not enough space to write files. Exit program..." << endl;
				bExit = true;
			}
		}
		
		// hit enter and exit
		while (_kbhit())
		{
			if (_gettch_nolock() == 13) bExit = true;
		}

		Sleep(1);
	}

	// stop threads
	pDV->Stop();
	pNC->StopCaptureThread();

	// cleanup
	SAFE_DELETE(pHM);
	SAFE_DELETE(pProc);
	SAFE_DELETE(pNC);
	SAFE_DELETE(pS);
	SAFE_DELETE(pFFilter);
	SAFE_DELETE(pFAllData);
	SAFE_DELETE(pDV);
	SAFE_DELETE(pSet);

	SAFE_DELETE(pTI);
	SAFE_DELETE(pUC);

	return 0;
}

