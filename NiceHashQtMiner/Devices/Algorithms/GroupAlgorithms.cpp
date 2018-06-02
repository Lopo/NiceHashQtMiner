#include "Devices/Algorithms/GroupAlgorithms.h"
#include "config.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Algorithms/Algorithm.h"
#include "Algorithms/DualAlgorithm.h"
#if WITH_AMD
#	include "Devices/AmdGpuDevice.h"
#endif
#include "Utils/Helpers.h"
#include "Devices/Algorithms/DefaultAlgorithms.h"


QMap<Enums::MinerBaseType, QList<Algorithm*>*>* GroupAlgorithms::CreateForDevice(ComputeDevice* device)
{
	if (device==nullptr) {
		return nullptr;
		}
	QMap<Enums::MinerBaseType, QList<Algorithm*>*>* algoSettings=CreateDefaultsForGroup(device->DeviceGroupType);
	if (algoSettings==nullptr) {
		return nullptr;
		}
#if WITH_AMD
	if (device->DeviceType==Enums::DeviceType::AMD) {
		// sgminer stuff
		if (algoSettings->contains(Enums::MinerBaseType::sgminer)) {
			QList<Algorithm*>* sgminerAlgos=algoSettings->value(Enums::MinerBaseType::sgminer);
			// Check for optimized version
			foreach (Algorithm* el, *sgminerAlgos) {
				if (el->NiceHashID==Enums::AlgorithmType::Lyra2REv2) {
					el->ExtraLaunchParameters=AmdGpuDevice::DefaultParam()+QStringList({"--nfactor", "10", "--xintensity", "64", "--thread-concurrency", "0", "--worksize", "64", "--gpu-threads", "2"});
					}
				if (el->NiceHashID==Enums::AlgorithmType::NeoScrypt && !device->Codename().contains("Tahiti")) {
					el->ExtraLaunchParameters=AmdGpuDevice::DefaultParam()+QStringList({"--nfactor", "10", "--xintensity", "2", "--thread-concurrency", "8192", "--worksize", "64", "--gpu-threads", "2"});
					Helpers::ConsolePrint("ComputeDevice", "The GPU detected ("+device->Codename()+") is not Tahiti. Changing default gpu-threads to 2.");
					}
				if (el->NiceHashID==Enums::AlgorithmType::CryptoNight || el->NiceHashID==Enums::AlgorithmType::CryptoNightV7) {
					if (device->Name.contains("Hawaii")) {
						el->ExtraLaunchParameters=AmdGpuDevice::DefaultParam()+QStringList({"--rawintensity", "640", "-w", "8", "-g", "2"});
						}
					else if (device->Name.contains("Vega")) {
						el->ExtraLaunchParameters=AmdGpuDevice::DefaultParam()+QStringList({"--rawintensity", "1850", "-w", "8", "-g", "2"});
						}
					}
				}
			}

		// Ellesmere, Polaris
		// Ellesmere sgminer workaround, keep this until sgminer is fixed to work with Ellesmere
		if ((device->Codename().contains("Ellesmere") || device->InfSection().toLower().contains("polaris"))) {
			foreach (QList<Algorithm*>* algosInMiner, *algoSettings) {
				foreach (Algorithm* algo, *algosInMiner) {
					// disable all algos in list
					if (algo->NiceHashID==Enums::AlgorithmType::Decred || algo->NiceHashID==Enums::AlgorithmType::Lbry) {
						algo->Enabled=false;
						}
					}
				}
			}
		// non sgminer optimizations
		if (algoSettings->contains(Enums::MinerBaseType::Claymore_old) && algoSettings->contains(Enums::MinerBaseType::Claymore)) {
			QList<Algorithm*>* claymoreOldAlgos=algoSettings->value(Enums::MinerBaseType::Claymore_old);
			int cryptoNightOldIndex=-1;
			for (int i=0; i<claymoreOldAlgos->count(); i++) {
				if (claymoreOldAlgos->at(i)->NiceHashID==Enums::AlgorithmType::CryptoNight) {
					cryptoNightOldIndex=i;
					break;
					}
				}
			QList<Algorithm*>* claymoreNewAlgos=algoSettings->value(Enums::MinerBaseType::Claymore);
			int cryptoNightNewIndex=-1;
			for (int i=0; i<claymoreNewAlgos->count(); i++) {
				if (claymoreNewAlgos->at(i)->NiceHashID==Enums::AlgorithmType::CryptoNight) {
					cryptoNightNewIndex=i;
					break;
					}
				}
			if (cryptoNightOldIndex>-1 && cryptoNightNewIndex>-1) {
				QStringList a4={
					"270",
					"270x",
					"280",
					"280x",
					"290",
					"290x",
					"370",
					"380",
					"390",
					"470",
					"480"
					};
				foreach (QString namePart, a4) {
					if (!device->Name.contains(namePart)) {
						continue;
						}
					claymoreOldAlgos->at(cryptoNightOldIndex)->ExtraLaunchParameters=QStringList({"-a", "4"});
					break;
					}
				QStringList old={
					"Verde",
					"Oland",
					"Bonaire"
					};
				foreach (QString codeName, old) {
					bool isOld=device->Codename().contains(codeName);
					claymoreOldAlgos->at(cryptoNightOldIndex)->Enabled=isOld;
					claymoreNewAlgos->at(cryptoNightNewIndex)->Enabled=!isOld;
					}
				}
			}

		// drivers algos issue
		if (device->DriverDisableAlgos()) {
			algoSettings=FilterMinerAlgos(algoSettings, {Enums::AlgorithmType::NeoScrypt, Enums::AlgorithmType::Lyra2REv2});
			}

		// disable by default
		{
			QList<Enums::MinerBaseType> minerBases={
				Enums::MinerBaseType::ethminer,
				Enums::MinerBaseType::OptiminerAMD
				};
			foreach (Enums::MinerBaseType minerKey, minerBases) {
				if (!algoSettings->contains(minerKey)) {
					continue;
					}
				foreach (Algorithm* algo, *algoSettings->value(minerKey)) {
					algo->Enabled=false;
					}
				}
			if (algoSettings->contains(Enums::MinerBaseType::sgminer)) {
				foreach (Algorithm* algo, *algoSettings->value(Enums::MinerBaseType::sgminer)) {
					if (algo->NiceHashID==Enums::AlgorithmType::DaggerHashimoto) {
						algo->Enabled=false;
						}
					}
				}
		}
		}
#endif

	// check if it is Etherum capable
	if (!device->IsEtherumCapale) {
		algoSettings=FilterMinerAlgos(algoSettings, {Enums::AlgorithmType::DaggerHashimoto});
		}
	if (algoSettings->contains(Enums::MinerBaseType::ccminer_alexis)) {
		foreach (Algorithm* unstable_algo, *algoSettings->value(Enums::MinerBaseType::ccminer_alexis)) {
			unstable_algo->Enabled=false;
			}
		}
	if (algoSettings->contains(Enums::MinerBaseType::experimental)) {
		foreach (Algorithm* unstable_algo, *algoSettings->value(Enums::MinerBaseType::experimental)) {
			unstable_algo->Enabled=false;
			}
		}
#if WITH_NVIDIA
	{ // NhEqMiner exceptions scope
		const Enums::MinerBaseType minerBaseKey=Enums::MinerBaseType::nheqminer;
		if (algoSettings->contains(minerBaseKey) && device->Name.contains("GTX")
			&& (device->Name.contains("560") || device->Name.contains("650") || device->Name.contains("680") || device->Name.contains("770"))
			) {
			algoSettings=FilterMinerBaseTypes(algoSettings, {minerBaseKey});
			}
	}
#endif
	return algoSettings;
}

