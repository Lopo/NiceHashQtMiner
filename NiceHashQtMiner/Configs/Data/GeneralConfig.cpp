#include "GeneralConfig.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QApplication>
#include "Utils/Helpers.h"


void GeneralConfig::SetDefaults()
{
//	if (ConfigFileVersion!=nullptr) {delete ConfigFileVersion;}
	ConfigFileVersion=new QVersionNumber(QVersionNumber::fromString(qApp->applicationVersion()));
	Language=Enums::LanguageType::En;
	ForceCPUExtension=Enums::CPUExtensionType::Automatic;
	BitcoinAddress="";
	WorkerName="worker1";
	TimeUnit=Enums::TimeUnitType::Day;
	ServiceLocation=0;
	AutoStartMining=false;
//	DebugConsole=false;
//	HideMiningWindows=false;
	MinimizeToTray=false;
//	if (BenchmarkTimeLimits!=nullptr) {delete BenchmarkTimeLimits;}
	BenchmarkTimeLimits=new BenchmarkTimeLimitsConfig;
//	if (DeviceDetection!=nullptr) {delete DeviceDetection;}
	DeviceDetection=new DeviceDetectionConfig;
#if WITH_AMD
	DisableAMDTempControl=true;
#endif
	DisableDefaultOptimizations=false;
	AutoScaleBTCValues=true;
	StartMiningWhenIdle=false;
	LogToFile=true;
	LogMaxFileSize=1048576;
	ShowDriverVersionWarning=true;
//	DisableWindowsErrorReporting=true;
	ShowInternetConnectionWarning=true;
#if WITH_NVIDIA
	NVIDIAP0State=false;
#endif
	MinerRestartDelayMS=500;
	ethminerDefaultBlockHeight=2000000;
	SwitchMinSecondsFixed=90;
	SwitchMinSecondsDynamic=30;
#if WITH_AMD
	SwitchMinSecondsAMD=90;
#endif
	SwitchProfitabilityThreshold=0.05; // percent
	MinIdleSeconds=60;
	DisplayCurrency="USD";
	ApiBindPortPoolStart=4000;
	MinimumProfit=0;
	EthminerDagGenerationType=Enums::DagGenerationType::SingleKeep;
	DownloadInit=false;
	IdleWhenNoInternetAccess=true;
	Use3rdPartyMiners=Enums::Use3rdPartyMiners::NOT_SET;
	DownloadInit3rdParty=false;
	AllowMultipleInstances=true;
	UseIFTTT=false;
	IQROverFactor=3.0;
	CoolDownCheckEnabled=true;
	RunScriptOnCUDA_GPU_Lost=false;
	ForceSkipAMDNeoscryptLyraCheck=false;
	OverrideAMDBusIds="";
	SwitchSmaTimeChangeSeconds=Interval(34, 55);
	SwitchSmaTicksStable=Interval(2, 3);
	SwitchSmaTicksUnstable=Interval(5, 13);
	UseSmaCache=true;
}

