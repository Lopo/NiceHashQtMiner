#ifndef PINVOKE_CPUID_H_
#define PINVOKE_CPUID_H_

#include "config.h"
#include <QString>
//#include <QLibrary>
#include <libcpuid/libcpuid.h>


namespace NiceHashQtMiner {

class CPUID
{
public:
//	static bool Init();

	static QString GetCPUName();
	static cpu_vendor_t GetCPUVendorID();
	static QString GetCPUVendor();
	static int SupportsSSE2();
	static int SupportsAVX();
	static int SupportsAVX2();
	static int SupportsAES();
	static int GetVirtualCoresCount();
	static int GetPhysicalProcessorCount();
	static int GetNumberOfCores();
	static bool IsHyperThreadingEnabled();
	static ulong CreateAffinityMask(int index, int percpu);
	static void AdjustAffinity(int pid, ulong mask);

private:
//	static QLibrary* lib;
};

}

#endif
