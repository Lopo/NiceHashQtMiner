#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Devices/GroupNames.h"
#include "International.h"
#include "Devices/GroupAlgorithms.h"
#if WITH_NVIDIA
#	include "Devices/CudaDevice.h"
#endif
#if WITH_AMD
#	include "Devices/AmdGpuDevice.h"
#endif
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Configs/Data/DeviceBenchmarkConfig.h"
#include "Miners/Grouping/MinerPaths.h"
#include "Algorithm.h"
#include <QCryptographicHash>


ulong ComputeDevice::Memory3Gb=3221225472;


ComputeDevice::ComputeDevice(int id, QString name, bool enabled, Enums::DeviceGroupType group, bool ethereumCapable, Enums::DeviceType type, QString nameCount, ulong gpuRam)
{
	ID=id;
	Name=name;
	Enabled=enabled;
	DeviceGroupType=group;
	IsEtherumCapale=ethereumCapable;
	DeviceType=type;
	NameCount=nameCount;
	GpuRam=gpuRam;
}

ComputeDevice::ComputeDevice(int id)
{
	ID=id;
	Name="fake_"+QString::number(id);
	NameCount=Name;
	Enabled=true;
	DeviceType=Enums::DeviceType::CPU;
	DeviceGroupType=Enums::DeviceGroupType::NONE;
	IsEtherumCapale=false;
	_Codename="fake";
	_Uuid=GetUuid(ID, GroupNames::GetGroupName(DeviceGroupType, ID), Name, DeviceGroupType);
	GpuRam=0;
}

ComputeDevice::ComputeDevice(int id, QString group, QString name, int threads, ulong affinityMask, int cpuCount)
{
	ID=id;
	Name=name;
	_Threads=threads;
	_AffinityMask=affinityMask;
	Enabled=true;
	DeviceGroupType=Enums::DeviceGroupType::CPU;
	DeviceType=Enums::DeviceType::CPU;
	NameCount=International::GetText("ComputeDevice_Short_Name_CPU").arg(cpuCount);
	_Uuid=GetUuid(ID, GroupNames::GetGroupName(DeviceGroupType, ID), Name, DeviceGroupType);
	AlgorithmSettings=GroupAlgorithms::CreateForDeviceList(this);
	IsEtherumCapale=false;
	GpuRam=0;
}

#if WITH_NVIDIA
ComputeDevice::ComputeDevice(CudaDevice* cudaDevice, Enums::DeviceGroupType group, int gpuCount)
{
	_SM_major=cudaDevice->SM_major;
	_SM_minor=cudaDevice->SM_minor;
	ID=(int)cudaDevice->DeviceID;
	Name=cudaDevice->GetName();
	Enabled=true;
	DeviceGroupType=group;
	IsEtherumCapale=cudaDevice->IsEtherumCapable();
	DeviceType=Enums::DeviceType::NVIDIA;
	NameCount=International::GetText("ComputeDevice_Short_Name_NVIDIA_GPU").arg(gpuCount);
	_Uuid=cudaDevice->UUID;
	AlgorithmSettings=GroupAlgorithms::CreateForDeviceList(this);
	GpuRam=cudaDevice->DeviceGlobalMemory;
}

bool ComputeDevice::IsSM50()
{
	return (_SM_major==5 && !_SM_minor);
}
#endif

#if WITH_AMD
ComputeDevice::ComputeDevice(AmdGpuDevice* amdDevice, int gpuCount, bool isDetectionFallback)
{
	ID=amdDevice->DeviceID();
	_BusID=amdDevice->BusID();
	DeviceGroupType=Enums::DeviceGroupType::AMD_OpenCL;
	Name=amdDevice->DeviceName;
	Enabled=true;
	IsEtherumCapale=amdDevice->IsEtherumCapable();
	DeviceType=Enums::DeviceType::AMD;
	NameCount=International::GetText("ComputeDevice_Short_Name_AMD_GPU").arg(gpuCount);
	_Uuid= isDetectionFallback
		? GetUuid(ID, GroupNames::GetGroupName(DeviceGroupType, ID), Name, DeviceGroupType)
		: amdDevice->UUID;
	_Codename=amdDevice->Codename();
	_InfSection=amdDevice->InfSection;
	AlgorithmSettings=GroupAlgorithms::CreateForDeviceList(this);
	_DriverDisableAlgos=amdDevice->DriverDisableAlgos;
	GpuRam=amdDevice->DeviceGlobalMemory();
}
#endif

