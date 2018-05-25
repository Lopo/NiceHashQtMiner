#include "Miners/ClaymoreDual.h"
#include "Globals.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Algorithms/Algorithm.h"
#include "Algorithms/DualAlgorithm.h"


ClaymoreDual::ClaymoreDual(Enums::AlgorithmType secondaryAlgorithmType)
	: ClaymoreBaseMiner("ClaymoreDual", 90*1000)
{
	IgnoreZero=true;
	ApiReadMult=1000;
	_ConectionType=Enums::NhmConectionType::STRATUM_TCP;
	SecondaryAlgorithmType=secondaryAlgorithmType;

	LookForStart="eth - total speed:";
	SecondaryLookForStart=SecondaryShortName()+" - total speed:";
	DevFee=IsDual()? 1.5 : 1.0;

	IsMultiType=true;
}

// the short form the miner uses for secondary algo in cmd line and log
QString ClaymoreDual::SecondaryShortName()
{
	switch (SecondaryAlgorithmType) {
		case Enums::AlgorithmType::Decred:
			return "dcr";
		case Enums::AlgorithmType::Lbry:
			return "lbc";
		case Enums::AlgorithmType::Pascal:
			return "pasc";
		case Enums::AlgorithmType::Sia:
			return "sc";
		case Enums::AlgorithmType::Blake2s:
			return "b2s";
		case Enums::AlgorithmType::Keccak:
			return "kc";
		default: // prevent warning: enumeration value not handled in switch
			return "";
		}
	return "";
}

QStringList ClaymoreDual::GetStartCommand(QString url, QString btcAdress, QString worker)
{
	QString username=GetUsername(btcAdress, worker);

	QStringList dualModeParams;
	if (!IsDual()) { // leave convenience param for non-dual entry
		foreach (MiningPair* pair, *MiningSetup_->MiningPairs) {
			if (!pair->CurrentExtraLaunchParameters.join("").contains("-dual=")) {
				continue;
				}
			Enums::AlgorithmType dual=Enums::AlgorithmType::NONE;
			QStringList coinP;
			if (pair->CurrentExtraLaunchParameters.join("").contains("Decred")) {
				dual=Enums::AlgorithmType::Decred;
				coinP=QStringList({"-dcoin", "dcr"});
				}
			if (pair->CurrentExtraLaunchParameters.join("").contains("Siacoin")) {
				dual=Enums::AlgorithmType::Sia;
				coinP=QStringList({"-dcoin", "sc"});
				}
			if (pair->CurrentExtraLaunchParameters.join("").contains("Lbry")) {
				dual=Enums::AlgorithmType::Lbry;
				coinP=QStringList({"-dcoin", "lbc"});
				}
			if (pair->CurrentExtraLaunchParameters.join("").contains("Pascal")) {
				dual=Enums::AlgorithmType::Pascal;
				coinP=QStringList({"-dcoin", "pasc"});
				}
			if (pair->CurrentExtraLaunchParameters.join("").contains("Blake2s")) {
				dual=Enums::AlgorithmType::Blake2s;
				coinP=QStringList({"-dcoin", "blake2s"});
				}
			if (pair->CurrentExtraLaunchParameters.join("").contains("Keccak")) {
				dual=Enums::AlgorithmType::Keccak;
				coinP=QStringList({"-dcoin", "keccak"});
				}
			if (dual!=Enums::AlgorithmType::NONE) {
				dualModeParams << coinP << "-dpool" << Globals::GetLocationURL(dual, Globals::MiningLocation[ConfigManager.generalConfig->ServiceLocation], _ConectionType) << "-dwal" << username;
				break;
				}
			}
		}
	else {
		dualModeParams << "-dcoin" << SecondaryShortName() << "-dpool" << Globals::GetLocationURL(SecondaryAlgorithmType, Globals::MiningLocation[ConfigManager.generalConfig->ServiceLocation], _ConectionType) << "-dwal" << username << "-dpsw" << "x";
		}

	return GetDevicesCommandString()
			<< "-epool" << url << "-ewal" << username << "-mport" << QString("127.0.0.1:-%1").arg(ApiPort()) << "-esm" << "3" << "-epsw" << "x" << "-allpools" << "1"
			<< dualModeParams
			;
}

void ClaymoreDual::Start(QString url, QString btcAdress, QString worker)
{
	// Update to most profitable intensity
	foreach (MiningPair* mPair, *MiningSetup_->MiningPairs) {
		DualAlgorithm* algo=qobject_cast<DualAlgorithm*>(mPair->algorithm);
		if (algo!=nullptr && algo->TuningEnabled) {
			int intensity=algo->MostProfitableIntensity();
			if (intensity<0) {
				intensity=defaultIntensity;
				}
			algo->CurrentIntensity=intensity;
			}
		}
	LastCommandLine=GetStartCommand(url, btcAdress, worker) << "-dbg" << "-1";
	ProcessHandle=_Start();
}

QStringList ClaymoreDual::DeviceCommand(int amdCount/*=1*/)
{
	// If no AMD cards loaded, instruct CD to only regard NV cards for indexing
	// This will allow proper indexing if AMD GPUs or APUs are present in the system but detection disabled
	QStringList ret= !amdCount? QStringList({"-platform", "2"}) : QStringList();
	return ret << ClaymoreBaseMiner::DeviceCommand(amdCount);
}

QStringList ClaymoreDual::BenchmarkCreateCommandLine(Algorithm* algorithm, int time)
{
	// network stub
	QString url=GetServiceUrl(algorithm->NiceHashID);
	// demo for benchmark
	QStringList ret=GetStartCommand(url, Globals::GetBitcoinUser(), ConfigManager.generalConfig->WorkerName.trimmed()) << "-logfile" << GetLogFileName();
	// local benchmark
	if (!IsDual()) {
		BenchmarkTimeWait=time;
		ret << "-benchmark" << "1"; // benchmark 1 does not output secondary speeds
		return ret;
		}
	BenchmarkTimeWait=std::max(60, std::min(120, time*3)); // dual seems to stop mining after this time if redirect output is true
	return ret; // benchmark 1 does not output secondary speeds
}
