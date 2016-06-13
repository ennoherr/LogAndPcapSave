#include "stdafx.h"

#include "TimeInfo.h"
#include "dbgprint.h"
#include "UniConvert.h"

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <fstream>
#include <chrono>

#include <stdio.h>

#define WPCAP
#define HAVE_REMOTE
#include <pcap.h>

#include "NetCapture.h"


NetCapture::NetCapture(void)
	: interfaces(NULL)
	, selectedIf(NULL)
	, numOfIf(0)
	, worker()
	, isThreadRunning(false)
	, isSafeDump(false)
{
	initInterfaces();
}

NetCapture::~NetCapture(void)
{
	cleanup();
}

int NetCapture::initInterfaces(void)
{
	int res = 0;
	char errbuf[PCAP_ERRBUF_SIZE] = "\0";
	pcap_if_t *iface = NULL;

	if (res == 0 && interfaces != NULL)
	{
		dbgtprintf(_T("NetCapture::InitInterfaces WARNING: pcap_findalldevs_ex already called, Devicelist is initialized."));
		res = 1;
	}

	if (res == 0 && pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &interfaces, errbuf) == -1)
	{
		dbgtprintf(_T("NetCapture::InitInterfaces ERROR: pcap_findalldevs_ex(...) returned with error msg - \'%s\'"), errbuf);
		res = 2;
	}

	if (res == 0)
	{
		for (iface = interfaces; iface != NULL; iface = iface->next)
		{
			numOfIf++;
		}
		
		if (numOfIf == 0)
		{
			dbgtprintf(_T("NetCapture::InitInterfaces WARNING: No interfaces found."));
			res = 3;
		}
		else
		{
			dbgtprintf(_T("NetCapture::InitInterfaces STATUS: %d interfaces found."), numOfIf);
		}
	}
	
	return res;
}

int NetCapture::cleanup(void)
{
	int res = 0;
	char errbuf[PCAP_ERRBUF_SIZE] = "\0";

	if (res == 0 && interfaces == NULL)
	{
		dbgtprintf(_T("NetCapture::Cleanup WARNING: Devicelist is NOT initialized. No need to call pcap_freealldevs(...)."));
		res = 1;
	}

	if (res == 0)
	{
		pcap_freealldevs(interfaces);
		interfaces = NULL;
		selectedIf = NULL;
		numOfIf = 0;
	}

	return res;
}

int NetCapture::startCaptureThread(const unsigned Interface, std::string fname, long long fMaxSizeMbyte)
{
	if (isThreadRunning)
	{
		dbgtprintf(_T("NetCapture::StartCaptureThread ERROR: thread already running with ID: 0x%lx"), worker.get_id());
		return -1;
	}
	
	int res = 0;

	if (res == 0) res = setInterface(Interface);
	if (res == 0) worker = std::thread(&NetCapture::writeDump, this, fname, fMaxSizeMbyte);

	if (worker.joinable())
	{
		isThreadRunning = true;
		res = 0;

		dbgtprintf(_T("NetCapture::StartCaptureThread STATUS: thread started with ID: 0x%lx"), worker.get_id());
	}
	else
	{
		dbgtprintf(_T("NetCapture::StartCaptureThread ERROR: Failed to start thread, error = %d"), GetLastError());
		res = 1;
	}

	return res;
}

int NetCapture::stopCaptureThread(void)
{
	int res = 0;

	if (worker.joinable())
	{
		isThreadRunning = false;
		worker.join();
		
		dbgtprintf(_T("NetCapture::StopCaptureThread STATUS: thread stopped"));
	}
	else
	{
		dbgtprintf(_T("NetCapture::StopCaptureThread WARNING: thread not running."));
		res = 1;
	}

	return res;
}


int NetCapture::getInterfacesCount(void)
{
	if (interfaces == NULL)
	{
		return -1;
	}

	int iRet = 0;
	pcap_if_t *iface = NULL;

	if (iRet == 0)
	{
		for (iface = interfaces; iface != NULL; iface = iface->next)
		{
			++iRet;
		}
	}

	return iRet;
}

int NetCapture::getInterfaces(std::vector<std::string> &adapters)
{
	if (interfaces == NULL)
	{
		return -1;
	}

	int res = 0;
	pcap_if_t *iface = NULL;

	if (res == 0)
	{
		for (iface = interfaces; iface != NULL; iface = iface->next)
		{
			adapters.push_back(iface->description);

			//printf("%d. %s\n    ", ++i, iface->name);
			//if (iface->description)
			//	printf(" (%s)\n", iface->description);
			//else
			//	printf(" (No description available)\n");
		}
	}

	return res;
}

