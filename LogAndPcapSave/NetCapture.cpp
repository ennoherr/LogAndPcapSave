#include "stdafx.h"

#include "TimeInfo.h"
#include "dbgprint.h"
#include "UnicodeConv.h"

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <fstream>

#include <stdio.h>

#define WPCAP
#define HAVE_REMOTE
#include <pcap.h>

#include "NetCapture.h"


CNetCapture::CNetCapture(void)
	: m_pInterfaces(NULL)
	, m_pSelectedIf(NULL)
	, m_uiNumOfDevices(0)
	, m_thWorker()
	, m_bThreadRunning(false)
	, m_bSafeDump(false)
{
	InitInterfaces();
}

CNetCapture::~CNetCapture(void)
{
	Cleanup();
}

int CNetCapture::InitInterfaces(void)
{
	int iRet = 0;
	char errbuf[PCAP_ERRBUF_SIZE] = "\0";
	pcap_if_t *pIf = NULL;

	if (iRet == 0 && m_pInterfaces != NULL)
	{
		dbgtprintf(_T("CNetCapture::InitInterfaces WARNING: pcap_findalldevs_ex already called, Devicelist is initialized."));
		iRet = 1;
	}

	if (iRet == 0 && pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &m_pInterfaces, errbuf) == -1)
	{
		dbgtprintf(_T("CNetCapture::InitInterfaces ERROR: pcap_findalldevs_ex(...) returned with error msg - \'%s\'"), errbuf);
		iRet = 2;
	}

	if (iRet == 0)
	{
		for (pIf = m_pInterfaces; pIf != NULL; pIf = pIf->next)
		{
			m_uiNumOfDevices++;
		}
		
		if (m_uiNumOfDevices == 0)
		{
			dbgtprintf(_T("CNetCapture::InitInterfaces WARNING: No interfaces found."));
			iRet = 3;
		}
		else
		{
			dbgtprintf(_T("CNetCapture::InitInterfaces STATUS: %d interfaces found."), m_uiNumOfDevices);
		}
	}
	
	return iRet;
}

int CNetCapture::Cleanup(void)
{
	int iRet = 0;
	char errbuf[PCAP_ERRBUF_SIZE] = "\0";

	if (iRet == 0 && m_pInterfaces == NULL)
	{
		dbgtprintf(_T("CNetCapture::Cleanup WARNING: Devicelist is NOT initialized. No need to call pcap_freealldevs(...)."));
		iRet = 1;
	}

	if (iRet == 0)
	{
		pcap_freealldevs(m_pInterfaces);
		m_pInterfaces = NULL;
		m_pSelectedIf = NULL;
		m_uiNumOfDevices = 0;
	}

	return iRet;
}

int CNetCapture::StartCaptureThread(const unsigned Interface, std::string fname, long long fMaxSizeMbyte)
{
	if (m_bThreadRunning)
	{
		dbgtprintf(_T("CNetCapture::StartCaptureThread ERROR: thread already running with ID: 0x%lx"), m_thWorker.get_id());
		return -1;
	}
	
	int iRet = 0;

	if (iRet == 0) iRet = SetInterface(Interface);
	if (iRet == 0) m_thWorker = thread(&CNetCapture::WriteDump, this, fname, fMaxSizeMbyte);

	if (m_thWorker.joinable())
	{
		m_bThreadRunning = true;
		iRet = 0;

		dbgtprintf(_T("CNetCapture::StartCaptureThread STATUS: thread started with ID: 0x%lx"), m_thWorker.get_id());
	}
	else
	{
		dbgtprintf(_T("CNetCapture::StartCaptureThread ERROR: Failed to start thread, error = %d"), GetLastError());
		iRet = 1;
	}

	return iRet;
}

int CNetCapture::StopCaptureThread(void)
{
	int iRet = 0;

	if (m_thWorker.joinable()) // && m_bThreadRunning)
	{
		m_bThreadRunning = false;
		m_thWorker.join();
		
		dbgtprintf(_T("CNetCapture::StopCaptureThread STATUS: thread stopped"));
	}
	else
	{
		dbgtprintf(_T("CNetCapture::StopCaptureThread WARNING: thread not running."));
		iRet = 1;
	}

	return iRet;
}


int CNetCapture::GetInterfacesCount(void)
{
	if (m_pInterfaces == NULL)
	{
		return -1;
	}

	int iRet = 0;
	pcap_if_t *pIf = NULL;

	if (iRet == 0)
	{
		for (pIf = m_pInterfaces; pIf != NULL; pIf = pIf->next)
		{
			++iRet;
		}
	}

	return iRet;
}

int CNetCapture::GetInterfaces(vector<string> &adapters)
{
	if (m_pInterfaces == NULL)
	{
		return -1;
	}

	int iRet = 0;
	pcap_if_t *pIf = NULL;

	if (iRet == 0)
	{
		for (pIf = m_pInterfaces; pIf != NULL; pIf = pIf->next)
		{
			adapters.push_back(pIf->description);

			//printf("%d. %s\n    ", ++i, pIf->name);
			//if (pIf->description)
			//	printf(" (%s)\n", pIf->description);
			//else
			//	printf(" (No description available)\n");
		}
	}

	return iRet;
}

