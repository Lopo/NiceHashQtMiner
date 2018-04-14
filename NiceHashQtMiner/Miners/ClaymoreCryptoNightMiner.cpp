#include "Miners/ClaymoreCryptoNightMiner.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Miners/Parsing/ExtraLaunchParametersParser.h"
#include "Globals.h"
#include "Algorithm.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"


const QString ClaymoreCryptoNightMiner::_LookForStart="XMR - Total Speed:";
const QString ClaymoreCryptoNightMiner::LookForStartOld="hashrate =";


ClaymoreCryptoNightMiner::ClaymoreCryptoNightMiner(bool isOld/*=false*/)
	: ClaymoreBaseMiner("ClaymoreCryptoNightMiner", isOld? LookForStartOld : _LookForStart, 60*1000*5)
{
	_isOld=isOld;
}

double ClaymoreCryptoNightMiner::DevFee()
{
	return _isOld
		? 2.0
		: 0.0;
}

QStringList ClaymoreCryptoNightMiner::GetDevicesCommandString()
{
	if (!_isOld) {
		return ClaymoreBaseMiner::GetDevicesCommandString();
		}

	QStringList ids;
	foreach (MiningPair* mPair, *_MiningSetup->MiningPairs) {
		ids.append(QString::number(mPair->Device->ID));
		}

	return QStringList() << "-di" << ids.join("") << ExtraLaunchParametersParser::ParseForMiningSetup(_MiningSetup, Enums::DeviceType::AMD);
}

void ClaymoreCryptoNightMiner::Start(QString url, QString btcAdress, QString worker)
{
	QString username=GetUsername(btcAdress, worker);
	if (_isOld) {
		LastCommandLine=GetDevicesCommandString() << "-mport" << QString::number(-ApiPort()) << "-o" << url << "-u" << username << "-p" << "x" << "-dbg" << "-1";
		}
	else {
		LastCommandLine=GetDevicesCommandString() << "-mport" << QString::number(-ApiPort()) << "-xpool" << url << "-xwal" << username << "-xpsw" << "x" << "-dbg" << "-1";
		if (_MiningSetup->CurrentAlgorithmType==Enums::AlgorithmType::CryptoNightV7) {
			LastCommandLine << "-pow7" << "1";
			}
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
	if (_isOld) {
		return GetDevicesCommandString() << "-mport" << QString::number(-ApiPort()) << "-o" << url << "-u" << username << "-p" << "x" << "-dbg" << "1";
		}
	QStringList ret=GetDevicesCommandString() << "-mport" << QString::number(-ApiPort()) << "-xpool" << url << "-xwal" << username << "-xpsw" << "x" << "-dbg" << "1";
	if (algorithm->NiceHashID==Enums::AlgorithmType::CryptoNightV7) {
		ret << "-pow7" << "1";
		}
	return ret;
}
