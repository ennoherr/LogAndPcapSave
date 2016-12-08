#include "stdafx.h"

#include <string>
#include <cstdio>

#ifdef _WIN32
#include "UniConvert.h"

#include <windows.h>
#include <tlhelp32.h>
#else
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <streambuf>
#include <fstream>
#endif

#include "dbgprint.h"

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
    DIR* dir;
    struct dirent* ent;
    char* endptr;
    char buf[512];
    
    std::string path;
    size_t len;

    // an implementation for opendir will come with c++17
    if (!(dir = opendir("/proc"))) 
    {
        dbgtprintf(_T("ERROR: cannot open /proc to read open processes!"));
        return true;
    }
    
    while ((ent = readdir(dir)) != NULL) 
    {
        // get and check for valid pid
        path = ent->d_name;
        if (path.find_first_not_of("0123456789") != std::string::npos)
        {
            continue;
        }
        
        // try to open the cmdline file
        std::ifstream file("/proc/" + path + "/cmdline");
        
        if (file.is_open())
        {
            file.read(buf, sizeof(buf));
            path = buf;
            
            if (path.length())
            {
#ifdef _DEBUG
                dbgtprintf(_T("path = %s"), path.c_str());
#endif   
                if (path.find(processName) != std::string::npos)
                {
                    exists = true;
                    break;
                }
                
//                // strip the prog from params and path
//                len = path.find_first_of(" ");
//                if (len != std::string::npos) path = path.substr(0, len);
//                len = path.find_last_of("/") + 1;
//                if (len != std::string::npos) path = path.substr(len);
//                
//                dbgtprintf(_T("len = %ld, path = %s"), len, path.c_str());
//                
//                // if process running
//                if (path == processName)
//                {
//                    exists = true;                 
//                    break;
//                }
            }
        }
    } // end while

    closedir(dir);

#endif

	return exists;
}

