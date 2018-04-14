#include "Devices/ComputeDevice/CudaComputeDevice.h"
#include "Devices/GroupAlgorithms.h"
#include "Devices/ComputeDeviceManager.h"
#include "International.h"
#include "Devices/CudaDevice.h"

//using namespace NVIDIA::NVAPI;

float CudaComputeDevice::Load()
{
	float load=0;
/*	NvPStates* pStates=new NvPStates;
	pStates->Version=NVAPI::GPU_PSTATES_VER;
	pStates->PStates=new NvPState[NVAPI::MAX_PSTATES_PER_GPU];
*/
	return load;
}

float CudaComputeDevice::Temp()
{
	uint temp=0;
	return temp;
}

uint CudaComputeDevice::FanSpeed()
{
	int fanSpeed=0;
	return (uint)fanSpeed;
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
CudaComputeDevice::CudaComputeDevice(CudaDevice* cudaDevice, Enums::DeviceGroupType group, int gpuCount, nvmlDevice_t* nvHandle)
	: ComputeDevice((int)cudaDevice->DeviceID, cudaDevice->GetName(), true, group, cudaDevice->IsEtherumCapable(), Enums::DeviceType::NVIDIA, "GPU#"+QString::number(gpuCount), cudaDevice->DeviceGlobalMemory)
{
	_SM_major=cudaDevice->SM_major;
	_SM_minor=cudaDevice->SM_minor;
	_Uuid=cudaDevice->UUID;
	AlgorithmSettings=GroupAlgorithms::CreateForDeviceList(this);
	_Index=ID+ComputeDeviceManager.Avaliable.AvailCpus();

	_nvHandle=nvHandle;
}
