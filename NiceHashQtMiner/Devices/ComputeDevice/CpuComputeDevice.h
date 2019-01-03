#ifndef DEVICES_COMPUTEDEVICE_CPUCOMPUTEDEVICE_H_
#define DEVICES_COMPUTEDEVICE_CPUCOMPUTEDEVICE_H_

#include "config.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include <libcpuid/libcpuid.h>


class CpuComputeDevice
	: public ComputeDevice
{
public:
	CpuComputeDevice(int id, QString group, QString name, int threads, ulong affinityMask, int cpuCount, cpu_vendor_t vendor=cpu_vendor_t::VENDOR_UNKNOWN);

	float Load() override;

private:
//	PerformanceCounter _cpuCounter;
};

#endif
