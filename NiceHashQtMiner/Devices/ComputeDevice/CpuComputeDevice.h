#ifndef DEVICES_COMPUTEDEVICE_CPUCOMPUTEDEVICE_H_
#define DEVICES_COMPUTEDEVICE_CPUCOMPUTEDEVICE_H_

#include "Devices/ComputeDevice/ComputeDevice.h"


class CpuComputeDevice
	: public ComputeDevice
{
public:
	CpuComputeDevice(int id, QString group, QString name, int threads, ulong affinityMask, int cpuCount);

	float Load() override;

private:
//	PerformanceCounter _cpuCounter;
};

#endif
