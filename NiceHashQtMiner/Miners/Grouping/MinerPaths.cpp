#define __MinerPaths_cpp__
#include "Miners/Grouping/MinerPaths.h"
#include "Algorithms/Algorithm.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Utils/Helpers.h"
#include "Devices/Algorithms/GroupAlgorithms.h"
#include <QMetaEnum>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>


MinerPath::MinerPath(Enums::AlgorithmType algo, QString path)
{
	Algorithm=algo;
	Path=path;
	Name=QMetaEnum::fromType<Enums::AlgorithmType>().valueToKey((int)Algorithm);
}

MinerPath::MinerPath(QJsonObject obj)
{
	Algorithm=(Enums::AlgorithmType)obj.value("Algorithm").toInt();
	Path=obj.value("Path").toString();
	Name=obj.value("Name").toString(QMetaEnum::fromType<Enums::AlgorithmType>().valueToKey((int)Algorithm));
}

QJsonObject MinerPath::asJsonObject()
{
	QJsonObject o;
	o.insert("Algorithm", (int)Algorithm);
	o.insert("Path", Path);
	o.insert("Name", Name);
	return o;
}

MinerTypePath::MinerTypePath(Enums::MinerBaseType type, QList<MinerPath> paths)
{
	Type=type;
	Algorithms=paths;
	Name=QMetaEnum::fromType<Enums::MinerBaseType>().valueToKey((int)type);
}

MinerTypePath::MinerTypePath(QJsonObject obj)
{
	Type=(Enums::MinerBaseType)obj.value("Type").toInt();
	foreach (QJsonValue a, obj.value("Algorithms").toArray()) {
		Algorithms.append(MinerPath(a.toObject()));
		}
	Name=obj.value("Name").toString(QMetaEnum::fromType<Enums::MinerBaseType>().valueToKey((int)Type));
}

QJsonObject MinerTypePath::asJsonObject()
{
	QJsonObject o;
	o.insert("Type", (int)Type);
	o.insert("Name", Name);
	QJsonArray aA;
	foreach (MinerPath mp, Algorithms) {
		aA.append(mp.asJsonObject());
		}
	o.insert("Algorithms", aA);
	return o;
}

MinerPathPackage::MinerPathPackage(Enums::DeviceGroupType type, QList<MinerTypePath*> paths)
{
	DeviceType=type;
	MinerTypes=paths;
	Name=QMetaEnum::fromType<Enums::DeviceGroupType>().valueToKey((int)DeviceType);
}


const QString MinerPaths::Data::Bin="bin";

#if WITH_NVIDIA
const QString MinerPaths::Data::CcminerDecred=Bin+"/ccminer_decred/ccminer";

const QString MinerPaths::Data::CcminerNanashi=Bin+"/ccminer_nanashi/ccminer";
const QString MinerPaths::Data::CcminerNeoscrypt=Bin+"/ccminer_neoscrypt/ccminer";
const QString MinerPaths::Data::CcminerSp=Bin+"/ccminer_sp/ccminer";
const QString MinerPaths::Data::CcminerTPruvot=Bin+"/ccminer_tpruvot/ccminer";
const QString MinerPaths::Data::CcminerCryptonight=Bin+"/ccminer_cryptonight/ccminer";
const QString MinerPaths::Data::CcminerX11Gost=Bin+"/ccminer_x11gost/ccminer";
const QString MinerPaths::Data::CcminerKlausT=Bin+"/ccminer_klaust/ccminer";
#endif
const QString MinerPaths::Data::Ethminer=Bin+"/ethminer/ethminer";
#if WITH_AMD
const QString MinerPaths::Data::Sgminer560General=Bin+"/sgminer-5-6-0-general/sgminer";
const QString MinerPaths::Data::SgminerGm=Bin+"/sgminer-gm/sgminer";
const QString MinerPaths::Data::Avermore=Bin+"/avermore/sgminer";
#endif
const QString MinerPaths::Data::NhEqMiner=Bin+"/nheqminer_v0.4b/nheqminer";
#if WITH_NVIDIA
const QString MinerPaths::Data::Excavator=Bin+"/excavator/excavator";
#endif

