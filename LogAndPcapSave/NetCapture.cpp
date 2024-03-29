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
	: Interfaces(NULL)
	, Pcap(NULL)
	, NumOfIf(0)
	, Worker()
	, IsThreadRunning(false)
	, IsSafeDump(false)
{
	//initInterfaces();
}

NetCapture::~NetCapture(void)
{
	cleanup();
}

int NetCapture::initInterfaces(void)
{
	int res = 0;
	char errbuf[PCAP_ERRBUF_SIZE] = "\0";
	pcap_if_t* iface = NULL;

	if (res == 0)
	{
		res = cleanup();
	}

	if (res == 0 && Interfaces != NULL)
	{
		dbgtprintf(_T("NetCapture::InitInterfaces WARNING: pcap_findalldevs_ex already called, Devicelist is initialized."));
		res = 1;
	}

#ifdef _WIN32
	if (res == 0 && pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &Interfaces, errbuf) == -1)
	{
		dbgtprintf(_T("NetCapture::InitInterfaces ERROR: pcap_findalldevs_ex(...) returned with error msg - \'%s\'"), errbuf);
		res = 2;
	}
#else
	if (res == 0 && pcap_findalldevs(&Interfaces, errbuf) == -1)
	{
		dbgtprintf(_T("NetCapture::InitInterfaces ERROR: pcap_findalldevs(...) returned with error msg - \'%s\'"), errbuf);
		res = 2;
	}
#endif

	if (res == 0)
	{
		for (iface = Interfaces; iface != NULL; iface = iface->next)
		{
			NumOfIf++;
		}

		if (NumOfIf == 0)
		{
			dbgtprintf(_T("NetCapture::InitInterfaces WARNING: No Interfaces found."));
			res = 3;
		}
		else
		{
			dbgtprintf(_T("NetCapture::InitInterfaces STATUS: %d Interfaces found."), NumOfIf);
		}
	}

	return res;
}

int NetCapture::cleanup(void)
{
	int res = 0;
	char errbuf[PCAP_ERRBUF_SIZE] = "\0";

	//if (res == 0 && Interfaces == NULL)
	//{
	//	dbgtprintf(_T("NetCapture::Cleanup WARNING: Devicelist is NOT initialized. No need to call pcap_freealldevs(...)."));
	//	res = 1;
	//}

	if (res == 0)
	{
		if (Interfaces != NULL)
		{
			pcap_freealldevs(Interfaces);
			Interfaces = NULL;
		}
		Pcap = NULL;
		NumOfIf = 0;
	}

	return res;
}

