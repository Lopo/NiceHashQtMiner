#ifndef PINVOKE_CPUID_H_
#define PINVOKE_CPUID_H_

#include <QString>
//#include <QLibrary>


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
//	static QLibrary* lib;
};

}

#endif