void GeneralConfig::FixSettingBounds()
{
//	if (ConfigFileVersion!=nullptr) {delete ConfigFileVersion;}
	ConfigFileVersion=new QVersionNumber(QVersionNumber::fromString(qApp->applicationVersion()));
	if (DisplayCurrency.trimmed()=="") {
		DisplayCurrency="USD";
		}
	if (SwitchMinSecondsFixed<=0) {
		SwitchMinSecondsFixed=90;
		}
	if (SwitchMinSecondsDynamic<=0) {
		SwitchMinSecondsDynamic=30;
		}
#if WITH_AMD
	if (SwitchMinSecondsAMD<=0) {
		SwitchMinSecondsAMD=60;
		}
#endif
	if (MinerAPIQueryInterval<=0) {
		MinerAPIQueryInterval=5;
		}
	if (MinerRestartDelayMS<=0) {
		MinerRestartDelayMS=500;
		}
	if (MinIdleSeconds<=0) {
		MinIdleSeconds=60;
		}
	if (LogMaxFileSize<=0) {
		LogMaxFileSize=1048576;
		}
	if (ApiBindPortPoolStart>(65535-2000)) {
		ApiBindPortPoolStart=5100;
		}
	if (BenchmarkTimeLimits==nullptr) {
		BenchmarkTimeLimits=new BenchmarkTimeLimitsConfig;
		}
	if (DeviceDetection==nullptr) {
		DeviceDetection=new DeviceDetectionConfig;
		}
	if (LastDevicesSettup==nullptr) {
		LastDevicesSettup=new QList<ComputeDeviceConfig*>;
		}
	if (IQROverFactor<0) {
		IQROverFactor=3.0;
		}
	if (NormalizedProfitHistory<0) {
		NormalizedProfitHistory=15;
		}
	if (IQRNormalizeFactor<0) {
		IQRNormalizeFactor=0.0;
		}
	if (KwhPrice<0) {
		KwhPrice=0;
		}

	SwitchSmaTimeChangeSeconds.FixRange();
	SwitchSmaTicksStable.FixRange();
	SwitchSmaTicksUnstable.FixRange();
}

