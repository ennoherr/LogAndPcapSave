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

//using namespace std;

#define SAFE_DELETE(p) {if (p) {delete p; p = NULL;}}


int _tmain(int argc, _TCHAR* argv[])
{
	bool exit = false;
	std::string line = "";
	queue<DBG_DATA> qData;
	mutex mtxData;

	CDbgView *pDV = new CDbgView(&qData, &mtxData);
	CFiles *pFAllData = new CFiles();
	CFiles *pFFilter = new CFiles();
	CSearch *pS = new CSearch();
	CNetCapture *pNC = new CNetCapture();
	CSettings *pSet = new CSettings();
	CProcess *pProc = new CProcess();
	CHddMgmt *pHM = new CHddMgmt();
	
	// read cmd line args 
	pSet->ProcessCmdLineArgs(argv, argc);

	cout << "LogVsPcapTracer started... (Press Enter 'q' to quit)" << endl;
	cout << "Version: " << pSet->GetVersion() << endl;
	cout << "---------------------------------------" << endl;

	// select nic or use the one selected
	// check if another process is using and blocking DBGWIN_BUFFER
	if (!exit && pSet->GetNicCount() > 1)
	{
		std::cout << "Error: More than one NIC available. Use option -l and select the NIC to listen on" << endl;
		exit = true;
	}

	// close running processes
	if (!exit)
	{
		for each (string p in pSet->GetProcRunningList())
		{
			if (pProc->IsProcessRunning(p))
			{
				cout << "Error: Process \"" + p + "\" running. Close it and try again." << endl;
				exit = true;
			}
		}
	}

	// start threads
	if (!exit && pSet->GetNicToUse() > 0)
	{
		pNC->StartCaptureThread(pSet->GetNicToUse(), pSet->GetFilename(), pSet->GetPcapMax());	
		pDV->Start();

		cout << "Only output from filter will be shown." << endl;
		cout << "Filter input: \"" << pSet->GetFind() << "\"" << endl;
		cout << "---------------------------------------" << endl;
	}
	else
	{
		exit = true;
	}

	// main loop
	while (!exit)
	{
		mtxData.lock();
		if (qData.size() > 0)
		{
			DBG_DATA dd = qData.front();
			qData.pop();
			line = to_string(dd.timestamp_ms) + ";" + dd.time + ";" + to_string(dd.pid) + ";" + dd.msg;

			//wcout << line << endl;
			pFAllData->WriteToFile(pSet->GetFilename() + "_all", line, pSet->GetLogInterval());

			if (pS->IsInString(line, pSet->GetFind()))
			{
				wcout << line << endl;

				pFFilter->WriteToFile(pSet->GetFilename() + "_filter", line, pSet->GetLogInterval());
				pNC->SafeCurrentDump();
			}
		}
		mtxData.unlock();

		// check disk space
		if (pHM->ReadHddValues())
		{
			__int64 spaceMB = pHM->GetFreeBytesAvailable() / (1024 * 1024);

			if (spaceMB < (pSet->GetPcapMax() * 1.5))
			{
				wcout << L"free disk space [MB]: " << to_wstring(spaceMB) << endl;
				wcout << L"Not enough space to write files. Exit program..." << endl;
				exit = true;
			}
		}
		
		// hit enter and exit
		while (_kbhit())
		{
			if (_gettch_nolock() == 'q') exit = true;
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

	return 0;
}

