#include "Miners/ClaymoreCryptoNightMiner.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Miners/Parsing/ExtraLaunchParametersParser.h"
#include "Globals.h"
#include "Algorithms/Algorithm.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"


ClaymoreCryptoNightMiner::ClaymoreCryptoNightMiner()
	: ClaymoreBaseMiner("ClaymoreCryptoNightMiner", 60*1000*5)
{
	LookForStart="xmr - total speed:";
}

void ClaymoreCryptoNightMiner::Start(QString url, QString btcAdress, QString worker)
{
	LastCommandLine=GetDevicesCommandString() << "-mport" << "127.0.0.1:-"+QString::number(ApiPort()) << "-xpool" << url << "-xwal" << GetUsername(btcAdress, worker) << "-xpsw" << "x" << "-dbg" << "-1";
	if (MiningSetup_->CurrentAlgorithmType==Enums::AlgorithmType::CryptoNightV7) {
		LastCommandLine << "-pow7" << "1";
		}
	ProcessHandle=_Start();
}

QStringList ClaymoreCryptoNightMiner::BenchmarkCreateCommandLine(Algorithm* algorithm, int time)
{
	BenchmarkTimeWait=time; // Takes longer as of v10

	// network workaround
	QString url=Globals::GetLocationURL(algorithm->NiceHashID, Globals::MiningLocation[ConfigManager.generalConfig->ServiceLocation], _ConectionType);
	// demo for benchmark
	QString username=Globals::DemoUser;
	if (ConfigManager.generalConfig->WorkerName.length()>0) {
		username+="."+ConfigManager.generalConfig->WorkerName.trimmed();
		}
	QStringList ret=GetDevicesCommandString() << "-mport" << QString::number(-ApiPort()) << "-xpool" << url << "-xwal" << username << "-xpsw" << "x" << "-logfile" << GetLogFileName()/*<< "-dbg" << "1"*/;
	if (algorithm->NiceHashID==Enums::AlgorithmType::CryptoNightV7) {
		ret << "-pow7" << "1";
		}
	return ret;
}
