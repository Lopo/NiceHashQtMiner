#define __ConfigManager_cpp__
#include "ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Configs/ConfigJsonFile/GeneralConfigFile.h"
#include "Configs/ConfigJsonFile/DeviceBenchmarkConfigFile.h"
#include "Devices/ComputeDeviceManager.h"
#include "Utils/Helpers.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include <QApplication>


const QString ConfigManager::Tag="ConfigManager";
GeneralConfig* ConfigManager::generalConfig=new GeneralConfig;
bool ConfigManager::_isGeneralConfigFileInit=false;
bool ConfigManager::_isNewVersion=false;
GeneralConfigFile* ConfigManager::generalConfigFile=new GeneralConfigFile;
QMap<QString, DeviceBenchmarkConfigFile*>* ConfigManager::BenchmarkConfigFiles=new QMap<QString, DeviceBenchmarkConfigFile*>;
GeneralConfig* ConfigManager::_generalConfigBackup=new GeneralConfig;
QMap<QString, DeviceBenchmarkConfig*>* ConfigManager::_benchmarkConfigsBackup=new QMap<QString, DeviceBenchmarkConfig*>;


void ConfigManager::InitializeConfig()
{
	// init defaults
	generalConfig->SetDefaults();
	generalConfig->hwid=Helpers::GetCpuID();
	// if exists load file
	GeneralConfig* fromFile=nullptr;
	if (generalConfigFile->IsFileExists()) {
		fromFile=generalConfigFile->ReadFile();
		}
	// just in case
	if (fromFile!=nullptr) {
		// set config loaded from file
		_isGeneralConfigFileInit=true;
//		if (generalConfig!=nullptr) {delete generalConfig;}
		generalConfig=fromFile;
		if (generalConfig->ConfigFileVersion==nullptr
			|| generalConfig->ConfigFileVersion->toString()!=qApp->applicationVersion()
			) {
			if (generalConfig->ConfigFileVersion==nullptr) {
				Helpers::ConsolePrint(Tag, "Loaded Config file no version detected falling back to defaults.");
				generalConfig->SetDefaults();
				}
			Helpers::ConsolePrint(Tag, "Config file is from an older version of NiceHashQtMiner..");
			_isNewVersion=true;
			generalConfigFile->CreateBackup();
			}
		generalConfig->FixSettingBounds();
		}
	else {
		GeneralConfigFileCommit();
		}
}

bool ConfigManager::GeneralConfigIsFileExist()
{
	return _isGeneralConfigFileInit;
}

void ConfigManager::CreateBackup()
{
//	if (_generalConfigBackup!=nullptr) {delete _generalConfigBackup;}
	_generalConfigBackup=GeneralConfig::fromJson(generalConfig->asJson()); // MemoryHelper.DeepClone(generalConfig)
//	if (_benchmarkConfigsBackup!=nullptr) {delete _benchmarkConfigsBackup;}
	_benchmarkConfigsBackup=new QMap<QString, DeviceBenchmarkConfig*>;
	foreach (ComputeDevice* cDev, *ComputeDeviceManager.Avaliable.AllAvailableDevices) {
		(*_benchmarkConfigsBackup)[cDev->Uuid()]=cDev->GetAlgorithmDeviceConfig();
		}
}

void ConfigManager::RestoreBackup()
{
	// restore general
//	if (generalConfig) {delete generalConfig;}
	generalConfig=_generalConfigBackup;
	if (generalConfig->LastDevicesSettup!=nullptr) {
		foreach (ComputeDevice* cDev, *ComputeDeviceManager.Avaliable.AllAvailableDevices) {
			foreach (ComputeDeviceConfig* conf, *generalConfig->LastDevicesSettup) {
				cDev->SetFromComputeDeviceConfig(conf);
				}
			}
		}
	// restore benchmarks
	foreach (ComputeDevice* cDev, *ComputeDeviceManager.Avaliable.AllAvailableDevices) {
		if (_benchmarkConfigsBackup!=nullptr && _benchmarkConfigsBackup->contains(cDev->Uuid())) {
			cDev->SetAlgorithmDeviceConfig((*_benchmarkConfigsBackup)[cDev->Uuid()]);
			}
		}
}