const QString MinerPaths::Data::XmrStackCpuMiner=Bin+"/xmr-stack-cpu/xmr-stak";
#if WITH_AMD
const QString MinerPaths::Data::XmrStakAmd=Bin+"/xmr-stak-amd/xmr-stak-amd";
#endif
const QString MinerPaths::Data::XmrStak=Bin+"/xmr-stak/xmr-stak";
const QString MinerPaths::Data::Xmrig=Bin+"/xmrig/xmrig";
#if WITH_AMD
const QString MinerPaths::Data::XmrigAmd=Bin+"/xmrig/xmrig-amd";
#endif
#if WITH_NVIDIA
const QString MinerPaths::Data::XmrigNvidia=Bin+"/xmrig/xmrig-nvidia";
#endif
const QString MinerPaths::Data::XmrStakHeavy=Bin+"/xmr-stak_heavy/xmr-stak";

const QString MinerPaths::Data::CpuMiner=Bin+"/cpuminer_opt/cpuminer";

const QString MinerPaths::Data::None="";

const QString MinerPaths::Data::Bin3rdParty="bin_3rdparty";
#if WITH_AMD
const QString MinerPaths::Data::ClaymoreZcashMiner=Bin3rdParty+"/claymore_zcash/zecminer64";
const QString MinerPaths::Data::ClaymoreCryptoNightMiner=Bin3rdParty+"/claymore_cryptonight/nsgpucnminer";
#endif
const QString MinerPaths::Data::OptiminerZcashMiner=Bin3rdParty+"/optiminer_zcash/optiminer-zcash";
const QString MinerPaths::Data::ClaymoreDual=Bin3rdParty+"/claymore_dual/ethdcrminer64";
#if WITH_NVIDIA
const QString MinerPaths::Data::Ewbf=Bin3rdParty+"/ewbf/miner";
#endif
const QString MinerPaths::Data::Prospector=Bin3rdParty+"/prospector/prospector";
#if WITH_NVIDIA
const QString MinerPaths::Data::Dtsm=Bin3rdParty+"/dtsm/zm";
#endif

QList<MinerPathPackage*>* MinerPaths::MinerPathPackages=new QList<MinerPathPackage*>;
QList<Enums::MinerBaseType>* MinerPaths::ConfigurableMiners=new QList<Enums::MinerBaseType>({
	Enums::MinerBaseType::ccminer,
	Enums::MinerBaseType::sgminer
	});


