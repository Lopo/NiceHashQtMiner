#ifndef CPUID_H
#define CPUID_H

#include <QLibrary>
#include <QString>
#define HAVE_STDINT_H
#include <libcpuid/libcpuid.h>


namespace NiceHashQtMiner {

class CPUID
{
public:
//	static bool Init();

	static QString GetCPUName();
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
	static QLibrary* lib;
};

}

#endif // CPUID_H
