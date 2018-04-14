#include "Devices/ComputeDevice/AmdComputeDevice.h"
#include "3rdParty/ADL.h"
#include "Utils/Helpers.h"
#include "Devices/AmdGpuDevice.h"
#include "International.h"
#include "Devices/GroupNames.h"
#include "Devices/GroupAlgorithms.h"
#include "Devices/ComputeDeviceManager.h"


using namespace  ATI::ADL;


uint AmdComputeDevice::FanSpeed()
{
	if (!ADL::Init()) {
		return 0;
		}
	struct ADLFanSpeedValue adlf;
	adlf.iSpeedType=ADL_DL_FANCTRL_SPEED_TYPE_RPM;
	int result=ADL::ADL_Overdrive5_FanSpeed_Get(_adapterIndex, 0, &adlf);
	if (result!=ADL::ADL_SUCCESS) {
		Helpers::ConsolePrint("ADL", "ADL fan getting failed with error code "+QString::number(result));
		}
	return (uint)adlf.iFanSpeed;
}

float AmdComputeDevice::Temp()
{
	if (!ADL::Init()) {
		return 0;
		}
	struct ADLTemperature adlt;
	int result=ADL::ADL_Overdrive5_Temperature_Get(_adapterIndex, 0, &adlt);
	if (result!=ADL::ADL_SUCCESS) {
		Helpers::ConsolePrint("ADL", "ADL temp getting failed with error code "+QString::number(result));
		}
	return adlt.iTemperature*0.001f;
}

float AmdComputeDevice::Load()
{
	if (!ADL::Init()) {
		return 0;
		}
	struct ADLPMActivity adlp;
	int result=ADL::ADL_Overdrive5_CurrentActivity_Get(_adapterIndex, &adlp);
	if (result!=ADL::ADL_SUCCESS) {
		Helpers::ConsolePrint("ADL", "ADL load getting failed with error code "+QString::number(result));
		}
	return adlp.iActivityPercent;
}

AmdComputeDevice::AmdComputeDevice(AmdGpuDevice* amdDevice, int gpuCount, bool isDetectionFallback)
	: ComputeDevice(amdDevice->DeviceID(), amdDevice->DeviceName, true, Enums::DeviceGroupType::AMD_OpenCL, amdDevice->IsEtherumCapable(), Enums::DeviceType::AMD, International::GetText("ComputeDevice_Short_Name_AMD_GPU").arg(gpuCount), amdDevice->DeviceGlobalMemory())
{
	_Uuid= isDetectionFallback
		? GetUuid(ID, GroupNames::GetGroupName(DeviceGroupType, ID), Name, DeviceGroupType)
		: amdDevice->UUID;
	_BusID=amdDevice->BusID();
	_Codename=amdDevice->Codename();
	_InfSection=amdDevice->InfSection;
	AlgorithmSettings=GroupAlgorithms::CreateForDeviceList(this);
	_DriverDisableAlgos=amdDevice->DriverDisableAlgos;
	_Index=ID+ComputeDeviceManager.Avaliable.AvailCpus()+ComputeDeviceManager.Avaliable.AvailNVGpus();
	_adapterIndex=amdDevice->AdapterIndex;
}