QString ComputeDevice::GetFullName()
{
	return International::GetText("ComputeDevice_Full_Device_Name").arg(NameCount, Name);
}

Algorithm* ComputeDevice::GetAlgorithm(Enums::MinerBaseType minerBaseType, Enums::AlgorithmType algorithmType, Enums::AlgorithmType secondaryAlgorithmType)
{
	foreach (Algorithm* a, *AlgorithmSettings) {
		if (a->NiceHashID==algorithmType && a->MinerBaseType==minerBaseType && a->SecondaryNiceHashID==secondaryAlgorithmType) {
			return a;
			}
		}
	return nullptr;
}

void ComputeDevice::CopyBenchmarkSettingsFrom(ComputeDevice* copyBenchCDev)
{
	foreach (Algorithm* copyFromAlgo, *copyBenchCDev->AlgorithmSettings) {
		Algorithm* setAlgo=GetAlgorithm(copyFromAlgo->MinerBaseType, copyFromAlgo->NiceHashID, copyFromAlgo->SecondaryNiceHashID);
		if (setAlgo!=nullptr) {
			setAlgo->BenchmarkSpeed=copyFromAlgo->BenchmarkSpeed;
			setAlgo->SecondaryBenchmarkSpeed=copyFromAlgo->SecondaryBenchmarkSpeed;
			setAlgo->ExtraLaunchParameters=copyFromAlgo->ExtraLaunchParameters;
			setAlgo->LessThreads=copyFromAlgo->LessThreads;
			}
		}
}

void ComputeDevice::SetFromComputeDeviceConfig(ComputeDeviceConfig* config)
{
	if (config!=nullptr && config->UUID==_Uuid) {
		Enabled=config->Enabled;
		}
}

void ComputeDevice::SetAlgorithmDeviceConfig(DeviceBenchmarkConfig* config)
{
	if (config!=nullptr && config->DeviceUUID==_Uuid && config->AlgorithmSettings!=nullptr) {
		AlgorithmSettings=GroupAlgorithms::CreateForDeviceList(this);
		foreach (AlgorithmConfig* conf, *config->AlgorithmSettings) {
			Algorithm* setAlgo=GetAlgorithm(conf->MinerBaseType, conf->NiceHashID, conf->SecondaryNiceHashID);
			if (setAlgo!=nullptr) {
				setAlgo->BenchmarkSpeed=conf->BenchmarkSpeed;
				setAlgo->SecondaryBenchmarkSpeed=conf->SecondaryBenchmarkSpeed;
				setAlgo->ExtraLaunchParameters=conf->ExtraLaunchParameters;
				setAlgo->Enabled=conf->Enabled;
				setAlgo->LessThreads=conf->LessThreads;
				}
			}
		}
}

ComputeDeviceConfig* ComputeDevice::GetComputeDeviceConfig()
{
	ComputeDeviceConfig* ret=new ComputeDeviceConfig;
	ret->Enabled=Enabled;
	ret->Name=Name;
	ret->UUID=_Uuid;
	return ret;
}

DeviceBenchmarkConfig* ComputeDevice::GetAlgorithmDeviceConfig()
{
	DeviceBenchmarkConfig* ret=new DeviceBenchmarkConfig;
	ret->DeviceName=Name;
	ret->DeviceUUID=_Uuid;
	foreach (Algorithm* algo, *AlgorithmSettings) { // init algo settings
		AlgorithmConfig* conf=new AlgorithmConfig;
		conf->Name=algo->AlgorithmStringID;
		conf->NiceHashID=algo->NiceHashID;
		conf->SecondaryNiceHashID=algo->SecondaryNiceHashID;
		conf->MinerBaseType=algo->MinerBaseType;
		conf->MinerName=algo->MinerName;
		conf->BenchmarkSpeed=algo->BenchmarkSpeed;
		conf->SecondaryBenchmarkSpeed=algo->SecondaryBenchmarkSpeed;
		conf->ExtraLaunchParameters=algo->ExtraLaunchParameters;
		conf->Enabled=algo->Enabled;
		conf->LessThreads=algo->LessThreads;
		// TODO probably not needed
		// insert
		ret->AlgorithmSettings->append(conf);
		}
	return ret;
}

