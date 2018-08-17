#include "Devices/ComputeDevice/CudaComputeDevice.h"
#include "Devices/Algorithms/GroupAlgorithms.h"
#include "Devices/ComputeDeviceManager.h"
#include "International.h"
#include "Devices/CudaDevice.h"
#include "Utils/Helpers.h"
#include <NvmlTypes.h>

//using namespace NVIDIA::NVAPI;

float CudaComputeDevice::Load() // @todo
{
	float load=-1;
	try {
//		nvmlUtilization* rates=new nvmlUtilization;
		}
	catch (std::exception& e) {
		Helpers::ConsolePrint("NVML", e.what());
		}

	return load;
}

float CudaComputeDevice::Temp() // @todo
{
	float temp=-1;
	try {
		
		}
	catch (std::exception& e) {
		Helpers::ConsolePrint("NVML", e.what());
		}

	return temp;
}

int CudaComputeDevice::FanSpeed() // @todo
{
	int fanSpeed=-1;
	try {
		
		}
	catch (std::exception& e) {
		Helpers::ConsolePrint("NVML", e.what());
		}
	return fanSpeed;
}

double CudaComputeDevice::PowerUsage() // @todo
{
	try {
		
		}
	catch (std::exception& e) {
		Helpers::ConsolePrint("NVML", e.what());
		}
	return -1;
}

/*
CudaComputeDevice::CudaComputeDevice(CudaDevice* cudaDevice, Enums::DeviceGroupType group, int gpuCount, NvPhysicalGpuHandle* nvHandle)
	: ComputeDevice((int)cudaDevice->DeviceID, cudaDevice->GetName(), true, group, cudaDevice->IsEtherumCapable(), Enums::DeviceType::NVIDIA, International::GetText("ComputeDevice_Short_Name_NVIDIA_GPU").arg(gpuCount), cudaDevice->DeviceGlobalMemory)
{
	_SM_major=cudaDevice->SM_major;
	_SM_minor=cudaDevice->SM_minor;
	_Uuid=cudaDevice->UUID;
	AlgorithmSettings=GroupAlgorithms::CreateForDeviceList(this);
	_Index=ID+ComputeDeviceManager.Available.AvailCPUs();

	_nvHandle=nvHandle;
}
*/
CudaComputeDevice::CudaComputeDevice(CudaDevice* cudaDevice, Enums::DeviceGroupType group, int gpuCount, nvmlDevice_t* nvHandle, ManagedCuda::Nvml::nvmlDevice* nvmlHandle) // @todo finish
	: ComputeDevice((int)cudaDevice->DeviceID, cudaDevice->GetName(), true, group, cudaDevice->IsEtherumCapable(), Enums::DeviceType::NVIDIA, International::GetText("ComputeDevice_Short_Name_NVIDIA_GPU").arg(gpuCount), cudaDevice->DeviceGlobalMemory)
{
	BusID_=cudaDevice->pciBusID;
	SMMajor=cudaDevice->SM_major;
	SMMinor=cudaDevice->SM_minor;
	Uuid_=cudaDevice->UUID;
	AlgorithmSettings=GroupAlgorithms::CreateForDeviceList(this);
	Index_=ID+ComputeDeviceManager.Available.AvailCpus();

	_nvHandle=nvHandle;
	_nvmlDevice=nvmlHandle;

	ShouldRunEthlargement_= cudaDevice->DeviceName.contains("1080") || cudaDevice->DeviceName.contains("Titan Xp");
}
