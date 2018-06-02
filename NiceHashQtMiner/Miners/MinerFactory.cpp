#include "Miners/MinerFactory.h"
#include "Miners/ClaymoreDual.h"
#include "Miners/ClaymoreCryptoNightMiner.h"
#include "Miners/Ewbf.h"
#include "Miners/Excavator.h"
#include "Miners/Ccminer.h"
#include "Miners/Sgminer.h"
#include "Miners/Equihash/NhEqMiner.h"
#include "Miners/Equihash/OptiminerZcashMiner.h"
#include "Miners/Equihash/ClaymoreZcashMiner.h"
#include "Miners/Equihash/Dtsm.h"
#include "Miners/ethminer/MinerEtherumOCL.h"
#include "Miners/ethminer/MinerEtherumCUDA.h"
#include "Miners/XmrStak/XmrStak.h"
#include "Miners/Prospector.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Algorithms/Algorithm.h"
#include "Miners/Xmrig.h"
#include "Miners/CpuMiner.h"


Miner* MinerFactory::CreateEthminer(Enums::DeviceType deviceType)
{
#if WITH_AMD
	if (deviceType==Enums::DeviceType::AMD) {
		return new MinerEtherumOCL;
		}
#endif
#if WITH_NVIDIA
	if (deviceType==Enums::DeviceType::NVIDIA) {
		return new MinerEtherumCUDA;
		}
#endif
	return nullptr;
}

Miner* MinerFactory::CreateClaymore(Algorithm* algorithm)
{
	switch (algorithm->NiceHashID) {
#if WITH_AMD
		case Enums::AlgorithmType::Equihash:
			return new ClaymoreZcashMiner;
		case Enums::AlgorithmType::CryptoNight:
		case Enums::AlgorithmType::CryptoNightV7:
			return new ClaymoreCryptoNightMiner;
#endif
		case Enums::AlgorithmType::DaggerHashimoto:
			return new ClaymoreDual(algorithm->SecondaryNiceHashID());
		default:
			return nullptr;
		}
	return nullptr;
}

Miner* MinerFactory::CreateExperimental(Enums::DeviceType deviceType, Enums::AlgorithmType algorithmType)
{
#if WITH_NVIDIA
	if (algorithmType==Enums::AlgorithmType::NeoScrypt && deviceType==Enums::DeviceType::NVIDIA) {
		return new Ccminer;
		}
#endif
	return nullptr;
}

Miner* MinerFactory::CreateMiner(Enums::DeviceType deviceType, Algorithm* algorithm)
{
	switch (algorithm->MinerBaseType) {
#if WITH_NVIDIA
		case Enums::MinerBaseType::ccminer:
			return new Ccminer;
#endif
#if WITH_AMD
		case Enums::MinerBaseType::sgminer:
			return new Sgminer;
#endif
		case Enums::MinerBaseType::nheqminer:
			return new NhEqMiner;
		case Enums::MinerBaseType::ethminer:
			return CreateEthminer(deviceType);
		case Enums::MinerBaseType::Claymore:
			return CreateClaymore(algorithm);
#if WITH_AMD
		case Enums::MinerBaseType::OptiminerAMD:
			return new OptiminerZcashMiner;
#endif
#if WITH_NVIDIA
		case Enums::MinerBaseType::excavator:
			return new Excavator(algorithm->SecondaryNiceHashID());
#endif
		case Enums::MinerBaseType::XmrStak:
			return new XmrStak;
#if WITH_NVIDIA
		case Enums::MinerBaseType::ccminer_alexis:
			return new Ccminer;
#endif
		case Enums::MinerBaseType::experimental:
			return CreateExperimental(deviceType, algorithm->NiceHashID);
#if WITH_NVIDIA
		case Enums::MinerBaseType::EWBF:
			return new Ewbf;
#endif
		case Enums::MinerBaseType::Prospector:
			return new Prospector;
		case Enums::MinerBaseType::Xmrig:
			return new Xmrig;
#if WITH_NVIDIA
		case Enums::MinerBaseType::dtsm:
			return new Dtsm;
#endif
		case Enums::MinerBaseType::cpuminer:
			return new CpuMiner;
		default:
			return nullptr;
		}
	return nullptr;
}

Miner* MinerFactory::CreateMiner(ComputeDevice* device, Algorithm* algorithm)
{
	if (device!=nullptr && algorithm!=nullptr) {
		return CreateMiner(device->DeviceType, algorithm);
		}
	return nullptr;
}
