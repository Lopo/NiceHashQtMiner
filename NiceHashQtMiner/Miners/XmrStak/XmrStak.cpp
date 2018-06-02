#include "Miners/XmrStak/XmrStak.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Utils/Helpers.h"
#include "Globals.h"
#include "Algorithms/Algorithm.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Configs/XmrStakConfig.h"
#include "Configs/XmrStakConfigPool.h"
#include "Miners/Parsing/ExtraLaunchParametersParser.h"
#include "Configs/XmrStakConfigCpu.h"
#include "Configs/XmrStakConfigGpu.h"
#include "Configs/IasJsonObject.h"
#include "Qt/LException.h"
#include <QtConcurrent/QtConcurrentRun>
#include <QMetaEnum>
#include <qt/QtCore/qfile.h>


XmrStak::XmrStak(QString name/*=""*/)
	: Miner("XmrStak", 5*60*1000)
{
	_ConectionType=Enums::NhmConectionType::NONE;
//	IsNeverHideMiningWindow=true;
	TimeoutStandard=true;
	IsMultiType=true;
}

void XmrStak::_Stop(Enums::MinerStopType willswitch)
{
	Stop_cpu_ccminer_sgminer_nheqminer(willswitch);
}

ApiData* XmrStak::GetSummaryAsync()
{
	return QtConcurrent::run(this, static_cast<ApiData*(XmrStak::*)(QString, bool)>(&Miner::GetSummaryCpuAsync), QString("api.json"), true).result();
}

bool XmrStak::IsApiEof(uchar third, uchar second, uchar last)
{
	return second==0x7d && last==0x7d;
}

void XmrStak::Start(QString url, QString btcAdress, QString worker)
{
	if (!IsInit()) {
		Helpers::ConsolePrint(MinerTag(), "MiningSetup is not initialized exiting Start()");
		return;
		}
/*	QString username=GetUsername(btcAdress, worker);
	LastCommandLine= QStringList() << "-c" << GetConfigFileName();

	switch (MiningSetup_->MiningPairs->at(0)->Device->DeviceType) {
		case Enums::DeviceType::CPU:
			LastCommandLine << "--noAMD" << "--noNVIDIA" << "--cpu" << GetConfigFileName();
			break;
		case Enums::DeviceType::AMD:
			LastCommandLine << "--noCPU" << "--noNVIDIA" << "--amd" << GetConfigFileName();
			break;
		case Enums::DeviceType::NVIDIA:
			LastCommandLine << "--noCPU" << "--noAMD" << "--nvidia" << GetConfigFileName();
			break;
		}

	prepareConfigFile(url, username);*/
	auto devConfigs=PrepareConfigFiles(url, btcAdress, worker);
	LastCommandLine=CreateLaunchCommand(ConfigName, devConfigs);

	ProcessHandle=_Start();
}

QStringList XmrStak::CreateLaunchCommand(QString configName, QMap<Enums::DeviceType, QString> devConfigs)
{
	QStringList devs;
	foreach (Enums::DeviceType dev, devConfigs.keys()) {
		devs << "--"+QString(QMetaEnum::fromType<Enums::DeviceType>().valueToKey((int)dev)).toLower() << devConfigs.value(dev);
		}
	return QStringList() << "-c" << configName << "-C" << GetPoolConfigName() << devs << DisableDevCmd(devConfigs.keys());
}

