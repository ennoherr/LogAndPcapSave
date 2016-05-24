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
#include "UnicodeConv.h"


class CNetCapture
{
public:
	CNetCapture(void);
	~CNetCapture(void);

	int StartCaptureThread(const unsigned Interface, std::string fname, long long fMaxSizeMbyte);
	int StopCaptureThread(void);

	int GetInterfacesCount(void);
	int GetInterfaces(vector<std::string> &adapters);
	int SetInterface(const unsigned Interface);

	int WriteDump(std::string fname, long long fMaxSizeMbyte);
	void SafeCurrentDump(void);

private:
	pcap_if_t *m_pInterfaces;
	pcap_t *m_pSelectedIf;
	unsigned int m_uiNumOfDevices;

	thread m_thWorker;
	atomic<bool> m_bThreadRunning;
	atomic<bool> m_bSafeDump;
	
	int InitInterfaces(void);
	int Cleanup(void);

	long long GetFilesize(std::string fname);
	int SafeOrDeleteDump(std::string fname, std::string safe_prefix = "", std::string safe_postfix = "");
};

