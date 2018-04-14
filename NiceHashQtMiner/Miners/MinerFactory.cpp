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
#include "Miners/XmrStak/XmrStackCPUMiner.h"
#include "Miners/Prospector.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Algorithm.h"
#include "Miners/Xmrig.h"
#include "XmrStak/XmrStakAMD.h"


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

Miner* MinerFactory::CreateClaymore(Enums::AlgorithmType algorithmType, Enums::AlgorithmType secondaryAlgorithmType)
{
	switch (algorithmType) {
		case Enums::AlgorithmType::Equihash:
			return new ClaymoreZcashMiner;
		case Enums::AlgorithmType::CryptoNight:
		case Enums::AlgorithmType::CryptoNightV7:
			return new ClaymoreCryptoNightMiner;
		case Enums::AlgorithmType::DaggerHashimoto:
			return new ClaymoreDual(secondaryAlgorithmType);
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

Miner* MinerFactory::CreateMiner(Enums::DeviceType deviceType, Enums::AlgorithmType algorithmType, Enums::MinerBaseType minerBaseType, Enums::AlgorithmType secondaryAlgorithmType)
{
	switch (minerBaseType) {
		case Enums::MinerBaseType::ccminer:
			return new Ccminer;
		case Enums::MinerBaseType::sgminer:
			return new Sgminer;
		case Enums::MinerBaseType::nheqminer:
			return new NhEqMiner;
		case Enums::MinerBaseType::ethminer:
			return CreateEthminer(deviceType);
		case Enums::MinerBaseType::Claymore:
			return CreateClaymore(algorithmType, secondaryAlgorithmType);
		case Enums::MinerBaseType::OptiminerAMD:
			return new OptiminerZcashMiner;
		case Enums::MinerBaseType::excavator:
			return new Excavator(secondaryAlgorithmType);
		case Enums::MinerBaseType::XmrStackCPU:
			return new XmrStackCPUMiner;
		case Enums::MinerBaseType::ccminer_alexis:
			return new Ccminer;
		case Enums::MinerBaseType::experimental:
			return CreateExperimental(deviceType, algorithmType);
		case Enums::MinerBaseType::EWBF:
			return new Ewbf;
		case Enums::MinerBaseType::Prospector:
			return new Prospector;
		case Enums::MinerBaseType::Xmrig:
			return new Xmrig;
		case Enums::MinerBaseType::XmrStakAMD:
			return new XmrStakAMD;
		case Enums::MinerBaseType::Claymore_old:
			return new ClaymoreCryptoNightMiner(true);
		case Enums::MinerBaseType::dtsm:
			return new Dtsm;
		default:
			return nullptr;
		}
	return nullptr;
}

Miner* MinerFactory::CreateMiner(ComputeDevice* device, Algorithm* algorithm)
{
	if (device!=nullptr && algorithm!=nullptr) {
		return CreateMiner(device->DeviceType, algorithm->NiceHashID, algorithm->MinerBaseType, algorithm->SecondaryNiceHashID);
		}
	return nullptr;
}
