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
#include "Algorithms/Algorithm.h"
#include "Algorithms/DualAlgorithm.h"
#include "Configs/Data/DualAlgorithmConfig.h"
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
	Uuid_=GetUuid(ID, GroupNames::GetGroupName(DeviceGroupType, ID), Name, DeviceGroupType);
	GpuRam=0;
}

QString ComputeDevice::GetFullName()
{
	return International::GetText("ComputeDevice_Full_Device_Name").arg(NameCount, Name);
}

Algorithm* ComputeDevice::GetAlgorithm(Algorithm* modelAlgo)
{
	return GetAlgorithm(modelAlgo->MinerBaseType, modelAlgo->NiceHashID, modelAlgo->SecondaryNiceHashID());
}

Algorithm* ComputeDevice::GetAlgorithm(Enums::MinerBaseType minerBaseType, Enums::AlgorithmType algorithmType, Enums::AlgorithmType secondaryAlgorithmType)
{
	foreach (Algorithm* a, *AlgorithmSettings) {
		if (a->NiceHashID==algorithmType && a->MinerBaseType==minerBaseType && a->SecondaryNiceHashID()==secondaryAlgorithmType) {
			return a;
			}
		}
	return nullptr;
}

void ComputeDevice::CopyBenchmarkSettingsFrom(ComputeDevice* copyBenchCDev)
{
	foreach (Algorithm* copyFromAlgo, *copyBenchCDev->AlgorithmSettings) {
		Algorithm* setAlgo=GetAlgorithm(copyFromAlgo);
		if (setAlgo!=nullptr) {
			setAlgo->BenchmarkSpeed(copyFromAlgo->BenchmarkSpeed());
			setAlgo->ExtraLaunchParameters=copyFromAlgo->ExtraLaunchParameters;
			setAlgo->LessThreads=copyFromAlgo->LessThreads;
			setAlgo->PowerUsage(copyFromAlgo->PowerUsage());
			DualAlgorithm* dualSA=qobject_cast<DualAlgorithm*>(setAlgo);
			DualAlgorithm* dualCFA=qobject_cast<DualAlgorithm*>(copyFromAlgo);
			if (dualSA!=nullptr && dualCFA!=nullptr) {
				dualSA->SecondaryBenchmarkSpeed(dualCFA->SecondaryBenchmarkSpeed());
				}
			}
		}
}

void ComputeDevice::CopyTuningSettingsFrom(ComputeDevice* copyTuningCDev)
{
	foreach (Algorithm* acopyFromAlgo, *copyTuningCDev->AlgorithmSettings) {
		DualAlgorithm* copyFromAlgo=qobject_cast<DualAlgorithm*>(acopyFromAlgo);
		if (copyFromAlgo==nullptr) {
			continue;
			}
		DualAlgorithm* setAlgo=qobject_cast<DualAlgorithm*>(GetAlgorithm(copyFromAlgo));
		if (setAlgo!=nullptr) {
			setAlgo->IntensitySpeeds=copyFromAlgo->IntensitySpeeds;
			setAlgo->SecondaryIntensitySpeeds=copyFromAlgo->SecondaryIntensitySpeeds;
			setAlgo->TuningStart=copyFromAlgo->TuningStart;
			setAlgo->TuningEnd=copyFromAlgo->TuningEnd;
			setAlgo->TuningInterval=copyFromAlgo->TuningInterval;
			setAlgo->TuningEnabled=copyFromAlgo->TuningEnabled;
			setAlgo->IntensityPowers=copyFromAlgo->IntensityPowers;
			setAlgo->UseIntensityPowers=copyFromAlgo->UseIntensityPowers;
			setAlgo->IntensityUpToDate=false;
			}
		}
}

void ComputeDevice::SetFromComputeDeviceConfig(ComputeDeviceConfig* config)
{
	if (config!=nullptr && config->UUID==Uuid_) {
		Enabled=config->Enabled;
		}
}

