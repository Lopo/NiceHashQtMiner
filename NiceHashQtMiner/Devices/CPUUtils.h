#ifndef DEVICES_CPUUTILS_H_
#define DEVICES_CPUUTILS_H_

#include "Enums.h"


class CPUUtils
{
public:
	static bool IsCpuMiningCapable();

private:
	static Enums::CPUExtensionType _detectOrder[];
	static bool HasExtensionSupport(Enums::CPUExtensionType type);
};

#endif
