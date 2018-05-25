#ifndef DEVICES_COMPUTEDEVICE_CUDACOMPUTEDEVICE_H_
#define DEVICES_COMPUTEDEVICE_CUDACOMPUTEDEVICE_H_

#include "Devices/ComputeDevice/ComputeDevice.h"
//#include "3rdParty/NVAPI.h"
#include "3rdParty/NVML.h"
#include "Enums.h"
#include <NvmlTypes.h>


//using namespace NVIDIA::NVAPI;


class CudaComputeDevice
	: public ComputeDevice
{
public:
//	CudaComputeDevice(CudaDevice* cudaDevice, Enums::DeviceGroupType group, int gpuCount, struct NvPhysicalGpuHandle* nvHandle);
	CudaComputeDevice(CudaDevice* cudaDevice, Enums::DeviceGroupType group, int gpuCount, nvmlDevice_t* nvHandle, ManagedCuda::Nvml::nvmlDevice* nvmlHandle);

	float Load() override;
	float Temp() override;
	int FanSpeed() override;
	double PowerUsage() override;

protected:
	int SMMajor;
	int SMMinor;

private:
//	struct NvPhysicalGpuHandle* _nvHandle=nullptr;
	nvmlDevice_t* _nvHandle=nullptr;
	ManagedCuda::Nvml::nvmlDevice* _nvmlDevice=nullptr; // For NVML
	const int GpuCorePState=0;
};

#endif
