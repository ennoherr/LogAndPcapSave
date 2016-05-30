#include "stdafx.h"

#include <string>
#include <cstdio>

#ifdef _WIN32
#include "UniConvert.h"

#include <windows.h>
#include <tlhelp32.h>
#else
// todo - includes for posix implementation - 16-05-30
#endif

#include "Process.h"

processes::processes(void)
{
}

processes::~processes(void)
{
}

bool processes::isProcessRunning(const std::string processName)
{
	bool exists = false;

#ifdef _WIN32
	UniConvert uc;
	tstring procToCheck = uc.s2ts(processName);

	PROCESSENTRY32 entry = { 0 };
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry))
	{
		while (Process32Next(snapshot, &entry))
		{
			if (!_tcsicmp(entry.szExeFile, procToCheck.c_str()))
			{
				exists = true;
			}
		}
	}

	CloseHandle(snapshot);

#else
	// todo - posix implementation - 16-05-30

#endif

	return exists;
}

