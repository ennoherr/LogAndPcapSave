#pragma once

#include <string>
#include "TimeInfo.h"

class FileMgmt
{
public:
	FileMgmt(TimeInfo *tiInOut);
	~FileMgmt(void);

	int writeToFile(std::string fname, std::string line, std::string interval = "none", bool append = true);

private:
	TimeInfo *ti;
};