QMap<Enums::DeviceType, QString> XmrStak::PrepareConfigFiles(QString url, QString btcAddress, QString worker, bool bench)
{
	QMap<Enums::DeviceType, QString> configs;
	QList<Enums::DeviceType> types;
	bool isHeavy=false;
	foreach (MiningPair* pair, *MiningSetup_->MiningPairs) {
		if (!types.contains(pair->Device->DeviceType)) {
			types.append(pair->Device->DeviceType);
			}
		if (pair->algorithm->NiceHashID==Enums::AlgorithmType::CryptoNightHeavy) {
			isHeavy=true;
			}
		}

	QString configName= bench? GetBenchConfigName() : ConfigName;
	XmrStakConfig* config=ParseJsonFile<XmrStakConfig>(Enums::DeviceType::CPU, DefConfigName);
	if (config==nullptr) {
		config=new XmrStakConfig;
		}
	config->httpd_port=ApiPort();
	if (bench) {
		config->SetBenchmarkOptions(GetLogFileName());
		}

	WriteJsonFile(config, configName, DefConfigName);

	XmrStakConfigPool* pools=new XmrStakConfigPool;
	pools->SetupPools(url, GetUsername(btcAddress, worker), isHeavy);
	WriteJsonFile(pools, GetPoolConfigName());
	WriteJsonFile(pools, DefPoolName);

	foreach (Enums::DeviceType type, types) {
		if (type==Enums::DeviceType::CPU) {
			MiningPair* cpuPair=*std::find_if(miningSetup()->MiningPairs->begin(), miningSetup()->MiningPairs->end(), [type](MiningPair* p){return p->Device->DeviceType==type;});
			bool isHyperThreadingEnabled=cpuPair->CurrentExtraLaunchParameters.join(' ').contains("enable_ht=true");
			int numTr=ExtraLaunchParametersParser::GetThreadsNumber(cpuPair);
			bool no_prefetch=ExtraLaunchParametersParser::GetNoPrefetch(cpuPair);
			if (isHyperThreadingEnabled) {
				numTr>>=1; // /2
				}

			// Fallback on classic config if haven't been able to open
			XmrStakConfigCpu* configCpu=ParseJsonFile<XmrStakConfigCpu>(type);
			if (configCpu==nullptr) {
				configCpu=new XmrStakConfigCpu(numTr);
				}
			if (!configCpu->cpu_threads_conf.count()) {
				// No thread count would prevent CPU from mining, so fill with estimates
				// Otherwise use values set by xmr-stak/user
				configCpu->Inti_cpu_threads_conf(false, no_prefetch, false, isHyperThreadingEnabled);
				}

			configs.insert(type, WriteJsonFile(configCpu, type));
			}
		else {
			QList<int> ids;
			foreach (MiningPair* p, *miningSetup()->MiningPairs) {
				if (p->Device->DeviceType==type) {
					ids.append(p->Device->ID);
					}
				}

			if (type==Enums::DeviceType::AMD) {
				XmrStakConfigAmd* configGpu=ParseJsonFile<XmrStakConfigAmd>(type);
				if (configGpu==nullptr) {
					configGpu=new XmrStakConfigAmd;
					}
				configGpu->SetupThreads(ids);
				configs.insert(type, WriteJsonFile(configGpu, type));
				}
			else {
				bool keepBVals=std::find_if(miningSetup()->MiningPairs->begin(), miningSetup()->MiningPairs->end(), [type](MiningPair* p){return p->CurrentExtraLaunchParameters.join(' ').contains("--keep-b") && p->Device->DeviceType==type;})!=miningSetup()->MiningPairs->end();
				XmrStakConfigNvidia* configGpu=ParseJsonFile<XmrStakConfigNvidia>(type);
				if (configGpu==nullptr) {
					configGpu=new XmrStakConfigNvidia;
					}
				// B values do not seem to work on many nv setups, workaround by forcing higher vals unless user opts out
				configGpu->SetupThreads(ids);
				if (!keepBVals) {
					configGpu->OverrideBVals();
					}
				configs.insert(type, WriteJsonFile(configGpu, type));
				}
			}
		}

	return configs;
}

QString XmrStak::GetDevConfigFileName(Enums::DeviceType type)
{
	QStringList ids;
	foreach (MiningPair* pair, *miningSetup()->MiningPairs) {
		if (pair->Device->DeviceType==type) {
			ids << QString::number(pair->Device->ID);
			}
		}
	return QString("%1_%2.txt").arg(QMetaEnum::fromType<Enums::DeviceType>().valueToKey((int)type)).arg(ids.join(','));
}

QString XmrStak::GetBenchConfigName()
{
	return QString("bench_%1.txt").arg(GetDeviceID());
}

QString XmrStak::GetPoolConfigName()
{
	return QString("pools_%1.txt").arg(GetDeviceID());
}

QStringList XmrStak::DisableDevCmd(QList<Enums::DeviceType> usedDevs)
{
	QList<Enums::DeviceType> devTypes={
		Enums::DeviceType::AMD,
		Enums::DeviceType::CPU,
		Enums::DeviceType::NVIDIA
		};
	QStringList ret;
	foreach (Enums::DeviceType d, devTypes) {
		if (!usedDevs.contains(d)) {
			ret << QString("--no%1").arg(QMetaEnum::fromType<Enums::DeviceType>().valueToKey((int)d));
			}
		}
	return ret;
}

QStringList XmrStak::DisableDevCmd(Enums::DeviceType usedDev)
{
	return DisableDevCmd({usedDev});
}

QStringList XmrStak::BenchmarkCreateCommandLine(Algorithm* algorithm, int time)
{
	QString url=Globals::GetLocationURL(algorithm->NiceHashID, Globals::MiningLocation[ConfigManager.generalConfig->ServiceLocation], _ConectionType);
	QMap<Enums::DeviceType, QString> configs=PrepareConfigFiles(url, Globals::GetBitcoinUser(), ConfigManager.generalConfig->WorkerName.trimmed(), true);
	_benchmarkCount=0;
	_benchmarkSum=0;
	BenchmarkTimeInSeconds=std::max(time, 60);
	CleanOldLogs();
	return CreateLaunchCommand(GetBenchConfigName(), configs);
}

