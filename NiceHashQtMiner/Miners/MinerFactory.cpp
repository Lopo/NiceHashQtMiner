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
		case Enums::AlgorithmType::Equihash:
			return new ClaymoreZcashMiner;
		case Enums::AlgorithmType::CryptoNight:
		case Enums::AlgorithmType::CryptoNightV7:
			return new ClaymoreCryptoNightMiner;
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
		case Enums::MinerBaseType::ccminer:
			return new Ccminer;
		case Enums::MinerBaseType::sgminer:
			return new Sgminer;
		case Enums::MinerBaseType::nheqminer:
			return new NhEqMiner;
		case Enums::MinerBaseType::ethminer:
			return CreateEthminer(deviceType);
		case Enums::MinerBaseType::Claymore:
			return CreateClaymore(algorithm);
		case Enums::MinerBaseType::OptiminerAMD:
			return new OptiminerZcashMiner;
		case Enums::MinerBaseType::excavator:
			return new Excavator(algorithm->SecondaryNiceHashID());
		case Enums::MinerBaseType::XmrStak:
			return new XmrStak;
		case Enums::MinerBaseType::ccminer_alexis:
			return new Ccminer;
		case Enums::MinerBaseType::experimental:
			return CreateExperimental(deviceType, algorithm->NiceHashID);
		case Enums::MinerBaseType::EWBF:
			return new Ewbf;
		case Enums::MinerBaseType::Prospector:
			return new Prospector;
		case Enums::MinerBaseType::Xmrig:
			return new Xmrig;
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
		return CreateMiner(device->DeviceType, algorithm);
		}
	return nullptr;
}
