#pragma once

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <fstream>

#define WPCAP
#define HAVE_REMOTE
#include <pcap.h>

#include "TimeInfo.h"
#include "UniConvert.h"


class NetCapture
{
public:
	NetCapture(void);
	~NetCapture(void);

	int startCaptureThread(const unsigned Interface, std::string fname, long long fMaxSizeMbyte);
	int stopCaptureThread(void);

	int getInterfacesCount(void);
	int getInterfaces(std::vector<std::string> &adapters);
	int setInterface(const unsigned Interface);

	int writeDump(std::string fname, long long fMaxSizeMbyte);
	void safeCurrentDump(void);

private:
	pcap_if_t *interfaces;
	pcap_t *selectedIf;
	unsigned int numOfIf;

	std::thread worker;
	std::atomic<bool> isThreadRunning;
	std::atomic<bool> isSafeDump;
	
	int initInterfaces(void);
	int cleanup(void);

	long long getFilesize(std::string fname);
	int saveOrDeleteDump(std::string fname, std::string safe_prefix = "", std::string safe_postfix = "");
};