QList<Algorithm*>* GroupAlgorithms::CreateForDeviceList(ComputeDevice* device)
{
	QList<Algorithm*>* ret=new QList<Algorithm*>;
	QMap<Enums::MinerBaseType, QList<Algorithm*>*>* retDict=CreateForDevice(device);
	if (retDict!=nullptr) {
		foreach (QList<Algorithm*>* kvp, *retDict) {
			ret->append(*kvp);
			}
		}
	return ret;
}

QMap<Enums::MinerBaseType, QList<Algorithm*>*>* GroupAlgorithms::CreateDefaultsForGroup(Enums::DeviceGroupType deviceGroupType)
{
	switch (deviceGroupType) {
		case Enums::DeviceGroupType::CPU:
			return DefaultAlgorithms::Cpu();
#if WITH_AMD
		case Enums::DeviceGroupType::AMD_OpenCL:
			return DefaultAlgorithms::Amd();
#endif
#if WITH_NVIDIA
		case Enums::DeviceGroupType::NVIDIA_2_1:
		case Enums::DeviceGroupType::NVIDIA_3_x:
		case Enums::DeviceGroupType::NVIDIA_5_x:
		case Enums::DeviceGroupType::NVIDIA_6_x:
		{
			QList<Enums::AlgorithmType> toRemoveAlgoTypes;
			QList<Enums::MinerBaseType> toRemoveMinerTypes;

			QMap<Enums::MinerBaseType, QList<Algorithm*>*>* ret=DefaultAlgorithms::Nvidia();

			switch (deviceGroupType) {
				case Enums::DeviceGroupType::NVIDIA_6_x:
				case Enums::DeviceGroupType::NVIDIA_5_x:
					toRemoveMinerTypes.append({
						Enums::MinerBaseType::nheqminer
						});
					break;
				case Enums::DeviceGroupType::NVIDIA_2_1:
				case Enums::DeviceGroupType::NVIDIA_3_x:
					toRemoveAlgoTypes.append({
						Enums::AlgorithmType::NeoScrypt,
						Enums::AlgorithmType::Lyra2RE,
						Enums::AlgorithmType::Lyra2REv2,
						Enums::AlgorithmType::CryptoNightV7
						});
					toRemoveMinerTypes.append({
						Enums::MinerBaseType::eqm,
//						Enums::MinerBaseType::excavator,
						Enums::MinerBaseType::EWBF,
						Enums::MinerBaseType::dtsm
						});
					break;
				default: // make compiler happy
					break;
				}
			if (deviceGroupType==Enums::DeviceGroupType::NVIDIA_2_1) {
				toRemoveAlgoTypes.append({
					Enums::AlgorithmType::DaggerHashimoto,
//					Enums::AlgorithmType::CryptoNight,
					Enums::AlgorithmType::CryptoNightV7,
					Enums::AlgorithmType::Pascal,
					Enums::AlgorithmType::X11Gost
					});
				toRemoveMinerTypes.append({
					Enums::MinerBaseType::Claymore,
					Enums::MinerBaseType::XmrStak
					});
				}

			// filter unused
			QMap<Enums::MinerBaseType, QList<Algorithm*>*>* finalRet=FilterMinerAlgos(ret, toRemoveAlgoTypes, new QList<Enums::MinerBaseType>({ Enums::MinerBaseType::ccminer }));
			return FilterMinerBaseTypes(finalRet, toRemoveMinerTypes);
		}
#endif
		default:
			return nullptr;
		}
	return nullptr;
}

