#define __MinerPaths_cpp__
#include "Miners/Grouping/MinerPaths.h"
#include "Algorithm.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Utils/Helpers.h"
#include "Devices/GroupAlgorithms.h"
#include <QMetaEnum>
#include <QFile>


MinerPath::MinerPath(Enums::AlgorithmType algo, QString path)
{
	Algorithm=algo;
	Path=path;
	Name=QMetaEnum::fromType<Enums::AlgorithmType>().valueToKey((int)Algorithm);
}

MinerTypePath::MinerTypePath(Enums::MinerBaseType type, QList<MinerPath> paths)
{
	Type=type;
	Algorithms=paths;
	Name=QMetaEnum::fromType<Enums::MinerBaseType>().valueToKey((int)type);
}

MinerPathPackage::MinerPathPackage(Enums::DeviceGroupType type, QList<MinerTypePath> paths)
{
	DeviceType=type;
	MinerTypes=paths;
	Name=QMetaEnum::fromType<Enums::DeviceGroupType>().valueToKey((int)DeviceType);
}


const QString MinerPaths::Data::Bin="bin";

const QString MinerPaths::Data::CcminerDecred=Bin+"/ccminer_decred/ccminer";

const QString MinerPaths::Data::CcminerNanashi=Bin+"/ccminer_nanashi/ccminer";
const QString MinerPaths::Data::CcminerNeoscrypt=Bin+"/ccminer_neoscrypt/ccminer";
const QString MinerPaths::Data::CcminerSp=Bin+"/ccminer_sp/ccminer";
const QString MinerPaths::Data::CcminerTPruvot=Bin+"/ccminer_tpruvot/ccminer";
const QString MinerPaths::Data::CcminerCryptonight=Bin+"/ccminer_cryptonight/ccminer";
const QString MinerPaths::Data::CcminerX11Gost=Bin+"/ccminer_x11gost/ccminer";
const QString MinerPaths::Data::CcminerKlausT=Bin+"/ccminer_klaust/ccminer";

const QString MinerPaths::Data::Ethminer=Bin+"/ethminer/ethminer";

const QString MinerPaths::Data::Sgminer560General=Bin+"/sgminer-5-6-0-general/sgminer";
const QString MinerPaths::Data::SgminerGm=Bin+"/sgminer-gm/sgminer";

const QString MinerPaths::Data::NhEqMiner=Bin+"/nheqminer_v0.4b/nheqminer";
const QString MinerPaths::Data::Excavator=Bin+"/excavator/excavator";

const QString MinerPaths::Data::XmrStackCpuMiner=Bin+"/xmr-stack-cpu/xmr-stak";
const QString MinerPaths::Data::XmrStakAmd=Bin+"/xmr-stak-amd/xmr-stak-amd";
const QString MinerPaths::Data::Xmrig=Bin+"/xmrig/xmrig";

const QString MinerPaths::Data::None="";

const QString MinerPaths::Data::Bin3rdParty="bin_3rdparty";

const QString MinerPaths::Data::ClaymoreZcashMiner=Bin3rdParty+"/claymore_zcash/zecminer64";
const QString MinerPaths::Data::ClaymoreCryptoNightMiner=Bin3rdParty+"/claymore_cryptonight/nsgpucnminer";
const QString MinerPaths::Data::ClaymoreCryptoNightMinerOld=Bin3rdParty+"/claymore_cryptonight_old/NsGpuCNMiner";
const QString MinerPaths::Data::OptiminerZcashMiner=Bin3rdParty+"/optiminer_zcash/optiminer-zcash";
const QString MinerPaths::Data::ClaymoreDual=Bin3rdParty+"/claymore_dual/ethdcrminer64";
const QString MinerPaths::Data::Ewbf=Bin3rdParty+"/ewbf/miner";
const QString MinerPaths::Data::Prospector=Bin3rdParty+"/prospector/prospector";
const QString MinerPaths::Data::Dtsm=Bin3rdParty+"/dtsm/zm";

QList<MinerPathPackage>* MinerPaths::MinerPathPackages=new QList<MinerPathPackage>;
QList<Enums::MinerBaseType>* MinerPaths::ConfigurableMiners=new QList<Enums::MinerBaseType>({
	Enums::MinerBaseType::ccminer,
	Enums::MinerBaseType::sgminer
	});


