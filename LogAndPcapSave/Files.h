#pragma once

#include <string>
#include "TimeInfo.h"

class FileMgmt
{
public:
	FileMgmt(void);
	~FileMgmt(void);

	int writeToFile(std::string fname, std::string line, std::string interval);

private:
	TimeInfo *ti;
	std::string filename;
};

