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

	int initInterfaces(void);
	int cleanup(void);

	int startCaptureThread(const unsigned Interface, std::string fname, long long fMaxSizeMbyte);
	int stopCaptureThread(void);

	int getInterfacesCount(void);
	int getInterfaces(std::vector<std::string> &adapters);
	int configInterface(const unsigned interface);

	int writeDump(const unsigned interface, std::string fname, long long fMaxSizeMbyte);
	void safeCurrentDump(void);

private:
	pcap_if_t *Interfaces;
	pcap_t *Pcap;
	unsigned int NumOfIf;

	std::thread Worker;
	std::atomic<bool> IsThreadRunning;
	std::atomic<bool> IsSafeDump;
	
	long long getFilesize(std::string fname);
	int saveOrDeleteDump(std::string fname, std::string safe_prefix = "", std::string safe_postfix = "");
};