int NetCapture::startCaptureThread(const unsigned interface, std::string fname, long long fMaxSizeMbyte)
{
	if (IsThreadRunning)
	{
		dbgtprintf(_T("NetCapture::StartCaptureThread ERROR: thread already running with ID: 0x%lx"), Worker.get_id());
		return -1;
	}

	int res = 0;

//	if (res == 0) res = configInterface(Interface);
	if (res == 0) Worker = std::thread(&NetCapture::writeDump, this, interface, fname, fMaxSizeMbyte);

	if (Worker.joinable())
	{
		IsThreadRunning = true;
		res = 0;

		dbgtprintf(_T("NetCapture::StartCaptureThread STATUS: thread started with ID: 0x%lx"), Worker.get_id());
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

	if (Worker.joinable())
	{
		IsThreadRunning = false;
		Worker.join();

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
	if (Interfaces == NULL)
	{
		return -1;
	}

	int iRet = 0;
	pcap_if_t* iface = NULL;

	if (iRet == 0)
	{
		for (iface = Interfaces; iface != NULL; iface = iface->next)
		{
			++iRet;
		}
	}

	return iRet;
}

int NetCapture::getInterfaces(std::vector<std::string>& adapters)
{
	if (Interfaces == NULL)
	{
		return -1;
	}

	int res = 0;
	pcap_if_t* iface = NULL;

	if (res == 0)
	{
		for (iface = Interfaces; iface != NULL; iface = iface->next)
		{
			std::string adapter = std::string(iface->name);
			if (iface->description) {
				adapter += std::string(" (");
				adapter += std::string(iface->description);
				adapter += std::string(")");
			}
			adapters.push_back(adapter);

#ifdef _DEBUG
			printf("NetCapture::getInterfaces DEBUG: %s - desc: ", iface->name);
			if (iface->description)
				printf(" (%s)\n", iface->description);
			else
				printf(" (No description available)\n");
#endif
		}
	}

	return res;
}

int NetCapture::configInterface(const unsigned interface)
{
	int res = 0;
	pcap_if_t* iface = NULL;
	char errbuf[PCAP_ERRBUF_SIZE] = "\0";
	unsigned int i = 0;

	if (res == 0) // && Interfaces == NULL)
	{
		res = initInterfaces();
	}

	if (res == 0 && (interface < 1 || interface > NumOfIf))
	{
		dbgtprintf(_T("NetCapture::configInterface ERROR: Interface out of range. Max. Interfaces = %d, Input No. = %d."), NumOfIf, interface);
		res = 1;
	}

	// Jump to the selected adapter
	if (res == 0) for (iface = Interfaces, i = 0; i < interface - 1; iface = iface->next, i++);

#ifdef _WIN32
	if (res == 0 && ((Pcap = pcap_open(iface->name, 65536, PCAP_OPENFLAG_PROMISCUOUS, 1000, NULL, errbuf)) == NULL))
	{
		dbgtprintf(_T("NetCapture::configInterface ERROR: pcap_open(...) returned with error msg = \'%s\'."), errbuf);
		res = 2;
	}
#else
	// different approach, since default settings return truncated packages
	if (res == 0 && (Pcap = pcap_create(iface->name, errbuf)) == NULL)
	{
		dbgtprintf(_T("NetCapture::configInterface ERROR: pcap_create(...) returned with error msg = \'%s\'."), errbuf);
		res = 2;
	}

	// set the interface
	// max size of packet
	if (res == 0 && (res = pcap_set_snaplen(Pcap, 65535) != 0))
	{
		dbgtprintf(_T("NetCapture::configInterface ERROR: pcap_set_snaplen(...) returned with error = %d."), res);
		res = 3;
	}

	// promiscous enabled
	if (res == 0 && (res = pcap_set_promisc(Pcap, 1) != 0))
	{
		dbgtprintf(_T("NetCapture::configInterface ERROR: pcap_set_promisc(...) returned with error = %d."), res);
		res = 3;
	}

	// timeout
	if (res == 0 && (res = pcap_set_timeout(Pcap, 1000) != 0))
	{
		dbgtprintf(_T("NetCapture::configInterface ERROR: pcap_set_timeout(...) returned with error = %d."), res);
		res = 3;
	}

	// buffer = 16MB
	if (res == 0 && (res = pcap_set_buffer_size(Pcap, 16 << 20) != 0))
	{
		dbgtprintf(_T("NetCapture::configInterface ERROR: pcap_set_buffer_size(...) returned with error = %d."), res);
		res = 3;
	}

	if (res == 0 && (res = pcap_activate(Pcap)) != 0)
	{
		dbgtprintf(_T("NetCapture::configInterface ERROR: pcap_activate(...) returned with error = %d."), res);
		res = 4;
	}

	// nonblocking - according to doc it has to be set after activation
#ifdef _DEBUG
	int block = pcap_getnonblock(Pcap, errbuf);
	dbgtprintf(_T("NetCapture::configInterface MSG | ERROR: pcap_getnonblock(...) returned with block = %d, msg = \'%s\'."), block, errbuf);
#endif

	if (res == 0 && (res = pcap_setnonblock(Pcap, 1, errbuf) != 0))
	{
		dbgtprintf(_T("NetCapture::configInterface ERROR: pcap_setnonblock(...) returned with error = %d, msg = \'%s\'."), res, errbuf);
		res = 3;
	}

#ifdef _DEBUG        
	block = pcap_getnonblock(Pcap, errbuf);
	dbgtprintf(_T("NetCapture::configInterface MSG | ERROR: pcap_getnonblock(...) returned with block = %d, msg = \'%s\'."), block, errbuf);
#endif

#endif

	return res;
}

int NetCapture::writeDump(const unsigned interface, std::string fname, long long fMaxSizeMbyte)
{
	// startup delay
	for (int i = 0; i < 32; i++)
	{
		if (IsThreadRunning) break;
		else std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	int res = 0;
	int iWrite = 0;
	long long llFSize = 0;
	std::string DumpFile = "";
	TimeInfo ti;

	pcap_dumper_t* pDumpfile = NULL;
	struct pcap_pkthdr* pHeader = NULL;
	const u_char* pPktData = NULL;

	//if (res == 0 && Interfaces == NULL)
	//{
	//	res = initInterfaces();
	//}


	while (res == 0 && IsThreadRunning)
	{
		if (res == 0)
		{
			res = configInterface(interface);
		}

		if (res == 0 && Pcap == NULL)
		{
			dbgtprintf(_T("NetCapture::writeDump ERROR: No Interface selected, use setInterface(...) first."));
			res = 1;
		}

		DumpFile = fname + "_" + std::to_string(ti.getTimestampMs()) + "_" + ti.getTimeReadableMs("_", "-", "-") + ".pcap";

		if (res == 0 && (pDumpfile = pcap_dump_open(Pcap, DumpFile.c_str())) == NULL)
		{
			dbgtprintf(_T("NetCapture::WriteDump ERROR: pcap_dump_open(...) returned null."));
			res = 2;
		}

		//start the capture
		if (res == 0)
		{
			while ((iWrite = pcap_next_ex(Pcap, &pHeader, &pPktData)) >= 0 && IsThreadRunning)
			{
				// timeout
				//if (iWrite == 0)
				//{
				//	std::this_thread::sleep_for(std::chrono::milliseconds(1));
				//	continue;
				//}

				if (iWrite == 1)
				{
					//save the packet on the dump file
					pcap_dump((unsigned char*)pDumpfile, pHeader, pPktData);

					llFSize = getFilesize(DumpFile) / (1024 * 1024);
					if (llFSize >= fMaxSizeMbyte)
					{
						dbgprintf("NetCapture::writeDump STATUS: pcap file size limit reached. filename = %s, current size = %lld MB, max size = %lld MB", DumpFile.c_str(), llFSize, fMaxSizeMbyte);
						break;
					}
				}
				else
				{
					dbgprintf("NetCapture::writeDump WARNING or ERROR: pcap_next_ex returned = %d", iWrite);
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

	dbgtprintf(_T("DbgView::WriteDump STATUS: Exit thread with ID: 0x%lx"), Worker.get_id());

	// in case thread has exited for other reasons
	IsThreadRunning = false;

	return res;
}

long long NetCapture::getFilesize(std::string fname)
{
	std::ifstream in(fname, std::ifstream::ate | std::ifstream::binary);
	return (long long)in.tellg();
}

void NetCapture::safeCurrentDump(void)
{
	IsSafeDump = true;
}

int NetCapture::saveOrDeleteDump(std::string fname, std::string safe_prefix, std::string safe_postfix)
{
	int res = 0;

	if (IsSafeDump)
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

		IsSafeDump = false;
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

