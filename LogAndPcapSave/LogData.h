#pragma once

#include "stdafx.h"

#include <string>

// class for dbgview data
class DbgData
{
public:
	DbgData()
		: timestamp_ms(0)
		, time("")
		, pid(0)
		, msg("")
	{
	}
	~DbgData()
	{
	}

	long long timestamp_ms;
	std::string time;

	unsigned long pid;
	std::string msg;

};