QString MinerPaths::GetPathFor(Enums::MinerBaseType minerBaseType, Enums::AlgorithmType algoType, Enums::DeviceGroupType devGroupType, bool def/*=false*/)
{
	if (!def & ConfigurableMiners->contains(minerBaseType)) {
		// Override with internals
		foreach (MinerPathPackage mpp, *MinerPathPackages) {
			if (mpp.DeviceType!=devGroupType) {
				continue;
				}
			foreach (MinerTypePath mtp, mpp.MinerTypes) {
				if (mtp.Type!=minerBaseType) {
					continue;
					}
				foreach (MinerPath mp, mtp.Algorithms) {
					if (mp.Algorithm!=algoType) {
						continue;
						}
					if (!mp.Path.isEmpty()) {
						if (QFile::exists(mp.Path)) {
							return mp.Path;
							}
						Helpers::ConsolePrint("PATHS", QString("Path %1 not found, using defaults").arg(mp.Path));
						}
					}
				}
			}
		}
	switch (minerBaseType) {
		case Enums::MinerBaseType::ccminer:
			return NvidiaGroups.Ccminer_path(algoType, devGroupType);
		case Enums::MinerBaseType::sgminer:
			return AmdGroup.SgminerPath(algoType);
		case Enums::MinerBaseType::nheqminer:
			return Data.NhEqMiner;
		case Enums::MinerBaseType::ethminer:
			return Data.Ethminer;
		case Enums::MinerBaseType::Claymore:
			return AmdGroup.ClaymorePath(algoType);
		case Enums::MinerBaseType::OptiminerAMD:
			return Data.OptiminerZcashMiner;
		case Enums::MinerBaseType::excavator:
			return Data.Excavator;
		case Enums::MinerBaseType::XmrStackCPU:
			return Data.XmrStackCpuMiner;
		case Enums::MinerBaseType::ccminer_alexis:
			return NvidiaGroups.CcminerUnstablePath(algoType, devGroupType);
		case Enums::MinerBaseType::experimental:
			return Experimental.GetPath(algoType, devGroupType);
		case Enums::MinerBaseType::EWBF:
			return Data.Ewbf;
		case Enums::MinerBaseType::Prospector:
			return Data.Prospector;
		case Enums::MinerBaseType::Xmrig:
			return Data.Xmrig;
		case Enums::MinerBaseType::XmrStakAMD:
			return Data.XmrStakAmd;
		case Enums::MinerBaseType::Claymore_old:
			return Data.ClaymoreCryptoNightMinerOld;
		case Enums::MinerBaseType::dtsm:
			return Data.Dtsm;
		default:
			return Data.None;
		}
	return Data.None;
}

QString MinerPaths::GetPathFor(ComputeDevice* computeDevice, Algorithm* algorithm)
{
	if (computeDevice==nullptr || algorithm==nullptr) {
		return Data.None;
		}

	return GetPathFor(algorithm->MinerBaseType, algorithm->NiceHashID, computeDevice->DeviceGroupType);
}

bool MinerPaths::IsValidMinerPath(QString* minerPath)
{
	// TODO make a list of valid miner paths and check that instead
	return minerPath!=nullptr && minerPath!=Data.None && minerPath!=QString("");
}

QList<Algorithm*>* MinerPaths::GetAndInitAlgorithmsMinerPaths(QList<Algorithm*>* algos, ComputeDevice* computeDevice)
{
	QList<Algorithm*>* retAlgos=new QList<Algorithm*>;
	foreach (Algorithm* a, *algos) {
//		Algorithm* algo=new Algorithm(*a);
		a->MinerBinaryPath=GetPathFor(computeDevice, a);
		retAlgos->append(a);
		}
	return retAlgos;
}

QString MinerPaths::NvidiaGroups::CcminerSM21(Enums::AlgorithmType algorithmType)
{
	return algorithmType==Enums::AlgorithmType::CryptoNight
		? Data.CcminerCryptonight
		: Data.CcminerDecred;
}

QString MinerPaths::NvidiaGroups::CcminerSM3X(Enums::AlgorithmType algorithmType)
{
	if (algorithmType==Enums::AlgorithmType::Decred) {
		return Data.CcminerDecred;
		}
	if (algorithmType==Enums::AlgorithmType::CryptoNight) {
		return Data.CcminerCryptonight;
		}
	return Data.CcminerTPruvot;
}

