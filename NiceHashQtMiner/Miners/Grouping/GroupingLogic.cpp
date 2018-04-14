#include "Miners/Grouping/GroupingLogic.h"
#include "Miners/Grouping/MiningPair.h"
#include "Algorithm.h"
#include "Devices/ComputeDevice/ComputeDevice.h"


bool GroupingLogic::ShouldGroup(MiningPair* a, MiningPair* b)
{
	bool canGroup= IsGroupableMinerBaseType(a) && IsGroupableMinerBaseType(b);
	// group if same bin path and same algo type
	if (canGroup && IsSameBinPath(a, b) && IsSameAlgorithmType(a, b)
		&& ((IsNotCpuGroups(a, b) && IsSameDeviceType(a, b))
			|| (a->algorithm->MinerBaseType==Enums::MinerBaseType::Prospector && b->algorithm->MinerBaseType==Enums::MinerBaseType::Prospector)
			)
		) {
		return true;
		}
	return false;
}

bool GroupingLogic::IsNotCpuGroups(MiningPair* a, MiningPair* b)
{
	return a->Device->DeviceType!=Enums::DeviceType::CPU && b->Device->DeviceType!=Enums::DeviceType::CPU;
}

bool GroupingLogic::IsSameBinPath(MiningPair* a, MiningPair* b)
{
	return a->algorithm->MinerBinaryPath==b->algorithm->MinerBinaryPath;
}

bool GroupingLogic::IsSameAlgorithmType(MiningPair* a, MiningPair* b)
{
	return a->algorithm->DualNiceHashID()==b->algorithm->DualNiceHashID();
}

bool GroupingLogic::IsSameDeviceType(MiningPair* a, MiningPair* b)
{
	return a->Device->DeviceType==b->Device->DeviceType;
}

bool GroupingLogic::IsGroupableMinerBaseType(MiningPair* a)
{
	return a->algorithm->MinerBaseType!=Enums::MinerBaseType::cpuminer
		&& a->algorithm->MinerBaseType!=Enums::MinerBaseType::XmrStackCPU;
}