int CNetCapture::SetInterface(const unsigned Interface)
{
	int iRet = 0;
	pcap_if_t *pIf = NULL;
	char errbuf[PCAP_ERRBUF_SIZE] = "\0";
	unsigned int i = 0;

	if (iRet == 0 && m_pInterfaces == NULL)
	{
		iRet = InitInterfaces();
	}

	if (iRet == 0 && (Interface < 1 || Interface > m_uiNumOfDevices))
	{
		dbgtprintf(_T("CNetCapture::SetAdapter ERROR: Interface out of range. Max. Interfaces = %d, Input No. = %d."), m_uiNumOfDevices, Interface);
		iRet = 1;
	}

	// Jump to the selected adapter
	if (iRet == 0) for (pIf = m_pInterfaces, i = 0; i < Interface-1; pIf = pIf->next, i++);

	if (iRet == 0 && ((m_pSelectedIf = pcap_open(pIf->name, 100, PCAP_OPENFLAG_PROMISCUOUS, 20, NULL, errbuf)) == NULL))
	{
		dbgtprintf(_T("CNetCapture::SetAdapter ERROR: pcap_open(...) returned with error msg = \'%s\'."), errbuf);
		iRet = 2;
	}

	return iRet;
}

int CNetCapture::WriteDump(std::string fname, long long fMaxSizeMbyte)
{
	// startup delay
	for (int i = 0; i < 32; i++)
	{
		if (m_bThreadRunning) break;
		else Sleep(100);
	}
	
	int iRet = 0;
	int iWrite = 0;
	long long llFSize = 0;
	std::string DumpFile = "";
	CTimeInfo TI;

	pcap_dumper_t *pDumpfile = NULL;
	struct pcap_pkthdr *pHeader = NULL;
	const u_char *pPktData = NULL;

	if (iRet == 0 && m_pInterfaces == NULL)
	{
		iRet = InitInterfaces();
	}

	if (iRet == 0 && m_pSelectedIf == NULL)
	{
		dbgtprintf(_T("CNetCapture::WriteDump ERROR: No Interface selected, use SetInterface(...) first."));
		iRet = 1;
	}

	while (iRet == 0 && m_bThreadRunning)
	{	
		//DumpFile = fname + L"_" + to_wstring(TI.GetTimestampMs()) + L"_" + TI.GetTimeReadableMs(L"_", L"-", L"-") + L".pcap";
		DumpFile = fname + "_" + TI.GetTimeReadableMs("_", "-", "-") + ".pcap";
	
		if (iRet == 0 && (pDumpfile = pcap_dump_open(m_pSelectedIf, DumpFile.c_str())) == NULL)
		{
			dbgtprintf(_T("CNetCapture::WriteDump ERROR: pcap_dump_open(...) returned null."));
			iRet = 2;
		}

		//start the capture
		if (iRet == 0)
		{
			while((iWrite = pcap_next_ex(m_pSelectedIf, &pHeader, &pPktData)) >= 0 && m_bThreadRunning)
			{
				// timeout
				if(iWrite == 0)
				{
					Sleep(1);
					continue;
				}

				//save the packet on the dump file
				pcap_dump((unsigned char *) pDumpfile, pHeader, pPktData);

				llFSize = GetFilesize(DumpFile) / (1024 * 1024);
				if (llFSize >= fMaxSizeMbyte)
				{
					dbgprintf("CNetCapture::WriteDump STATUS: pcap file size limit reached. filename = %s, current size = %lld MB, max size = %lld MB", DumpFile.c_str(), llFSize, fMaxSizeMbyte);
					break;
				}
			} // end while - pcap_next_ex
		}

		// close the filepointer
		pcap_dump_close(pDumpfile);

		if (iRet == 0 && DumpFile.length() > 0 && SafeOrDeleteDump(DumpFile) != 0)
		{
			dbgtprintf(_T("CNetCapture::WriteDump ERROR: this::SafeOrDeleteDump(...) failed!"));
		}

		// reset error flag
		iRet = 0;

	} // end while - thread

	dbgtprintf(_T("CDbgView::WriteDump STATUS: Exit thread with ID: 0x%lx"), m_thWorker.get_id());
	
	// in case thread has exited for other reasons
	m_bThreadRunning = false;

	return iRet;
}

long long CNetCapture::GetFilesize(std::string fname)
{
	std::ifstream in(fname, std::ifstream::ate | std::ifstream::binary);
    return (long long)in.tellg(); 
}

void CNetCapture::SafeCurrentDump(void)
{
	m_bSafeDump = true;
}

int CNetCapture::SafeOrDeleteDump(std::string fname, std::string safe_prefix, std::string safe_postfix)
{
	int iRet = 0;

	if (m_bSafeDump)
	{
		dbgwprintf(L"CNetCapture::SafeOrDeleteDump STATUS: pcap file \'%s\' will be safed.", fname.c_str());

		if (safe_prefix.length() > 0 || safe_postfix.length() > 0)
		{
			std::string SafeFile = safe_prefix + fname + safe_postfix;
			if (fname != SafeFile)
			{
				if ((iRet = rename(fname.c_str(), SafeFile.c_str())) != 0)
				{
					dbgwprintf(L"CNetCapture::SafeOrDeleteDump WARNING: pcap file \'%s\' could not be renamed!", fname.c_str());
				}
			}
		}

		m_bSafeDump = false;
	}
	else
	{
		dbgwprintf(L"CNetCapture::SafeOrDeleteDump STATUS: pcap file \'%s\' will be deleted.", fname.c_str());
		if ((iRet = remove(fname.c_str())) != 0)
		{
			iRet = 2;
			dbgwprintf(L"CNetCapture::SafeOrDeleteDump WARNING: pcap file \'%s\' could not be deleted!", fname.c_str());
		}
	}
	
	return iRet;
}