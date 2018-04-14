#ifndef DEVICES_COMPUTEDEVICE_AMDCOMPUTEDEVICE_H_
#define DEVICES_COMPUTEDEVICE_AMDCOMPUTEDEVICE_H_

#include "Devices/ComputeDevice/ComputeDevice.h"


class AmdComputeDevice
	: public ComputeDevice
{
public:
	AmdComputeDevice(AmdGpuDevice* amdDevice, int gpuCount, bool isDetectionFallback);

	uint FanSpeed() override;
	float Temp() override;
	float Load() override;

private:
	int _adapterIndex; // For ADL
};

#endif
