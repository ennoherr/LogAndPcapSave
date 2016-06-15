#include "stdafx.h"

#include <algorithm>
#include <string>
#include <fstream>

#include "UniConvert.h"
#include "dbgprint.h"
#include "TimeInfo.h"

#include "Files.h"


FileMgmt::FileMgmt(TimeInfo *tiInOut)
	: ti(NULL)
	//, initNewFile(true)
	, filename("")
{
	ti = tiInOut;
}


FileMgmt::~FileMgmt(void)
{
}

int FileMgmt::writeToFile(std::string fname, std::string line, std::string interval)
{
	if (ti == NULL || fname.length() == 0 || line.length() == 0)
	{
		return -1;
	}

	int res = 0;
	//std::string logfile = "";
	bool newFile = false;

	transform(interval.begin(), interval.end(), interval.begin(), ::toupper);

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

	// first time start a file has to be created
	//if (initNewFile)
	//{
	//	newFile = true;
	//	initNewFile = false;
	//}

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
		UniConvert uc;
		dbgtprintf(_T("CFiles::WriteToFile ERROR: Unable to open file \'%s\'"), uc.s2ts(fname).c_str());
		res = 1;
	}

	return res;
}

