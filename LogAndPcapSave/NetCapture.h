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

using namespace std;

class CNetCapture
{
public:
	CNetCapture(CTimeInfo *pTI, CUnicodeConv *pUC);
	~CNetCapture(void);

	int StartCaptureThread(const unsigned Interface, wstring fname, long long fMaxSizeMbyte);
	int StopCaptureThread(void);

	int GetInterfaces(vector<string> &vAdapters);
	int SetInterface(const unsigned Interface);

	int WriteDump(wstring fname, long long fMaxSizeMbyte);
	void SafeCurrentDump(void);

private:
	CTimeInfo *m_pTI;
	CUnicodeConv *m_pUC;

	pcap_if_t *m_pInterfaces;
	pcap_t *m_pSelectedIf;
	unsigned int m_uiNumOfDevices;

	thread m_thWorker;
	atomic<bool> m_bThreadRunning;
	atomic<bool> m_bSafeDump;
	
	int InitInterfaces(void);
	int Cleanup(void);

	long long GetFilesize(wstring fname);
	int SafeOrDeleteDump(wstring fname, wstring safe_prefix = L"", wstring safe_postfix = L"");
};

