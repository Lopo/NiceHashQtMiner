#include "Devices/ComputeDevice/AmdComputeDevice.h"
#include "Utils/Helpers.h"
#include "Devices/AmdGpuDevice.h"
#include "International.h"
#include "Devices/GroupNames.h"
#include "Devices/Algorithms/GroupAlgorithms.h"
#include "Devices/ComputeDeviceManager.h"


#if ADL_LIBWORKS
using namespace  ATI::ADL;
#endif


int AmdComputeDevice::FanSpeed()
{
#if ADL_LIBWORKS
	if (!ADL::Init()) {
		return -1;
		}
	struct ADLFanSpeedValue adlf;
	adlf.iSpeedType=ADL_DL_FANCTRL_SPEED_TYPE_RPM;
	int result=ADL::ADL_Overdrive5_FanSpeed_Get(_adapterIndex, 0, &adlf);
	if (result!=ADL::ADL_SUCCESS) {
		Helpers::ConsolePrint("ADL", "ADL fan getting failed with error code "+QString::number(result));
		return -1;
		}
	return adlf.iFanSpeed;
#else
	return -1;
#endif
}

float AmdComputeDevice::Temp()
{
#if ADL_LIBWORKS
	if (!ADL::Init()) {
		return -1;
		}
	struct ADLTemperature adlt;
	int result=ADL::ADL_Overdrive5_Temperature_Get(_adapterIndex, 0, &adlt);
	if (result!=ADL::ADL_SUCCESS) {
		Helpers::ConsolePrint("ADL", "ADL temp getting failed with error code "+QString::number(result));
		return -1;
		}
	return adlt.iTemperature*0.001f;
#else
	return -1;
#endif
}

float AmdComputeDevice::Load()
{
#if ADL_LIBWORKS
	if (!ADL::Init()) {
		return -1;
		}
	struct ADLPMActivity adlp;
	int result=ADL::ADL_Overdrive5_CurrentActivity_Get(_adapterIndex, &adlp);
	if (result!=ADL::ADL_SUCCESS) {
		Helpers::ConsolePrint("ADL", "ADL load getting failed with error code "+QString::number(result));
		return -1;
		}
	return adlp.iActivityPercent;
#else
	return -1;
#endif
}

double AmdComputeDevice::PowerUsage()
{
#if ADL_LIBWORKS
	if (!ADL::Init()) {
		return -1;
		}
	int power=-1;
	if (!_powerHasFailed && _adlContext!=nullptr) {
		int result=ADL::ADL2_Overdrive6_CurrentPower_Get(_adlContext, _adapterIndex2, 1, &power);
		if (result==ADL::ADL_SUCCESS) {
			return (double)power/(1<<8);
			}
		// Only alert once
		Helpers::ConsolePrint("ADL", QString("ADL power getting failed with code %1 for GPU %2. Turning off power for this GPU.").arg(result).arg(NameCount));
		_powerHasFailed=true;
		}
	return power;
#else
	return -1;
#endif
}

AmdComputeDevice::AmdComputeDevice(AmdGpuDevice* amdDevice, int gpuCount, bool isDetectionFallback, int adl2Index)
	: ComputeDevice(amdDevice->DeviceID(), amdDevice->DeviceName, true, Enums::DeviceGroupType::AMD_OpenCL, amdDevice->IsEtherumCapable(), Enums::DeviceType::AMD, International::GetText("ComputeDevice_Short_Name_AMD_GPU").arg(gpuCount), amdDevice->DeviceGlobalMemory())
{
	Uuid_= isDetectionFallback
		? GetUuid(ID, GroupNames::GetGroupName(DeviceGroupType, ID), Name, DeviceGroupType)
		: amdDevice->UUID;
	BusID_=amdDevice->BusID();
	_Codename=amdDevice->Codename();
	_InfSection=amdDevice->InfSection;
	AlgorithmSettings=GroupAlgorithms::CreateForDeviceList(this);
	_DriverDisableAlgos=amdDevice->DriverDisableAlgos;
	Index_=ID+ComputeDeviceManager.Available.AvailCpus()+ComputeDeviceManager.Available.AvailNVGpus();
	_adapterIndex=amdDevice->AdapterIndex;

#if ADL_LIBWORKS
	ADL::ADL2_Main_Control_Create(ADL_Main_Memory_Alloc, 0, &(_adlContext));
#endif
	_adapterIndex2=adl2Index;
}