void XmrStak::FinishUpBenchmark()
{
	BenchmarkAlgorithm->BenchmarkSpeed(_benchmarkSum/std::max(1, _benchmarkCount));
}

bool XmrStak::BenchmarkParseLine(QString outdata)
{
	if (!outdata.contains("Totals (ALL):")) {
		return false;
		}
	QStringList speeds=outdata.split('\n');
	foreach (QString s, speeds) {
		bool ok=false;
		double speed=s.toDouble(&ok);
		if (!ok) {
			continue;
			}
		_benchmarkSum+=speed;
		_benchmarkCount++;
		break;
		}
	return false;
}

void XmrStak::BenchmarkOutputErrorDataReceivedImpl(QString outdata)
{
	CheckOutdata(outdata);
}

template<class T>
T* XmrStak::ParseJsonFile(Enums::DeviceType type, QString filename, bool fallback)
{
	if (filename=="") {
		filename=QString("%1.txt").arg(QMetaEnum::fromType<Enums::DeviceType>().valueToKey((int)type));
		}
	T* json;
	try {
		if (!QFile::exists(WorkingDirectory()+filename)) {
			throw LException("FileNotFound");
			}
		QFile fr(WorkingDirectory()+filename);
		fr.open(QIODevice::ReadOnly|QIODevice::Text);
		QString file(fr.readAll());
		fr.close();
		file.prepend('{');
		file+='}';
		json=T::fromJson(file);
		}
	catch (LException& e) {
		Helpers::ConsolePrint(MinerTag(), QString("Config file %1 not found, attempting to generate").arg(filename));
		}
	catch (QException& e) {
		Helpers::ConsolePrint(MinerTag(), e.what());
		}

	if (json==nullptr) {
		// If from recursive call, don't try again
		if (fallback) {
			return nullptr;
			}
		if (!QFile::exists(WorkingDirectory()+DefConfigName)) {
			// Exception since xmr-stak won't passively generate general config
			WriteJsonFile(new XmrStakConfig, filename);
			}

		if (typeid(T)!=typeid(XmrStakConfig)) { // @todo verify
			// Try running xmr-stak to create default configs
			QProcess* handle=BenchmarkStartProcess(DisableDevCmd(type) << "--generate-configs");
//			QTimer* timer=new QTimer(this);
//			timer->start();
			handle->start();
			try {
				if (!handle->waitForFinished(20*1000)) {
					handle->kill(); // Should have exited already if using right xmr-stak ver
					handle->waitForFinished(20*1000);
					handle->close();
					}
				}
			catch (...) {}

			json=ParseJsonFile<T>(type, filename, true);
			}
		}

	return json;
}

QString XmrStak::WriteJsonFile(IasJsonObject* config, QString filename, QString genFrom, bool isNv)
{
	QString header="// This config file was autogenerated by NHML.";
	if (genFrom!="") {
		header+=QString("\n// The values were filled from %1. If you wish to edit them, you should edit that file instead.").arg(genFrom);
		}

	if (isNv) {
		header+="\n// bsleep and bfactor are overriden by default for compatibility. You can disable this functionality by adding \"--keep-b\" to extra launch parameters.";
		}

	try {
		QJsonObject confJson=config->asJsonObject();
		QString writeStr=QJsonDocument(confJson).toJson();
		int start=writeStr.indexOf('{');
		int end=writeStr.lastIndexOf('}');
		writeStr=writeStr.mid(start+1, end-1);
		writeStr=header+"\n"+writeStr;
		QFile fw(WorkingDirectory()+filename);
		fw.open(QIODevice::WriteOnly|QIODevice::Text);
		fw.write(writeStr.toLatin1());
		fw.close();
		}
	catch (QException& e) {
		Helpers::ConsolePrint(MinerTag(), e.what());
		Helpers::ConsolePrint(MinerTag(), QString("Config %1 was unable to write for xmr-stak, mining may not work").arg(filename));
		}

	return filename;
}

QString XmrStak::WriteJsonFile(IasJsonObject* config, Enums::DeviceType type)
{
	return WriteJsonFile(config, GetDevConfigFileName(type), QString("%1.txt").arg(QMetaEnum::fromType<Enums::DeviceType>().valueToKey((int)type)), type==Enums::DeviceType::NVIDIA);
}