GeneralConfig* GeneralConfig::fromJson(QString json)
{
	GeneralConfig* gc=new GeneralConfig;
	QJsonObject o=QJsonDocument::fromJson(json.toLatin1()).object();

	if (o.value("ConfigFileVersion")!=QJsonValue::Undefined) {
		gc->ConfigFileVersion=new QVersionNumber(QVersionNumber::fromString(o.value("ConfigFileVersion").toString()));
		}
	if (o.value("Language")!=QJsonValue::Undefined) {
		gc->Language=(Enums::LanguageType)o.value("Language").toInt();
		}
	if (o.value("DisplayCurrency")!=QJsonValue::Undefined) {
		gc->DisplayCurrency=o.value("DisplayCurrency").toString();
		}

//	if (o.value("DebugConsole")!=QJsonValue::Undefined) {
//		gc->DebugConsole=o.value("DebugConsole").toBool();
//		}
	if (o.value("BitcoinAddress")!=QJsonValue::Undefined) {
		gc->BitcoinAddress=o.value("BitcoinAddress").toString();
		}
	if (o.value("WorkerName")!=QJsonValue::Undefined) {
		gc->WorkerName=o.value("WorkerName").toString();
		}
	if (o.value("TimeUnit")!=QJsonValue::Undefined) {
		gc->TimeUnit=(Enums::TimeUnitType)o.value("TimeUnit").toInt();
		}
	if (o.value("IFTTTKey")!=QJsonValue::Undefined) {
		gc->IFTTTKey=o.value("IFTTTKey").toString();
		}
	if (o.value("ServiceLocation")!=QJsonValue::Undefined) {
		gc->ServiceLocation=o.value("ServiceLocation").toInt();
		}
	if (o.value("AutoStartMining")!=QJsonValue::Undefined) {
		gc->AutoStartMining=o.value("AutoStartMining").toBool();
		}
//	if (o.value("HideMiningWindows")!=QJsonValue::Undefined) {
//		gc->HideMiningWindows=o.value("HideMiningWindows").toBool();
//		}
	if (o.value("MinimizeToTray")!=QJsonValue::Undefined) {
		gc->MinimizeToTray=o.value("MinimizeToTray").toBool();
		}

//	if (o.value("MinimizeMiningWindows")!=QJsonValue::Undefined) {
//		gc->MinimizeMiningWindows=o.value("MinimizeMiningWindows").toBool();
//		}

	if (o.value("ForceCPUExtension")!=QJsonValue::Undefined) {
		gc->ForceCPUExtension=(Enums::CPUExtensionType)o.value("ForceCPUExtension").toInt();
		}

	if (o.value("SwitchMinSecondsFixed")!=QJsonValue::Undefined) {
		gc->SwitchMinSecondsFixed=o.value("SwitchMinSecondsFixed").toInt();
		}
	if (o.value("SwitchMinSecondsDynamic")!=QJsonValue::Undefined) {
		gc->SwitchMinSecondsDynamic=o.value("SwitchMinSecondsDynamic").toInt();
		}
#if WITH_AMD
	if (o.value("SwitchMinSecondsAMD")!=QJsonValue::Undefined) {
		gc->SwitchMinSecondsAMD=o.value("SwitchMinSecondsAMD").toInt();
		}
#endif
	if (o.value("SwitchProfitabilityThreshold")!=QJsonValue::Undefined) {
		gc->SwitchProfitabilityThreshold=o.value("SwitchProfitabilityThreshold").toDouble();
		}
	if (o.value("MinerAPIQueryInterval")!=QJsonValue::Undefined) {
		gc->MinerAPIQueryInterval=o.value("MinerAPIQueryInterval").toInt();
		}
	if (o.value("MinerRestartDelayMS")!=QJsonValue::Undefined) {
		gc->MinerRestartDelayMS=o.value("MinerRestartDelayMS").toInt();
		}

	if (o.value("BenchmarkTimeLimits")!=QJsonValue::Undefined) {
		if (gc->BenchmarkTimeLimits==nullptr) {gc->BenchmarkTimeLimits=new BenchmarkTimeLimitsConfig;}
		QJsonObject btl=o.value("BenchmarkTimeLimits").toObject();
		QVector<int> vals;
		foreach (QJsonValue i, btl.value("CPU").toArray()) {
			vals.append(i.toInt());
			}
		gc->BenchmarkTimeLimits->setCPU(vals);
#if WITH_NVIDIA
		vals.clear();
		foreach (QJsonValue i, btl.value("NVIDIA").toArray()) {
			vals.append(i.toInt());
			}
		gc->BenchmarkTimeLimits->setNVIDIA(vals);
#endif
#if WITH_AMD
		vals.clear();
		foreach (QJsonValue i, btl.value("AMD").toArray()) {
			vals.append(i.toInt());
			}
		gc->BenchmarkTimeLimits->setAMD(vals);
#endif
		}

	if (o.value("DeviceDetection")!=QJsonValue::Undefined) {
		if (gc->DeviceDetection==nullptr) {gc->DeviceDetection=new DeviceDetectionConfig;}
		QJsonObject ddco=o.value("DeviceDetection").toObject();
#if WITH_AMD
		if (ddco.value("DisableDetectionAMD")!=QJsonValue::Undefined) {
			gc->DeviceDetection->DisableDetectionAMD=ddco.value("DisableDetectionAMD").toBool();
			}
#endif
#if WITH_NVIDIA
		if (ddco.value("DisableDetectionNVIDIA")!=QJsonValue::Undefined) {
			gc->DeviceDetection->DisableDetectionNVIDIA=ddco.value("DisableDetectionNVIDIA").toBool();
			}
#endif
		}

#if WITH_AMD
	if (o.value("DisableAMDTempControl")!=QJsonValue::Undefined) {
		gc->DisableAMDTempControl=o.value("DisableAMDTempControl").toBool();
		}
#endif
	if (o.value("DisableDefaultOptimizations")!=QJsonValue::Undefined) {
		gc->DisableDefaultOptimizations=o.value("DisableDefaultOptimizations").toBool();
		}

	if (o.value("AutoScaleBTCValues")!=QJsonValue::Undefined) {
		gc->AutoScaleBTCValues=o.value("AutoScaleBTCValues").toBool();
		}
	if (o.value("StartMiningWhenIdle")!=QJsonValue::Undefined) {
		gc->StartMiningWhenIdle=o.value("StartMiningWhenIdle").toBool();
		}

	if (o.value("MinIdleSeconds")!=QJsonValue::Undefined) {
		gc->MinIdleSeconds=o.value("MinIdleSeconds").toInt();
		}
	if (o.value("LogToFile")!=QJsonValue::Undefined) {
		gc->LogToFile=o.value("LogToFile").toBool();
		}

	if (o.value("LogMaxFileSize")!=QJsonValue::Undefined) {
		gc->LogMaxFileSize=Helpers::ParseLong(o.value("LogMaxFileSize").toString());
		}

	if (o.value("ShowDriverVersionWarning")!=QJsonValue::Undefined) {
		gc->ShowDriverVersionWarning=o.value("ShowDriverVersionWarning").toBool();
		}
//	if (o.value("DisableWindowsErrorReporting")!=QJsonValue::Undefined) {
//		gc->DisableWindowsErrorReporting=o.value("DisableWindowsErrorReporting").toBool();
//		}
	if (o.value("ShowInternetConnectionWarning")!=QJsonValue::Undefined) {
		gc->ShowInternetConnectionWarning=o.value("ShowInternetConnectionWarning").toBool();
		}
#if WITH_NVIDIA
	if (o.value("NVIDIAP0State")!=QJsonValue::Undefined) {
		gc->NVIDIAP0State=o.value("NVIDIAP0State").toBool();
		}
#endif

	if (o.value("ethminerDefaultBlockHeight")!=QJsonValue::Undefined) {
		gc->ethminerDefaultBlockHeight=o.value("ethminerDefaultBlockHeight").toInt();
		}
	if (o.value("EthminerDagGenerationType")!=QJsonValue::Undefined) { //enum
		gc->EthminerDagGenerationType=(Enums::DagGenerationType)o.value("EthminerDagGenerationType").toInt();
		}
	if (o.value("ApiBindPortPoolStart")!=QJsonValue::Undefined) {
		gc->ApiBindPortPoolStart=o.value("ApiBindPortPoolStart").toInt();
		}
	if (o.value("MinimumProfit")!=QJsonValue::Undefined) {
		gc->MinimumProfit=o.value("MinimumProfit").toDouble();
		}
	if (o.value("IdleWhenNoInternetAccess")!=QJsonValue::Undefined) {
		gc->IdleWhenNoInternetAccess=o.value("IdleWhenNoInternetAccess").toBool();
		}
	if (o.value("UseIFTTT")!=QJsonValue::Undefined) {
		gc->UseIFTTT=o.value("UseIFTTT").toBool();
		}
	if (o.value("DownloadInit")!=QJsonValue::Undefined) {
		gc->DownloadInit=o.value("DownloadInit").toBool();
		}

	if (o.value("RunScriptOnCUDA_GPU_Lost")!=QJsonValue::Undefined) {
		gc->RunScriptOnCUDA_GPU_Lost=o.value("RunScriptOnCUDA_GPU_Lost").toBool();
		}

	if (o.value("Use3rdPartyMiners")!=QJsonValue::Undefined) {
		gc->Use3rdPartyMiners=(Enums::Use3rdPartyMiners)o.value("Use3rdPartyMiners").toInt();
		}

	if (o.value("DownloadInit3rdParty")!=QJsonValue::Undefined) {
		gc->DownloadInit3rdParty=o.value("DownloadInit3rdParty").toBool();
		}

	if (o.value("AllowMultipleInstances")!=QJsonValue::Undefined) {
		gc->AllowMultipleInstances=o.value("AllowMultipleInstances").toBool();
		}

	if (o.value("LastDevicesSettup")!=QJsonValue::Undefined) {
//		if (gc->LastDevicesSettup==nullptr) {gc->LastDevicesSettup=new QList<ComputeDeviceConfig*>;}
		foreach (QJsonValue dv, o.value("LastDevicesSettup").toArray()) {
			ComputeDeviceConfig* cdc=new ComputeDeviceConfig;
			QJsonObject dvo=dv.toObject();
			cdc->Name=dvo.value("Name").toString();
			cdc->Enabled=dvo.value("Enabled").toBool();
			cdc->UUID=dvo.value("UUID").toString();
			gc->LastDevicesSettup->append(cdc);
			}
		}

	if (o.value("hwid")!=QJsonValue::Undefined) {
		gc->hwid=o.value("hwid").toString();
		}

	if (o.value("agreedWithTOS")!=QJsonValue::Undefined) {
		gc->agreedWithTOS=o.value("agreedWithTOS").toInt();
		}

	if (o.value("IQROverFactor")!=QJsonValue::Undefined) {
		gc->IQROverFactor=o.value("IQROverFactor").toDouble();
		}

	if (o.value("NormalizeProfitHistory")!=QJsonValue::Undefined) {
		gc->NormalizedProfitHistory=o.value("NormalizeProfitHistory").toInt();
		}
	if (o.value("IQRNormalizeFactor")!=QJsonValue::Undefined) {
		gc->IQRNormalizeFactor=o.value("IQRNormalizeFactor").toDouble();
		}

	if (o.value("CoolDownCheckEnabled")!=QJsonValue::Undefined) {
		gc->CoolDownCheckEnabled=o.value("CoolDownCheckEnabled").toBool();
		}

	if (o.value("ForceSkipAMDNeoscryptLyraCheck")!=QJsonValue::Undefined) {
		gc->ForceSkipAMDNeoscryptLyraCheck=o.value("ForceSkipAMDNeoscryptLyraCheck").toBool();
		}

	if (o.value("OverrideAMDBusIds")!=QJsonValue::Undefined) {
		gc->OverrideAMDBusIds=o.value("OverrideAMDBusIds").toString();
		}

	if (o.value("KwhPrice")!=QJsonValue::Undefined) {
		gc->KwhPrice=o.value("KwhPrice").toDouble();
		}

	if (o.value("UseSmaCache")!=QJsonValue::Undefined) {
		gc->UseSmaCache=o.value("UseSmaCache").toBool();
		}

	return gc;
}

