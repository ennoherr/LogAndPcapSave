#include "stdafx.h"

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#endif

#include <assert.h>
#include <sys/stat.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <ctime>
#include <chrono>

#include "LogData.h"
#include "TimeInfo.h"
#include "dbgprint.h"
#include "Tail.h"

#include "DbgView.h"


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
/// <remarks>	Enno Herr, 30.06.2016. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

DbgView::DbgView(std::queue<DbgData> *dataInOut, std::mutex *mtxInOut)
	: logfile("")
        , worker()
	, isThreadRunning(false)
	, data(NULL)
	, mtx(NULL)
	, readyEvent(0)
{
	data = dataInOut;
	mtx = mtxInOut;
}	  

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Destructor. </summary>
///
/// <remarks>	Enno Herr, 30.06.2016. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

DbgView::~DbgView(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	set the name of the logfile. if empty data from OutputDebugSting (only WIN32) will 
///             be used. Therefore on Posix systems this is mandatory. </summary>
///
/// <remarks>	Enno Herr, 06.12.2016. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

void DbgView::setLogfile(const std::string file)
{
    logfile = file;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Start thread. </summary>
///
/// <remarks>	Enno Herr, 30.06.2016. </remarks>
///
/// <returns>	0 on success, else error. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

int DbgView::Start(void)
{
	if (isThreadRunning)
	{
		dbgtprintf(_T("DbgView::Start ERROR: thread already running with ID: 0x%lx"), worker.get_id());
		return -1;
	}

	int res = 0;
	worker = std::thread(&DbgView::EventThreadRoutine, this);

	if (worker.joinable())
	{
		isThreadRunning = true;
		res = 0;

		dbgtprintf(_T("DbgView::Start STATUS: thread started with ID: 0x%lx"), worker.get_id());
	}
	else
	{
		res = 1;

		dbgtprintf(_T("DbgView::Start ERROR: Failed to start thread, error = %d"), GetLastError());
	}

	return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Stop thread. </summary>
///
/// <remarks>	Enno Herr, 30.06.2016. </remarks>
///
/// <returns>	0 on success, else error. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

int DbgView::Stop(void)
{
	int res = 0;

	if (worker.joinable())
	{
#ifdef _WIN32
            assert(readyEvent != INVALID_HANDLE_VALUE);
            SetEvent(readyEvent);
#endif
            SAFE_HANDLE(readyEvent);

            isThreadRunning = false;
            worker.join();
		
            dbgtprintf(_T("DbgView::Stop STATUS: thread stopped"));
	}
	else
	{
            res = 1;

            dbgtprintf(_T("DbgView::Stop WARNING: thread not running."));
	}

	return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	The main part of this routine comes from the MSDN DBMON example. </summary>
///
/// <remarks>	Enno Herr, 30.06.2016. </remarks>
///
/// <param name="pParam">	params passed to the thread as void*. </param>
///
/// <returns>	0 on success, else error. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

void DbgView::EventThreadRoutine(void)
{
	// startup delay - wait until a previous thread has been stopped
	for (int i = 0; i < 32; i++)
	{
		if (isThreadRunning) break;
		else std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	unsigned int res = 0;
	std::string temp = "";
        
      	TimeInfo TI;
        
        // no logfile, use OutputDebug (only WIN32)
        if (logfile.length() == 0)
        {
#ifdef _WIN32        
	HANDLE hAckEvent = INVALID_HANDLE_VALUE;
	HANDLE SharedFile = INVALID_HANDLE_VALUE;
	DBG_BUFFER* pDB = NULL;
	SECURITY_ATTRIBUTES sa = { 0 };
	SECURITY_DESCRIPTOR sd = { 0 };

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = &sd;

	while (isThreadRunning)
	{
		try
		{
			// Set the thread to a high priority because the debug strings
			// being sent by the debugged app can make it wait if
			// we don't grab the events quickly enough.
			if (res == 0 && !SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL))
			{
				res = 1;
			}

			// init
			if (res == 0 && !InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION))
			{
				res = 2;
			}
			if (res == 0 && !SetSecurityDescriptorDacl(&sd, TRUE, (PACL)NULL, FALSE))
			{
				res = 3;
			}
			if (res == 0 && !(hAckEvent = CreateEvent(&sa, FALSE, FALSE, _T("DBWIN_BUFFER_READY"))))
			{
				res = 4;
			}
			if (res == 0 && GetLastError() == ERROR_ALREADY_EXISTS)
			{
				res = 5;
			}
			if (res == 0 && !(readyEvent = CreateEvent(&sa, FALSE, FALSE, _T("DBWIN_DATA_READY"))))
			{
				res = 6;
			}
			if (res == 0 && !(SharedFile = CreateFileMapping(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE, 0, sizeof(DBG_BUFFER), _T("DBWIN_BUFFER"))))
			{
				res = 7;
			}
			if (res == 0 && !(pDB = (DBG_BUFFER*)MapViewOfFile(SharedFile, FILE_MAP_READ, 0, 0, 0)))
			{
				res = 8;
			}

			if (res == 0)
			{
				SetEvent(hAckEvent);

				// loop - get data and write it to queue
				while (isThreadRunning)
				{
					if (WaitForSingleObject(readyEvent, INFINITE) != WAIT_OBJECT_0)
					{
						break;
					}

					//printf("%d, %s", pDB->dwPid, pDB->abData);

					DbgData dd;
					dd.time = TI.getTimeReadableMs();
					dd.timestamp_ms = TI.getTimestampMs();
					dd.pid = pDB->dwPid;
					temp = reinterpret_cast<const char*>(pDB->abData);
					// some strings are too long causing an std::range_error exception when converting to string
					if (temp.length() > 1024) temp = temp.substr(0, 1024);
					dd.msg = removeCRLF(temp);

					mtx->lock();
					data->push(dd);
					mtx->unlock();

					// Ready for new event
					SetEvent(hAckEvent);

				} // end while
			}
		}
		catch(std::exception e)
		{
			dbgprintf("DbgView::EventThreadRoutine EXCEPTION: EventThreadRoutine(), caught = %s, type = %s", e.what(), typeid(e).name());
			res = 99;
		}

		// cleanup
		SAFE_HANDLE(hAckEvent);
	
		if (pDB)
		{
			UnmapViewOfFile(pDB);
			pDB = NULL;
		}
		SAFE_HANDLE(SharedFile);

		if (res > 0)
		{
			dbgtprintf(_T("DbgView::EventThreadRoutine ERROR: An error occured, res = %d, ErrorCode = %d"), res, GetLastError());
			
			// wait 10 sec before restarting the loop
			std::this_thread::sleep_for(std::chrono::milliseconds(10*1000));
			res = 0;
		}

	} // end while


	dbgtprintf(_T("DbgView::EventThreadRoutine STATUS: Exit thread with ID: 0x%lx"), worker.get_id());
#endif
            dbgtprintf(_T("ERROR: On Posix systems a log file must be given!"));
        } // end if logfile.length == 0
        else
        {
            struct stat buf;
            long long fileSize;
            Tail tail(logfile);
            
            while (isThreadRunning)
            {
                // check for changes
                stat(logfile.c_str(), &buf);

                // on change
                if (buf.st_size != fileSize)
                {
                    tail.OnChange();
//                        std::vector<char> v = tail.GetChanges();
                    temp = tail.GetChangesAsString();

#ifndef _DEBUG
//                        std::cout << "raw: " << &v.front() << std::endl;
//                        std::cout << "str: " << s << std::endl;
#endif // !_DEBUG
                        
                        // remember filesize
                    fileSize = buf.st_size;
                
                    DbgData dd;
                    dd.time = TI.getTimeReadableMs();
                    dd.timestamp_ms = TI.getTimestampMs();
                    dd.pid = 0;

                    // some strings are too long causing an std::range_error exception when converting to string
                    if (temp.length() > 1024) temp = temp.substr(0, 1024);
                    dd.msg = removeCRLF(temp);

                    mtx->lock();
                    data->push(dd);
                    mtx->unlock();
                }
                
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                
            } // end - while
	} // end - if-else logfile
        
	// in case thread has exited for other reasons
	isThreadRunning = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Remove CR and LF from string. </summary>
///
/// <remarks>	Enno Herr, 30.06.2016. </remarks>
///
/// <param name="str">	string. </param>
///
/// <returns>	string without CR and LF. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

std::string DbgView::removeCRLF(std::string str)
{
	str.erase(remove(str.begin(), str.end(), '\r'), str.end());
	str.erase(remove(str.begin(), str.end(), '\n'), str.end());

	return str;
}

