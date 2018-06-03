#ifndef DEVICES_COMPUTEDEVICE_AMDCOMPUTEDEVICE_H_
#define DEVICES_COMPUTEDEVICE_AMDCOMPUTEDEVICE_H_

#include "Devices/ComputeDevice/ComputeDevice.h"
#if ADL_LIBWORKS
#	include "3rdParty/ADL.h"
#endif


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
#if ADL_LIBWORKS
	ADL_CONTEXT_HANDLE _adlContext=nullptr;
#endif
	bool _powerHasFailed=false;
};

#endif