QString MinerPaths::GetPathFor(Enums::MinerBaseType minerBaseType, Enums::AlgorithmType algoType, Enums::DeviceGroupType devGroupType, bool def/*=false*/)
{
	if (!def & ConfigurableMiners->contains(minerBaseType)) {
		// Override with internals
		foreach (MinerPathPackage* mpp, *MinerPathPackages) {
			if (mpp->DeviceType!=devGroupType) {
				continue;
				}
			foreach (MinerTypePath* mtp, mpp->MinerTypes) {
				if (mtp->Type!=minerBaseType) {
					continue;
					}
				foreach (MinerPath mp, mtp->Algorithms) {
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
	// Temp workaround
	if (minerBaseType==Enums::MinerBaseType::XmrStak && algoType==Enums::AlgorithmType::CryptoNightHeavy) {
		return Data.XmrStakHeavy;
		}

	switch (minerBaseType) {
#if WITH_NVIDIA
		case Enums::MinerBaseType::ccminer:
			return NvidiaGroups.Ccminer_path(algoType, devGroupType);
#endif
#if WITH_AMD
		case Enums::MinerBaseType::sgminer:
			return AmdGroup.SgminerPath(algoType);
#endif
		case Enums::MinerBaseType::nheqminer:
			return Data.NhEqMiner;
		case Enums::MinerBaseType::ethminer:
			return Data.Ethminer;
		case Enums::MinerBaseType::Claymore:
			return AmdGroup.ClaymorePath(algoType);
#if WITH_AMD
		case Enums::MinerBaseType::OptiminerAMD:
			return Data.OptiminerZcashMiner;
#endif
#if WITH_NVIDIA
		case Enums::MinerBaseType::excavator:
			return Data.Excavator;
#endif
		case Enums::MinerBaseType::XmrStak:
			return Data.XmrStak;
#if WITH_NVIDIA
		case Enums::MinerBaseType::ccminer_alexis:
			return NvidiaGroups.CcminerUnstablePath(algoType, devGroupType);
		case Enums::MinerBaseType::experimental:
			return Experimental.GetPath(algoType, devGroupType);
		case Enums::MinerBaseType::EWBF:
			return Data.Ewbf;
#endif
		case Enums::MinerBaseType::Prospector:
			return Data.Prospector;
		case Enums::MinerBaseType::Xmrig:
			return Xmrig.GetPath(devGroupType);
#if WITH_NVIDIA
		case Enums::MinerBaseType::dtsm:
			return Data.Dtsm;
#endif
		case Enums::MinerBaseType::cpuminer:
			return Data.CpuMiner;
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

#if WITH_NVIDIA
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
		case Enums::AlgorithmType::Lyra2z:
		case Enums::AlgorithmType::X16R:
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
		default:
			return Data.None; // should not happen
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
#endif
#if WITH_AMD
QString MinerPaths::AmdGroup::SgminerPath(Enums::AlgorithmType type)
{
	switch (type) {
		case Enums::AlgorithmType::CryptoNight:
		case Enums::AlgorithmType::DaggerHashimoto:
			return Data.SgminerGm;
		case Enums::AlgorithmType::X16R:
			return Data.Avermore;
		default:
			return Data.Sgminer560General;
		}
}
#endif
QString MinerPaths::AmdGroup::ClaymorePath(Enums::AlgorithmType type)
{
	switch (type) {
#if WITH_AMD
		case Enums::AlgorithmType::Equihash:
			return Data.ClaymoreZcashMiner;
		case Enums::AlgorithmType::CryptoNight:
		case Enums::AlgorithmType::CryptoNightV7:
			return Data.ClaymoreCryptoNightMiner;
#endif
		case Enums::AlgorithmType::DaggerHashimoto:
			return Data.ClaymoreDual;
		default:
			return Data.None; // should not happen
		}
}
#if WITH_NVIDIA
QString MinerPaths::Experimental::GetPath(Enums::AlgorithmType algoType, Enums::DeviceGroupType devGroupType)
{
	return devGroupType==Enums::DeviceGroupType::NVIDIA_6_x
		? NvidiaGroups.Ccminer_path(algoType, devGroupType)
		:
			Data.None;
}
#endif

QString MinerPaths::Xmrig::GetPath(Enums::DeviceGroupType devGroupType)
{
	switch (devGroupType) {
		case Enums::DeviceGroupType::CPU:
			return Data.Xmrig;
#if WITH_AMD
		case Enums::DeviceGroupType::AMD_OpenCL:
			return Data.XmrigAmd;
#endif
#if WITH_NVIDIA
		case Enums::DeviceGroupType::NVIDIA_2_1:
		case Enums::DeviceGroupType::NVIDIA_3_x:
		case Enums::DeviceGroupType::NVIDIA_5_x:
		case Enums::DeviceGroupType::NVIDIA_6_x:
			return Data.XmrigNvidia;
#endif
		default:
			return Data.None;
		}
	return Data.None;
}

void MinerPaths::InitializePackages() // @todo finish & verify
{
	QList<MinerPathPackage*> defaults;
	for (int i=(int)Enums::DeviceGroupType::NONE+1; i<(int)Enums::DeviceGroupType::LAST; i++) {
		QMap<Enums::MinerBaseType, QList<Algorithm*>*>* package=GroupAlgorithms::CreateDefaultsForGroup((Enums::DeviceGroupType)i);
		QList<MinerTypePath*> minerTypePaths;
		foreach (Enums::MinerBaseType type, *ConfigurableMiners) {
			if (package==nullptr || !package->contains(type)) {
				continue;
				}
			QList<MinerPath> minerPaths;
			foreach (Algorithm* algo, *package->value(type)) { // @todo finish
				minerPaths.append(MinerPath(algo->NiceHashID, GetPathFor(type, algo->NiceHashID, (Enums::DeviceGroupType)i, true)));
				}
			minerTypePaths.append(new MinerTypePath(type, minerPaths));
			}
		if (minerTypePaths.count()) {
			defaults.append(new MinerPathPackage((Enums::DeviceGroupType)i, minerTypePaths));
			}
		}

	foreach (MinerPathPackage* pack, defaults) {
		QString packageName="MinerPathPackage_"+pack->Name;
		MinerPathPackageFile packageFile(packageName);
		MinerPathPackage* readPack=packageFile.ReadFile();
		if (readPack==nullptr) {
			// read has failed
			Helpers::ConsolePrint("MinerPaths", "Creating internal paths config "+packageName);
			MinerPathPackages->append(pack);
			packageFile.Commit(pack);
			}
		else {
			Helpers::ConsolePrint("MinerPaths", "Loading internal paths config "+packageName);
			bool isChange=false;
			foreach (MinerTypePath* miner, pack->MinerTypes) {
				MinerTypePath* readMiner=nullptr;
				foreach (MinerTypePath* x, readPack->MinerTypes) {
					if (x->Type==miner->Type) {
						readMiner=x;
						break;
						}
					}
				if (readMiner!=nullptr) {
					// file contains miner type
					foreach (MinerPath algo, miner->Algorithms) {
						bool e=false;
						foreach (MinerPath x, readMiner->Algorithms) {
							if (x.Algorithm==algo.Algorithm) {
								e=true;
								break;
								}
							}
						if (!e) {
							// file does not contain algo on this miner
							Helpers::ConsolePrint("PATHS", QString("Algorithm %1 not found in miner %2 on device %3. Adding default").arg(algo.Name).arg(miner->Name).arg(pack->Name));
							readMiner->Algorithms.append(algo);
							isChange=true;
							}
						}
					}
				else {
					// file does not contain miner type
					Helpers::ConsolePrint("PATHS", QString("Miner %1 not found on device %2").arg(miner->Name).arg(pack->Name));
					readPack->MinerTypes.append(miner);
					isChange=true;
					}
				}
			MinerPathPackages->append(readPack);
			if (isChange) {
				packageFile.Commit(readPack);
				}
			}
		}
}

MinerPathPackage* MinerPathPackage::fromJson(QString json)
{
	QJsonObject o=QJsonDocument::fromJson(json.toLatin1()).object();
	Enums::DeviceGroupType type;
	QList<MinerTypePath*> paths;
	if (o.value("DeviceType")!=QJsonValue::Undefined) {
		type=(Enums::DeviceGroupType)o.value("DeviceType").toInt();
		}
	else {
		return nullptr;
		}
	if (o.value("MinerTypes")!=QJsonValue::Undefined) {
		QJsonArray mtA=o.value("MinerTypes").toArray();
		foreach (QJsonValue mtV, mtA) {
			paths.append(new MinerTypePath(mtV.toObject()));
			}
		}
	else {
		return nullptr;
		}
	MinerPathPackage* ret=new MinerPathPackage(type, paths);
	ret->Name=o.value("Name").toString(QMetaEnum::fromType<Enums::DeviceGroupType>().valueToKey((int)type));
	return ret;
}

QString MinerPathPackage::asJson(QJsonDocument::JsonFormat format)
{
	QJsonObject o;
	o.insert("Name", Name);
	o.insert("DeviceType", (int)DeviceType);
	QJsonArray mtA;
	foreach (MinerTypePath* p, MinerTypes) {
		mtA.append(p->asJsonObject());
		}
	o.insert("MinerTypes", mtA);
	return QString(QJsonDocument(o).toJson(format));
}
