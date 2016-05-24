#include "stdafx.h"

#include <string>
#include <cstdio>

#ifdef _WIN32
#include <windows.h>
#include <tlhelp32.h>
#else
// todo - includes for posix implementation - 15-06-08
#endif

#include "Process.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Default constructor. </summary>
///
/// <remarks>	Enno Herr, 10.06.2015. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

CProcess::CProcess(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Destructor. </summary>
///
/// <remarks>	Enno Herr, 10.06.2015. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

CProcess::~CProcess(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Check if a process is running. </summary>
///
/// <remarks>	Enno Herr, 10.06.2015. </remarks>
///
/// <param name="processName">	Name of the process. </param>
///
/// <returns>	true if process running, false if not. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

bool CProcess::IsProcessRunning(const wstring processName)
{
	bool exists = false;

#ifdef _WIN32
	PROCESSENTRY32 entry = { 0 };
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry))
	{
		while (Process32Next(snapshot, &entry))
		{
			if (!_wcsicmp(entry.szExeFile, processName.c_str()))
			{
				exists = true;
			}
		}
	}

	CloseHandle(snapshot);

#else
	// todo - posix implementation - 15-06-08

#endif

	return exists;
}