QList<Algorithm*>* ComputeDevice::GetAlgorithmSettings()
{
	// hello state
	QList<Algorithm*>* algos=GetAlgorithmSettingsThirdParty(ConfigManager.generalConfig->Use3rdPartyMiners);
	QList<Algorithm*>* retAlgos=MinerPaths::GetAndInitAlgorithmsMinerPaths(algos, this);

	// NVIDIA
#if WITH_NVIDIA
	if (DeviceGroupType==Enums::DeviceGroupType::NVIDIA_5_x || DeviceGroupType==Enums::DeviceGroupType::NVIDIA_6_x) {
		foreach (Algorithm* a, *retAlgos) {
			if (a->MinerBaseType==Enums::MinerBaseType::nheqminer) {
				retAlgos->removeOne(a);
				}
			}
		}
	else if (DeviceType==Enums::DeviceType::NVIDIA) {
		foreach (Algorithm* a, *retAlgos) {
			if (a->MinerBaseType==Enums::MinerBaseType::eqm) {
				retAlgos->removeOne(a);
				}
			}
		}
#endif
	// sort by algo
	std::sort(retAlgos->begin(), retAlgos->end(), [](const Algorithm* a_1, const Algorithm* a_2) { return (a_1->NiceHashID-a_2->NiceHashID)? (a_1->NiceHashID<a_2->NiceHashID) : (((int)a_1->MinerBaseType)<(int)a_2->MinerBaseType); });
	return retAlgos;
}

QList<Algorithm*>* ComputeDevice::GetAlgorithmSettingsFastest() //done
{
	// hello state
	QList<Algorithm*> algosTmp=*GetAlgorithmSettings();
	QMap<Enums::AlgorithmType, Algorithm*> sortDict;
	foreach (Algorithm* algo, algosTmp) {
		Enums::AlgorithmType algoKey=algo->NiceHashID;
		if (sortDict.contains(algoKey)) {
			if (sortDict[algoKey]->BenchmarkSpeed<algo->BenchmarkSpeed) {
				sortDict[algoKey]=algo;
				}
			}
		else {
			sortDict[algoKey]=algo;
			}
		}
	return new QList<Algorithm*>(sortDict.values());
}

QList<Algorithm*>* ComputeDevice::GetAlgorithmSettingsThirdParty(Enums::Use3rdPartyMiners use3rdParty) //done
{
	if (use3rdParty==Enums::Use3rdPartyMiners::YES) {
		return AlgorithmSettings;
		}
	QList<Enums::MinerBaseType> thirdPartyMiners={
		Enums::MinerBaseType::Claymore,
		Enums::MinerBaseType::OptiminerAMD,
		Enums::MinerBaseType::EWBF,
		Enums::MinerBaseType::Prospector,
		Enums::MinerBaseType::dtsm
		};
	QList<Algorithm*>* ret=new QList<Algorithm*>;
	foreach (Algorithm* a, *AlgorithmSettings) {
		if (thirdPartyMiners.contains(a->MinerBaseType)) {
			ret->append(a);
			}
		}
	return ret;
}

QString ComputeDevice::GetUuid(int id, QString group, QString name, Enums::DeviceGroupType deviceGroupType)
{
	QString mixedAttr=QString::number(id)+group+name+QString::number((int)deviceGroupType);
	QByteArray hashedBytes=QCryptographicHash::hash(mixedAttr.toLatin1(), QCryptographicHash::Sha256);
	// GEN indicates the UUID has been generated and cannot be presumed to be imutablem
	return "GEN-"+QString(hashedBytes.toHex());
}

bool ComputeDevice::IsAlgorithmSettingsInitialized()
{
	return AlgorithmSettings!=nullptr;
}
