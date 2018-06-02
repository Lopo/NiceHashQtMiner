#ifndef DEVICES_COMPUTEDEVICE_AMDCOMPUTEDEVICE_H_
#define DEVICES_COMPUTEDEVICE_AMDCOMPUTEDEVICE_H_

#include "Devices/ComputeDevice/ComputeDevice.h"
#include <3rdParty/ADL.h>


class AmdComputeDevice
	: public ComputeDevice
{
public:
	AmdComputeDevice(AmdGpuDevice* amdDevice, int gpuCount, bool isDetectionFallback, int adl2Index);

	int FanSpeed() override;
	float Temp() override;
	float Load() override;
	double PowerUsage() override;

private:
	int _adapterIndex; // For ADL
	int _adapterIndex2; // For ADL2
	ADL_CONTEXT_HANDLE _adlContext=nullptr;
	bool _powerHasFailed=false;
};

#endif