QString MinerPaths::NvidiaGroups::CcminerSM5XOrSM6X(Enums::AlgorithmType algorithmType)
{
	switch (algorithmType) {
		case Enums::AlgorithmType::Decred:
			return Data.CcminerDecred;
		case Enums::AlgorithmType::Lyra2RE:
		case Enums::AlgorithmType::Lyra2REv2:
			return Data.CcminerNanashi;
		case Enums::AlgorithmType::CryptoNight:
			return Data.CcminerCryptonight;
		case Enums::AlgorithmType::Lbry:
		case Enums::AlgorithmType::X11Gost:
		case Enums::AlgorithmType::Blake2s:
		case Enums::AlgorithmType::Skunk:
		case Enums::AlgorithmType::Keccak:
			return Data.CcminerTPruvot;
		case Enums::AlgorithmType::Sia:
		case Enums::AlgorithmType::Nist5:
		case Enums::AlgorithmType::NeoScrypt:
			return Data.CcminerKlausT;
		default:
			return Data.CcminerSp;
		}
}

QString MinerPaths::NvidiaGroups::Ccminer_path(Enums::AlgorithmType algorithmType, Enums::DeviceGroupType nvidiaGroup)
{
	if (nvidiaGroup==Enums::DeviceGroupType::NVIDIA_6_x && algorithmType==Enums::AlgorithmType::CryptoNight) {
		return Data.CcminerTPruvot;
		}
	switch (nvidiaGroup) {
		// sm21 and sm3x no longer have same settings since tpruvot dropped 21 support
		case Enums::DeviceGroupType::NVIDIA_2_1:
			return CcminerSM21(algorithmType);
		case Enums::DeviceGroupType::NVIDIA_3_x:
			return CcminerSM3X(algorithmType);
		// sm5x and sm6x have same settings otherwise
		case Enums::DeviceGroupType::NVIDIA_5_x:
		case Enums::DeviceGroupType::NVIDIA_6_x:
			return CcminerSM5XOrSM6X(algorithmType);
		}
	return Data.None; // should not happen
}

QString MinerPaths::NvidiaGroups::CcminerUnstablePath(Enums::AlgorithmType algorithmType, Enums::DeviceGroupType nvidiaGroup)
{
	// sm5x and sm6x have same settings
	if ((nvidiaGroup==Enums::DeviceGroupType::NVIDIA_5_x || nvidiaGroup==Enums::DeviceGroupType::NVIDIA_6_x)
//		&& (algorithmType==Enums::AlgorithmType::X11Gost || algorithmType==Enums::AlgorithmType::Nist5 || algorithmType==Enums::AlgorithmType::Keccak)
		) {
		return Data.CcminerX11Gost;
		}
	return Data.None; // should not happen
}

QString MinerPaths::AmdGroup::SgminerPath(Enums::AlgorithmType type)
{
	if (type==Enums::AlgorithmType::CryptoNight || type==Enums::AlgorithmType::DaggerHashimoto) {
		return Data.SgminerGm;
		}
	return Data.Sgminer560General;
}

QString MinerPaths::AmdGroup::ClaymorePath(Enums::AlgorithmType type)
{
	switch (type) {
		case Enums::AlgorithmType::Equihash:
			return Data.ClaymoreZcashMiner;
		case Enums::AlgorithmType::CryptoNight:
		case Enums::AlgorithmType::CryptoNightV7:
			return Data.ClaymoreCryptoNightMiner;
		case Enums::AlgorithmType::DaggerHashimoto:
			return Data.ClaymoreDual;
		default:
			return Data.None; // should not happen
		}
	return Data.None; // should not happen
}

QString MinerPaths::Experimental::GetPath(Enums::AlgorithmType algoType, Enums::DeviceGroupType devGroupType)
{
	return devGroupType==Enums::DeviceGroupType::NVIDIA_6_x
		? NvidiaGroups.Ccminer_path(algoType, devGroupType)
		: Data.None;
}

void MinerPaths::InitializePackages() // @todo finish
{
	QList<MinerPathPackage> defaults;
	for (int i=(int)Enums::DeviceGroupType::NONE+1; i<(int)Enums::DeviceGroupType::LAST; i++) {
		QMap<Enums::MinerBaseType, QList<Algorithm*>*>* package=GroupAlgorithms::CreateDefaultsForGroup((Enums::DeviceGroupType)i);
		}
}