QString GeneralConfig::asJson(QJsonDocument::JsonFormat format)
{
	QJsonObject o;
	o.insert("ConfigFileVersion", ConfigFileVersion!=nullptr? ConfigFileVersion->toString() : QVersionNumber::fromString(qApp->applicationVersion()).toString());
	o.insert("Language", (int)Language);
	o.insert("DisplayCurrency", DisplayCurrency);

//	o.insert("DebugConsole", DebugConsole);
	o.insert("BitcoinAddress", BitcoinAddress);
	o.insert("WorkerName", WorkerName);
	o.insert("IFTTTKey", IFTTTKey);
	o.insert("ServiceLocation", ServiceLocation);
	o.insert("AutoStartMining", AutoStartMining);
//	o.insert("HideMiningWindows", HideMiningWindows);
	o.insert("MinimizeToTray", MinimizeToTray);

//	o.insert("MinimizeMiningWindows", MinimizeMiningWindows);

	o.insert("ForceCPUExtension", (int)ForceCPUExtension);

	o.insert("SwitchMinSecondsFixed", SwitchMinSecondsFixed);
	o.insert("SwitchMinSecondsDynamic", SwitchMinSecondsDynamic);
#if WITH_AMD
	o.insert("SwitchMinSecondsAMD", SwitchMinSecondsAMD);
#endif
	o.insert("SwitchProfitabilityThreshold", SwitchProfitabilityThreshold);
	o.insert("MinerAPIQueryInterval", MinerAPIQueryInterval);
	o.insert("MinerRestartDelayMS", MinerRestartDelayMS);

	QJsonObject btl;
	QJsonArray cpu;
	foreach (int v, BenchmarkTimeLimits->CPU()) {
		cpu.append(v);
		}
	btl.insert("CPU", cpu);
#if WITH_NVIDIA
	QJsonArray nvidia;
	foreach (int v, BenchmarkTimeLimits->NVIDIA()) {
		nvidia.append(v);
		}
	btl.insert("NVIDIA", nvidia);
#endif
#if WITH_AMD
	QJsonArray amd;
	foreach (int v, BenchmarkTimeLimits->AMD()) {
		amd.append(v);
		}
	btl.insert("AMD", amd);
#endif
	o.insert("BenchmarkTimeLimits", btl);

	QJsonObject ddo;
#if WITH_AMD
	ddo.insert("DisableDetectionAMD", DeviceDetection->DisableDetectionAMD);
#endif
#if WITH_NVIDIA
	ddo.insert("DisableDetectionNVIDIA", DeviceDetection->DisableDetectionNVIDIA);
#endif
	o.insert("DeviceDetection", ddo);

#if WITH_AMD
	o.insert("DisableAMDTempControl", DisableAMDTempControl);
#endif
	o.insert("DisableDefaultOptimizations", DisableDefaultOptimizations);

	o.insert("AutoScaleBTCValues", AutoScaleBTCValues);
	o.insert("StartMiningWhenIdle", StartMiningWhenIdle);

	o.insert("MinIdleSeconds", MinIdleSeconds);
	o.insert("LogToFile", LogToFile);

	o.insert("LogMaxFileSize", (long long)LogMaxFileSize); // error: conversion from ‘long int’ to ‘const QJsonValue’ is ambiguous

	o.insert("ShowDriverVersionWarning", ShowDriverVersionWarning);
//	o.insert("DisableWindowsErrorReporting", DisableWindowsErrorReporting);
	o.insert("ShowInternetConnectionWarning", ShowInternetConnectionWarning);
#if WITH_NVIDIA
	o.insert("NVIDIAP0State", NVIDIAP0State);
#endif

	o.insert("ethminerDefaultBlockHeight", ethminerDefaultBlockHeight);
	o.insert("EthminerDagGenerationType", (int)EthminerDagGenerationType);
	o.insert("ApiBindPortPoolStart", ApiBindPortPoolStart);
	o.insert("MinimumProfit", MinimumProfit);
	o.insert("IdleWhenNoInternetAccess", IdleWhenNoInternetAccess);
	o.insert("UseIFTTT", UseIFTTT);
	o.insert("DownloadInit", DownloadInit);

	o.insert("RunScriptOnCUDA_GPU_Lost", RunScriptOnCUDA_GPU_Lost);

	o.insert("Use3rdPartyMiners", (int)Use3rdPartyMiners);

	o.insert("DownloadInit3rdParty", DownloadInit3rdParty);

	o.insert("AllowMultipleInstances", AllowMultipleInstances);

	QJsonArray ldsa;
	foreach (ComputeDeviceConfig* lds, *LastDevicesSettup) {
		QJsonObject cdc;
		cdc.insert("Name", lds->Name);
		cdc.insert("Enabled", lds->Enabled);
		cdc.insert("UUID", lds->UUID);
		ldsa.append(cdc);
		}
	o.insert("LastDevicesSettup", ldsa);

	o.insert("hwid", hwid);

	o.insert("agreedWithTOS", agreedWithTOS);

	o.insert("IQROverFactor", IQROverFactor);

	o.insert("NormalizeProfitHistory", NormalizedProfitHistory);
	o.insert("IQRNormalizeFactor", IQRNormalizeFactor);

	o.insert("CoolDownCheckEnabled", CoolDownCheckEnabled);

	o.insert("ForceSkipAMDNeoscryptLyraCheck", ForceSkipAMDNeoscryptLyraCheck);

	o.insert("OverrideAMDBusIds", OverrideAMDBusIds);

	o.insert("KwhPrice", KwhPrice);

	o.insert("UseSmaCache", UseSmaCache);

	return QString(QJsonDocument(o).toJson(format));
}
