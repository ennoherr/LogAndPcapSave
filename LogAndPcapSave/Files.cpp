#include "stdafx.h"

#include <algorithm>
#include <string>
#include <fstream>

#include "UniConvert.h"
#include "dbgprint.h"
#include "TimeInfo.h"

#include "Files.h"

using namespace std;

FileMgmt::FileMgmt(TimeInfo *tiInOut)
	: ti(NULL)
{
	ti = tiInOut;
}


FileMgmt::~FileMgmt(void)
{
}

int FileMgmt::writeToFile(std::string fname, std::string line, std::string interval, bool append)
{
	if (ti == NULL || fname.length() == 0 || line.length() == 0)
	{
		return -1;
	}

	int res = 0;
	std::string logfile = "";
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
	
	// necessary to create a new file
	if (newFile)
	{
		logfile = fname + "_" + ti->getTimeReadableMs("_", "-", "-") + ".log";
	}
	
	ios_base::openmode om = (append) ? (ios_base::out | ios_base::app) : (ios_base::out);
	ofstream ofs(logfile, om);

	if (ofs.is_open())
	{
		ofs << line << endl;
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