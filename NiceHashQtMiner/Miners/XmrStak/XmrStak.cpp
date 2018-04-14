#include "Miners/XmrStak/XmrStak.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Utils/Helpers.h"
#include "Globals.h"
#include "Algorithm.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include <QtConcurrent/QtConcurrentRun>


XmrStak::XmrStak(QString name, int maxCDTime)
	: Miner(name, maxCDTime)
{
	_ConectionType=Enums::NhmConectionType::NONE;
//	IsNeverHideMiningWindow=true;
}

QString XmrStak::GetConfigFileName()
{
	QStringList ids;
	foreach (MiningPair* pair, *_MiningSetup->MiningPairs) {
		ids.append(QString::number(pair->Device->ID));
		}
	return QString("config_%1.txt").arg(ids.join(','));
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
	QString username=GetUsername(btcAdress, worker);
	LastCommandLine= QStringList() << "-c" << GetConfigFileName();

	switch (_MiningSetup->MiningPairs->at(0)->Device->DeviceType) {
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

	prepareConfigFile(url, username);

	ProcessHandle=_Start();
}

QStringList XmrStak::BenchmarkCreateCommandLine(Algorithm* algorithm, int time)
{
	QString url=Globals::GetLocationURL(algorithm->NiceHashID, Globals::MiningLocation[ConfigManager.generalConfig->ServiceLocation], this->_ConectionType);
	prepareConfigFile(url, Globals::DemoUser);
	return {/*"benchmark_mode", */"-c", GetConfigFileName()};
}

bool XmrStak::BenchmarkParseLine(QString outdata)
{
	if (outdata.contains("Total:")) {
		QString toParse=outdata.mid(outdata.indexOf("Total:")).remove("Total:").trimmed();
		QStringList strings=toParse.split(' ', QString::SplitBehavior::SkipEmptyParts);
		foreach (QString s, strings) {
			bool ok=false;
			double lastSpeed=s.toDouble(&ok);
			if (ok) {
				Helpers::ConsolePrint("BENCHMARK "+MinerTag(), "double.TryParse true. Last speed is"+QString::number(lastSpeed));
				BenchmarkAlgorithm->BenchmarkSpeed=Helpers::ParseDouble(s);
				return true;
				}
			}
		}
	return false;
}

void XmrStak::BenchmarkOutputErrorDataReceivedImpl(QString outdata)
{
	CheckOutdata(outdata);
}
