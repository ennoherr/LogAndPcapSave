// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <string>

struct DBG_DATA
{
	long long timestamp_ms;
	std::string time;
	
	DWORD pid;
	std::string msg;
	
	DBG_DATA()
		: timestamp_ms(0)
		, time("")
		, pid(0)
		, msg("")
	{
	}
	~DBG_DATA()
	{
	}
};

// reference additional headers your program requires here