void ComputeDevice::SetAlgorithmDeviceConfig(DeviceBenchmarkConfig* config)
{
	if (config!=nullptr && config->DeviceUUID==Uuid_ && config->AlgorithmSettings!=nullptr) {
		AlgorithmSettings=GroupAlgorithms::CreateForDeviceList(this);
		foreach (AlgorithmConfig* conf, *config->AlgorithmSettings) {
			Algorithm* setAlgo=GetAlgorithm(conf->MinerBaseType, conf->NiceHashID, conf->SecondaryNiceHashID);
			if (setAlgo!=nullptr) {
				setAlgo->BenchmarkSpeed(conf->BenchmarkSpeed);
				setAlgo->ExtraLaunchParameters=conf->ExtraLaunchParameters;
				setAlgo->Enabled=conf->Enabled;
				setAlgo->LessThreads=conf->LessThreads;
				setAlgo->PowerUsage(conf->PowerUsage);
				DualAlgorithm* dualSA=qobject_cast<DualAlgorithm*>(GetAlgorithm(setAlgo));
				if (dualSA!=nullptr) {
					dualSA->SecondaryBenchmarkSpeed(conf->SecondaryBenchmarkSpeed);
					DualAlgorithmConfig* dualConf=nullptr;
					if (config->DualAlgorithmSettings!=nullptr) {
						foreach (DualAlgorithmConfig* dac, *config->DualAlgorithmSettings) {
							if (dac->SecondaryNiceHashID==dualSA->SecondaryNiceHashID()) {
								dualConf=dac;
								}
							}
						}
					if (dualConf!=nullptr) {
						dualConf->FixSettingsBounds();
						dualSA->IntensitySpeeds=dualConf->IntensitySpeeds;
						dualSA->SecondaryIntensitySpeeds=dualConf->SecondaryIntensitySpeeds;
						dualSA->TuningEnabled=dualConf->TuningEnabled;
						dualSA->TuningStart=dualConf->TuningStart;
						dualSA->TuningEnd=dualConf->TuningEnd;
						dualSA->TuningInterval=dualConf->TuningInterval;
						dualSA->IntensityPowers=dualConf->IntensityPowers;
						dualSA->UseIntensityPowers=dualConf->UseIntensityPowers;
						}
					}
				}
			}
		}
}

ComputeDeviceConfig* ComputeDevice::GetComputeDeviceConfig()
{
	ComputeDeviceConfig* ret=new ComputeDeviceConfig;
	ret->Enabled=Enabled;
	ret->Name=Name;
	ret->UUID=Uuid_;
	return ret;
}

DeviceBenchmarkConfig* ComputeDevice::GetAlgorithmDeviceConfig()
{
	DeviceBenchmarkConfig* ret=new DeviceBenchmarkConfig;
	ret->DeviceName=Name;
	ret->DeviceUUID=Uuid_;
	foreach (Algorithm* algo, *AlgorithmSettings) { // init algo settings
		AlgorithmConfig* conf=new AlgorithmConfig;
		conf->Name=algo->AlgorithmStringID();
		conf->NiceHashID=algo->NiceHashID;
		conf->MinerBaseType=algo->MinerBaseType;
		conf->MinerName=algo->MinerName;
		conf->BenchmarkSpeed=algo->BenchmarkSpeed();
		conf->ExtraLaunchParameters=algo->ExtraLaunchParameters;
		conf->Enabled=algo->Enabled;
		conf->LessThreads=algo->LessThreads;
		conf->PowerUsage=algo->PowerUsage();
		// insert
		ret->AlgorithmSettings->append(conf);
		DualAlgorithm* dualAlgo=qobject_cast<DualAlgorithm*>(GetAlgorithm(algo));
		if (dualAlgo!=nullptr) {
			conf->SecondaryNiceHashID=dualAlgo->SecondaryNiceHashID();
			conf->SecondaryBenchmarkSpeed=dualAlgo->SecondaryBenchmarkSpeed();

			DualAlgorithmConfig* dualConf=new DualAlgorithmConfig;
			dualConf->Name=algo->AlgorithmStringID();
			dualConf->SecondaryNiceHashID=dualAlgo->SecondaryNiceHashID();
			dualConf->IntensitySpeeds=dualAlgo->IntensitySpeeds;
			dualConf->SecondaryIntensitySpeeds=dualAlgo->SecondaryIntensitySpeeds;
			dualConf->TuningEnabled=dualAlgo->TuningEnabled;
			dualConf->TuningStart=dualAlgo->TuningStart;
			dualConf->TuningEnd=dualAlgo->TuningEnd;
			dualConf->TuningInterval=dualAlgo->TuningInterval;
			dualConf->IntensityPowers=dualAlgo->IntensityPowers;
			dualConf->UseIntensityPowers=dualAlgo->UseIntensityPowers;

			ret->DualAlgorithmSettings->append(dualConf);
			}
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
	std::sort(retAlgos->begin(), retAlgos->end(), [](const Algorithm* a_1, const Algorithm* a_2) { return (a_1->NiceHashID-a_2->NiceHashID)? (a_1->NiceHashID<a_2->NiceHashID) : (((int)a_1->MinerBaseType-(int)a_2->MinerBaseType)? a_1->MinerBaseType<a_2->MinerBaseType : a_1->SecondaryNiceHashID()<a_2->SecondaryNiceHashID()); });
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
			if (sortDict[algoKey]->BenchmarkSpeed()<algo->BenchmarkSpeed()) {
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

bool ComputeDevice::Equals(const ComputeDevice* other)
{
	return ID==other->ID && DeviceGroupType==other->DeviceGroupType && DeviceType==other->DeviceType;
}

bool ComputeDevice::operator==(const ComputeDevice& right)
{
	return Equals(&right);
}

bool ComputeDevice::operator!=(const ComputeDevice& right)
{
	return !Equals(&right);
}