bool ConfigManager::IsRestartNeeded()
{
	return //generalConfig->DebugConsole!=_generalConfigBackup->DebugConsole ||
#if WITH_NVIDIA
		generalConfig->NVIDIAP0State!=_generalConfigBackup->NVIDIAP0State ||
#endif
		generalConfig->LogToFile!=_generalConfigBackup->LogToFile
		|| generalConfig->SwitchMinSecondsFixed!=_generalConfigBackup->SwitchMinSecondsFixed
#if WITH_AMD
		|| generalConfig->SwitchMinSecondsAMD!=_generalConfigBackup->SwitchMinSecondsAMD
#endif
		|| generalConfig->SwitchMinSecondsDynamic!=_generalConfigBackup->SwitchMinSecondsDynamic
		|| generalConfig->MinerAPIQueryInterval!=_generalConfigBackup->MinerAPIQueryInterval
//		|| generalConfig->DisableWindowsErrorReporting!=_generalConfigBackup->DisableWindowsErrorReporting
		;
}

void ConfigManager::GeneralConfigFileCommit()
{
	generalConfig->LastDevicesSettup->clear();
	foreach (ComputeDevice* cDev, *ComputeDeviceManager.Avaliable.AllAvailableDevices) {
		generalConfig->LastDevicesSettup->append(cDev->GetComputeDeviceConfig());
		}
	generalConfigFile->Commit(generalConfig);
}

void ConfigManager::CommitBenchmarks()
{
	foreach (ComputeDevice* cDev, *ComputeDeviceManager.Avaliable.AllAvailableDevices) {
		QString devUuid=cDev->Uuid();
		if (BenchmarkConfigFiles->contains(devUuid)) {
			(*BenchmarkConfigFiles)[devUuid]->Commit(cDev->GetAlgorithmDeviceConfig());
			}
		else {
			(*BenchmarkConfigFiles)[devUuid]=new DeviceBenchmarkConfigFile(devUuid);
			(*BenchmarkConfigFiles)[devUuid]->Commit(cDev->GetAlgorithmDeviceConfig());
			}
		}
}

void ConfigManager::AfterDeviceQueryInitialization()
{
	{ // extra check (probably will never happen but just in case)
		QList<ComputeDevice*> invalidDevices;
		foreach (ComputeDevice* cDev, *ComputeDeviceManager.Avaliable.AllAvailableDevices) {
			if (!cDev->IsAlgorithmSettingsInitialized()) {
				Helpers::ConsolePrint(Tag, "CRITICAL ISSUE!!! Device has AlgorithmSettings == null. Will remove");
				invalidDevices.append(cDev);
				}
			}
		// remove invalids
		foreach (ComputeDevice* invalid, invalidDevices) {
			ComputeDeviceManager.Avaliable.AllAvailableDevices->removeOne(invalid);
			}
	}
	// set enabled/disabled devs
	foreach (ComputeDevice* cDev, *ComputeDeviceManager.Avaliable.AllAvailableDevices) {
		foreach (ComputeDeviceConfig* devConf, *generalConfig->LastDevicesSettup) {
			cDev->SetFromComputeDeviceConfig(devConf);
			}
		}
	// create/init device benchmark configs files and configs
	foreach (ComputeDevice* cDev, *ComputeDeviceManager.Avaliable.AllAvailableDevices) {
		QString keyUuid=cDev->Uuid();
		BenchmarkConfigFiles->insert(keyUuid, new DeviceBenchmarkConfigFile(keyUuid));
		{ // init
			DeviceBenchmarkConfig* currentConfig=nullptr;
			if ((*BenchmarkConfigFiles)[keyUuid]->IsFileExists()) {
				currentConfig=(*BenchmarkConfigFiles)[keyUuid]->ReadFile();
				}
			// config exists and file load success set from file
			if (currentConfig!=nullptr) {
				cDev->SetAlgorithmDeviceConfig(currentConfig);
				// if new version create backup
				if (_isNewVersion) {
					(*BenchmarkConfigFiles)[keyUuid]->CreateBackup();
					}
				}
			else {
				// no config file or not loaded, create new
				(*BenchmarkConfigFiles)[keyUuid]->Commit(cDev->GetAlgorithmDeviceConfig());
				}
		}
		}
	// save settings
	GeneralConfigFileCommit();
}
