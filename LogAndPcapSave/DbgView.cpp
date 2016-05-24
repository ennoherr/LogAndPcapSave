#include "stdafx.h"

#include <windows.h>
#include <process.h>
#include <assert.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <ctime>
#include <chrono>

#include "TimeInfo.h"
#include "dbgprint.h"
#include "UnicodeConv.h"

#include "DbgView.h"

using namespace std;

#ifndef STRING_LENGTH
	#define STRING_LENGTH 1024
#endif

#ifndef SAFE_DELETE
	#define SAFE_DELETE(p) {if (p) {delete(p); p = NULL;}}
#endif

#ifndef SAFE_HANDLE
	#define SAFE_HANDLE(p) {if (p) {CloseHandle(p); p = NULL;}}
#endif



////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Default constructor. </summary>
///
/// <remarks>	Enno Herr, 24.05.2016. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

CDbgView::CDbgView(queue<DBG_DATA> *qData, mutex *mtxData)
	: m_thWorker()
	, m_bThreadRunning(false)
	, m_qData(NULL)
	, m_mtxData(NULL)
	, m_hReadyEvent(NULL)
{
	m_qData = qData;
	m_mtxData = mtxData;
}	  

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Destructor. </summary>
///
/// <remarks>	Enno Herr, 24.05.2016. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

CDbgView::~CDbgView(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Start thread. </summary>
///
/// <remarks>	Enno Herr, 24.05.2016. </remarks>
///
/// <returns>	0 on success, else error. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

int CDbgView::Start(void)
{
	if (m_bThreadRunning)
	{
		dbgtprintf(_T("CDbgView::Start ERROR: thread already running with ID: 0x%lx"), m_thWorker.get_id());
		return -1;
	}

	int iRet = 0;
	m_thWorker = thread(&CDbgView::EventThreadRoutine, this);

	if (m_thWorker.joinable())
	{
		m_bThreadRunning = true;
		iRet = 0;
		dbgtprintf(_T("CDbgView::Start STATUS: thread started with ID: 0x%lx"), m_thWorker.get_id());
	}
	else
	{
		dbgtprintf(_T("CDbgView::Start ERROR: Failed to start thread, error = %d"), GetLastError());
		iRet = 1;
	}

	return iRet;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Stop thread. </summary>
///
/// <remarks>	Enno Herr, 24.05.2016. </remarks>
///
/// <returns>	0 on success, else error. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

int CDbgView::Stop(void)
{
	int iRet = 0;

	if (m_thWorker.joinable())
	{
		assert(m_hReadyEvent != INVALID_HANDLE_VALUE);
		SetEvent(m_hReadyEvent);

		SAFE_HANDLE(m_hReadyEvent);

		m_bThreadRunning = false;
		m_thWorker.join();
		
		dbgtprintf(_T("CDbgView::Stop STATUS: thread stopped"));
	}
	else
	{
		dbgtprintf(_T("CDbgView::Stop WARNING: thread not running."));
		iRet = 1;
	}

	return iRet;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	The main part of this routine comes from the MSDN DBMON example. </summary>
///
/// <remarks>	Enno Herr, 24.05.2016. </remarks>
///
/// <param name="pParam">	params passed to the thread as void*. </param>
///
/// <returns>	0 on success, else error. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

void CDbgView::EventThreadRoutine(void)
{
	// startup delay
	for (int i = 0; i < 32; i++)
	{
		if (m_bThreadRunning) break;
		else Sleep(100);
	}

	unsigned int iRet = 0;
	string temp = "";
	HANDLE hAckEvent = INVALID_HANDLE_VALUE;
	HANDLE SharedFile = INVALID_HANDLE_VALUE;
	DBG_BUFFER* pDB = NULL;
	SECURITY_ATTRIBUTES sa = { 0 };
	SECURITY_DESCRIPTOR sd = { 0 };

	CTimeInfo TI;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = &sd;

	while (m_bThreadRunning)
	{
		try
		{
			// Set the thread to a high priority because the debug strings
			// being sent by the debugged app can make it wait if
			// we don't grab the events quickly enough.
			if (iRet == 0 && !SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL))
			{
				iRet = 1;
			}

			// init
			if (iRet == 0 && !InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION))
			{
				iRet = 2;
			}
			if (iRet == 0 && !SetSecurityDescriptorDacl(&sd, TRUE, (PACL)NULL, FALSE))
			{
				iRet = 3;
			}
			if (iRet == 0 && !(hAckEvent = CreateEvent(&sa, FALSE, FALSE, _T("DBWIN_BUFFER_READY"))))
			{
				iRet = 4;
			}
			if (iRet == 0 && GetLastError() == ERROR_ALREADY_EXISTS)
			{
				iRet = 5;
			}
			if (iRet == 0 && !(m_hReadyEvent = CreateEvent(&sa, FALSE, FALSE, _T("DBWIN_DATA_READY"))))
			{
				iRet = 6;
			}
			if (iRet == 0 && !(SharedFile = CreateFileMapping(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE, 0, sizeof(DBG_BUFFER), _T("DBWIN_BUFFER"))))
			{
				iRet = 7;
			}
			if (iRet == 0 && !(pDB = (DBG_BUFFER*)MapViewOfFile(SharedFile, FILE_MAP_READ, 0, 0, 0)))
			{
				iRet = 8;
			}

			if (iRet == 0)
			{
				SetEvent(hAckEvent);

				while (m_bThreadRunning)
				{
					if (WaitForSingleObject(m_hReadyEvent, INFINITE) != WAIT_OBJECT_0)
					{
						break;
					}

					//printf("%d, %s", pDB->dwPid, pDB->abData);

					DBG_DATA dd;
					dd.time = TI.GetTimeReadableMs();
					dd.timestamp_ms = TI.GetTimestampMs();
					dd.pid = pDB->dwPid;
					temp = reinterpret_cast<const char*>(pDB->abData);
					// some strings are too long causing an std::range_error exception when converting to wstring
					if (temp.length() > 1024) temp = temp.substr(0, 1024);
					dd.msg = RemoveCRLF(temp);

					m_mtxData->lock();
					m_qData->push(dd);
					m_mtxData->unlock();

					// Ready for new event
					SetEvent(hAckEvent);

				} // end while
			}
		}
		catch(exception e)
		{
			dbgprintf("CDbgView::EventThreadRoutine EXCEPTION: EventThreadRoutine(), caught = %s, type = %s", e.what(), typeid(e).name());
			iRet = 99;
		}

		// cleanup
		SAFE_HANDLE(hAckEvent);
	
		if (pDB)
		{
			UnmapViewOfFile(pDB);
			pDB = NULL;
		}
		SAFE_HANDLE(SharedFile);

		if (iRet > 0)
		{
			dbgtprintf(_T("CDbgView::EventThreadRoutine ERROR: An error occured, iRet = %d, ErrorCode = %d"), iRet, GetLastError());
			
			// wait 10 sec
			Sleep(10*1000);
			iRet = 0;
		}

	} // end while


	dbgtprintf(_T("CDbgView::EventThreadRoutine STATUS: Exit thread with ID: 0x%lx"), m_thWorker.get_id());
	
	// in case thread has exited for other reasons
	m_bThreadRunning = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Remove CR and LF from string. </summary>
///
/// <remarks>	Enno Herr, 24.05.2016 </remarks>
///
/// <param name="str">	string. </param>
///
/// <returns>	string without CR and LF. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

string CDbgView::RemoveCRLF(string str)
{
	str.erase(remove(str.begin(), str.end(), '\r'), str.end());
	str.erase(remove(str.begin(), str.end(), '\n'), str.end());

	return str;
}

