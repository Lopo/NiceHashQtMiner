#include "Devices/ComputeDevice/CpuComputeDevice.h"
#include "International.h"
#include "Devices/GroupNames.h"
#include "Devices/Algorithms/GroupAlgorithms.h"
#include "Utils/Helpers.h"
#include <QException>


CpuComputeDevice::CpuComputeDevice(int id, QString group, QString name, int threads, ulong affinityMask, int cpuCount)
	: ComputeDevice(id, name, true, Enums::DeviceGroupType::CPU, false, Enums::DeviceType::CPU, International::GetText("ComputeDevice_Short_Name_CPU").arg(cpuCount), 0)
{
	_Threads=threads;
	_AffinityMask=affinityMask;
	Uuid_=GetUuid(ID, GroupNames::GetGroupName(DeviceGroupType, ID), Name, DeviceGroupType);
	AlgorithmSettings=GroupAlgorithms::CreateForDeviceList(this);
	Index_=ID;

//	_cpuCounter=new PerformanceCounter;
//	_cpuCounter->CategoryName="Processor";
//	_cpuCounter->CategoryName="% Processor Time";
//	_cpuCounter->InstanceName="_Total";
}

float CpuComputeDevice::Load()
{
	try {
//		if (_cpuCounter!=nullptr) {
//			return _cpuCounter.NextValue();
//			}
		}
	catch (QException& e) {
		Helpers::ConsolePrint("CPUDIAG", QString(e.what()));
		}
	return -1;
}
