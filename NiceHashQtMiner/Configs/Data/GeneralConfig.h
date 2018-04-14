#ifndef CONFIGS_DATA_GENERALCONFIG_H_
#define CONFIGS_DATA_GENERALCONFIG_H_

#include "config.h"
#include "Configs/Data/ComputeDeviceConfig.h"
#include "Configs/Data/DeviceDetectionConfig.h"
#include "Configs/Data/BenchmarkTimeLimitsConfig.h"
#include "Switching/Interval.h"
#include "Enums.h"
#include <QVersionNumber>
#include <QJsonDocument>


class GeneralConfig
{
public:
	QVersionNumber* ConfigFileVersion;
	Enums::LanguageType Language=Enums::LanguageType::En;
	QString DisplayCurrency="USD";

//	bool DebugConsole=false;
	QString BitcoinAddress="";
	QString WorkerName="worker1";
	Enums::TimeUnitType TimeUnit=Enums::TimeUnitType::Day;
	QString IFTTTKey="";
	int ServiceLocation=0;
	bool AutoStartMining=false;
//	bool HideMiningWindows=false;
	bool MinimizeToTray=false;

//	bool MinimizeMiningWindows=false;

	Enums::CPUExtensionType ForceCPUExtension=Enums::CPUExtensionType::Automatic;

	int SwitchMinSecondsFixed=90; // @deprecated Use SwitchSmaTimeChangeSeconds
	int SwitchMinSecondsDynamic=30; // @deprecated Use SwitchSmaTimeChangeSeconds
#if WITH_AMD
	int SwitchMinSecondsAMD=60; // @deprecated Use SwitchSmaTimeChangeSeconds
#endif
	double SwitchProfitabilityThreshold=0.05; // percent
	int MinerAPIQueryInterval=5;
	int MinerRestartDelayMS=500;

	BenchmarkTimeLimitsConfig* BenchmarkTimeLimits=new BenchmarkTimeLimitsConfig;

	DeviceDetectionConfig* DeviceDetection=new DeviceDetectionConfig;

#if WITH_AMD
	bool DisableAMDTempControl=true;
#endif
	bool DisableDefaultOptimizations=false;

	bool AutoScaleBTCValues=true;
	bool StartMiningWhenIdle=false;

	int MinIdleSeconds=60;
	bool LogToFile=true;

	long LogMaxFileSize=1048576; // in bytes

	bool ShowDriverVersionWarning=true;
//	bool DisableWindowsErrorReporting=true;
	bool ShowInternetConnectionWarning=true;
#if WITH_NVIDIA
	bool NVIDIAP0State=false;
#endif

	int ethminerDefaultBlockHeight=2000000;
	Enums::DagGenerationType EthminerDagGenerationType=Enums::DagGenerationType::SingleKeep;
	int ApiBindPortPoolStart=5100;
	double MinimumProfit=0;
	bool IdleWhenNoInternetAccess=true;
	bool UseIFTTT=false;
	bool DownloadInit=false;

	bool RunScriptOnCUDA_GPU_Lost=false;

	// 3rd party miners
	Enums::Use3rdPartyMiners Use3rdPartyMiners=Enums::Use3rdPartyMiners::NOT_SET;

	bool DownloadInit3rdParty=false;

	bool AllowMultipleInstances=true;

	QList<ComputeDeviceConfig*>* LastDevicesSettup=new QList<ComputeDeviceConfig*>; // device enabled disabled stuff

	QString hwid="";

	int agreedWithTOS=0;

	// normalization stuff
	double IQROverFactor=3.0; // @deprecated

	int NormalizedProfitHistory=15; // @deprecated
	double IQRNormalizeFactor=0.0; // @deprecated

	bool CoolDownCheckEnabled=true;

	// Set to skip driver checks to enable Neoscrypt/Lyra2RE on AMD
	bool ForceSkipAMDNeoscryptLyraCheck=false;

	// Overriding AMDOpenCLDeviceDetection returned Bus IDs (in case of driver error, e.g. 17.12.1)
	QString OverrideAMDBusIds="";

	Interval SwitchSmaTimeChangeSeconds=Interval(34, 55);
	Interval SwitchSmaTicksStable=Interval(2, 3);
	Interval SwitchSmaTicksUnstable=Interval(5, 13);

	void SetDefaults();
	void FixSettingBounds();

	static GeneralConfig* fromJson(QString);
	QString asJson(QJsonDocument::JsonFormat=QJsonDocument::JsonFormat::Indented);
};

#endif
