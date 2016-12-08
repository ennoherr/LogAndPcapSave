#include "stdafx.h"

#include <algorithm>
#include <string>
#include <fstream>

#include "UniConvert.h"
#include "dbgprint.h"
#include "TimeInfo.h"

#include "Files.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) {if (p) {delete(p); p = NULL;}}
#endif


FileMgmt::FileMgmt(void)
	: ti(NULL)
	, filename("")
{
	ti = new TimeInfo();
}


FileMgmt::~FileMgmt(void)
{
	SAFE_DELETE(ti);
}

int FileMgmt::writeToFile(std::string fname, std::string line, std::string interval)
{
	if (fname.length() == 0 || line.length() == 0 || interval.length() == 0)
	{
		dbgprintf("FileMgmt::writeToFile ERROR: fname = \'%s\', line = \'%s\' or interval = \'%s\' have length zero!", fname.c_str(), line.c_str(), interval.c_str());
		return -1;
	}

	int res = 0;
	bool newFile = false;

	std::transform(interval.begin(), interval.end(), interval.begin(), ::toupper);

	if (interval == "NONE")
	{
		newFile = false;
	}
	if (interval == "HOUR")
	{
		newFile = ti->isNewHour();
	}
	if (interval == "DAY")
	{
		newFile = ti->isNewDay();
	}

	// create a new filename
	if (newFile || filename.length() == 0)
	{
		filename = fname + "_" + ti->getTimeReadableMs("_", "-", "-") + ".log";
	}
	
	std::ios_base::openmode om = (newFile) ? (std::ios_base::out) : (std::ios_base::out | std::ios_base::app);
	std::ofstream ofs(filename, om);

	if (ofs.is_open())
	{
		ofs << line << std::endl;
		ofs.close();
	}
	else
	{
		dbgprintf("FileMgmt::writeToFile ERROR: Unable to open file \'%s\'", filename.c_str());
		res = 1;
	}

	return res;
}