int NetCapture::setInterface(const unsigned Interface)
{
	int res = 0;
	pcap_if_t *iface = NULL;
	char errbuf[PCAP_ERRBUF_SIZE] = "\0";
	unsigned int i = 0;

	if (res == 0 && interfaces == NULL)
	{
		res = initInterfaces();
	}

	if (res == 0 && (Interface < 1 || Interface > numOfIf))
	{
		dbgtprintf(_T("NetCapture::setAdapter ERROR: Interface out of range. Max. Interfaces = %d, Input No. = %d."), numOfIf, Interface);
		res = 1;
	}

	// Jump to the selected adapter
	if (res == 0) for (iface = interfaces, i = 0; i < Interface-1; iface = iface->next, i++);

	if (res == 0 && ((selectedIf = pcap_open(iface->name, 100, PCAP_OPENFLAG_PROMISCUOUS, 20, NULL, errbuf)) == NULL))
	{
		dbgtprintf(_T("NetCapture::setAdapter ERROR: pcap_open(...) returned with error msg = \'%s\'."), errbuf);
		res = 2;
	}

	return res;
}

int NetCapture::writeDump(std::string fname, long long fMaxSizeMbyte)
{
	// startup delay
	for (int i = 0; i < 32; i++)
	{
		if (isThreadRunning) break;
		else std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	
	int res = 0;
	int iWrite = 0;
	long long llFSize = 0;
	std::string DumpFile = "";
	TimeInfo ti;

	pcap_dumper_t *pDumpfile = NULL;
	struct pcap_pkthdr *pHeader = NULL;
	const u_char *pPktData = NULL;

	if (res == 0 && interfaces == NULL)
	{
		res = initInterfaces();
	}

	if (res == 0 && selectedIf == NULL)
	{
		dbgtprintf(_T("NetCapture::writeDump ERROR: No Interface selected, use setInterface(...) first."));
		res = 1;
	}

	while (res == 0 && isThreadRunning)
	{	
		DumpFile = fname + "_" + std::to_string(ti.getTimestampMs()) + ti.getTimeReadableMs("_", "-", "-") + ".pcap";
	
		if (res == 0 && (pDumpfile = pcap_dump_open(selectedIf, DumpFile.c_str())) == NULL)
		{
			dbgtprintf(_T("NetCapture::WriteDump ERROR: pcap_dump_open(...) returned null."));
			res = 2;
		}

		//start the capture
		if (res == 0)
		{
			while((iWrite = pcap_next_ex(selectedIf, &pHeader, &pPktData)) >= 0 && isThreadRunning)
			{
				// timeout
				if(iWrite == 0)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
					continue;
				}

				//save the packet on the dump file
				pcap_dump((unsigned char *) pDumpfile, pHeader, pPktData);

				llFSize = getFilesize(DumpFile) / (1024 * 1024);
				if (llFSize >= fMaxSizeMbyte)
				{
					dbgprintf("NetCapture::writeDump STATUS: pcap file size limit reached. filename = %s, current size = %lld MB, max size = %lld MB", DumpFile.c_str(), llFSize, fMaxSizeMbyte);
					break;
				}
			} // end while - pcap_next_ex
		}

		// close the filepointer
		pcap_dump_close(pDumpfile);

		if (res == 0 && DumpFile.length() > 0 && saveOrDeleteDump(DumpFile) != 0)
		{
			dbgtprintf(_T("NetCapture::WriteDump ERROR: this::saveOrDeleteDump(...) failed!"));
		}

		// reset error flag
		res = 0;

	} // end while - thread

	dbgtprintf(_T("DbgView::WriteDump STATUS: Exit thread with ID: 0x%lx"), worker.get_id());
	
	// in case thread has exited for other reasons
	isThreadRunning = false;

	return res;
}

long long NetCapture::getFilesize(std::string fname)
{
	std::ifstream in(fname, std::ifstream::ate | std::ifstream::binary);
    return (long long)in.tellg(); 
}

void NetCapture::safeCurrentDump(void)
{
	isSafeDump = true;
}

int NetCapture::saveOrDeleteDump(std::string fname, std::string safe_prefix, std::string safe_postfix)
{
	int res = 0;

	if (isSafeDump)
	{
		dbgprintf("NetCapture::saveOrDeleteDump STATUS: pcap file \'%s\' will be safed.", fname.c_str());

		if (safe_prefix.length() > 0 || safe_postfix.length() > 0)
		{
			std::string SafeFile = safe_prefix + fname + safe_postfix;
			if (fname != SafeFile)
			{
				if ((res = rename(fname.c_str(), SafeFile.c_str())) != 0)
				{
					dbgprintf("NetCapture::saveOrDeleteDump WARNING: pcap file \'%s\' could not be renamed!", fname.c_str());
				}
			}
		}

		isSafeDump = false;
	}
	else
	{
		dbgprintf("NetCapture::saveOrDeleteDump STATUS: pcap file \'%s\' will be deleted.", fname.c_str());
		if ((res = remove(fname.c_str())) != 0)
		{
			res = 2;
			dbgprintf("NetCapture::saveOrDeleteDump WARNING: pcap file \'%s\' could not be deleted!", fname.c_str());
		}
	}
	
	return res;
}