QMap<Enums::MinerBaseType, QList<Algorithm*>*>* GroupAlgorithms::FilterMinerBaseTypes(QMap<Enums::MinerBaseType, QList<Algorithm*>*>* minerAlgos, QList<Enums::MinerBaseType> toRemove)
{
	QMap<Enums::MinerBaseType, QList<Algorithm*>*>* finalRet=new QMap<Enums::MinerBaseType, QList<Algorithm*>*>;
	foreach (Enums::MinerBaseType kvp, minerAlgos->keys()) {
		if (!toRemove.contains(kvp)) {
			finalRet->insert(kvp, minerAlgos->value(kvp));
			}
		}
	return finalRet;
}

QMap<Enums::MinerBaseType, QList<Algorithm*>*>* GroupAlgorithms::FilterMinerAlgos(QMap<Enums::MinerBaseType, QList<Algorithm*>*>* minerAlgos, QList<Enums::AlgorithmType> toRemove, QList<Enums::MinerBaseType>* toRemoveBase)
{
	QMap<Enums::MinerBaseType, QList<Algorithm*>*>* finalRet=new QMap<Enums::MinerBaseType, QList<Algorithm*>*>;
	if (toRemoveBase==nullptr) { // all minerbasekeys
		foreach (Enums::MinerBaseType kvp, minerAlgos->keys()) {
			QList<Algorithm*>* algoList=new QList<Algorithm*>;
			foreach (Algorithm* a, *minerAlgos->value(kvp)) {
				if (!toRemove.contains(a->NiceHashID)) {
					algoList->append(a);
					}
				}
			if (algoList->count()>0) {
				finalRet->insert(kvp, algoList);
				}
			}
		}
	else {
		foreach (Enums::MinerBaseType kvp, minerAlgos->keys()) {
			// filter only if base key is defined
			if (toRemoveBase->contains(kvp)) {
				QList<Algorithm*>* algoList=new QList<Algorithm*>;
				foreach (Algorithm* a, *minerAlgos->value(kvp)) {
					if (!toRemove.contains(a->NiceHashID)) {
						algoList->append(a);
						break;
						}
					}
				if (algoList->count()>0) {
					finalRet->insert(kvp, algoList);
					}
				}
			else { // keep all
				finalRet->insert(kvp, minerAlgos->value(kvp));
				}
			}
		}
	return finalRet;
}
