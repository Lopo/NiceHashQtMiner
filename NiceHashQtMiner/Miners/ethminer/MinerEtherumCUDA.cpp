#include "Miners/ethminer/MinerEtherumCUDA.h"
#include "Utils/Helpers.h"
#include "Miners/Parsing/ExtraLaunchParametersParser.h"


QList<MinerEtherum*>* MinerEtherumCUDA::MinerEtherumCudaList=new QList<MinerEtherum*>;


MinerEtherumCUDA::MinerEtherumCUDA()
	: MinerEtherum("MinerEtherumCUDA", "NVIDIA")
{
	MinerEtherumCudaList->append(this);
}

MinerEtherumCUDA::~MinerEtherumCUDA()
{
	// remove from list
	MinerEtherumCudaList->removeAll(this);
}

void MinerEtherumCUDA::Start(QString url, QString btcAdress, QString worker)
{
	Helpers::ConsolePrint(MinerTag(), "Starting MinerEtherumCUDA, checking existing MinerEtherumCUDA to stop");
	MinerEtherum::Start(url, btcAdress, worker, *MinerEtherumCudaList);
}

QStringList MinerEtherumCUDA::GetStartCommandStringPart(QString url, QString username)
{
	QStringList ret;
	ret << "--cuda"
			<< ExtraLaunchParametersParser::ParseForMiningSetup(_MiningSetup, Enums::DeviceType::NVIDIA)
			<< "-S" << url.mid(14)
			<< "-O" << username+":x"
			<< "-SP" << QString::number(2)
			<< "--api-port" << QString::number(ApiPort())
			<< "--cuda-devices";
	ret.removeAll("");
	return ret;
}

QStringList MinerEtherumCUDA::GetBenchmarkCommandStringPart(Algorithm* algorithm)
{
	QStringList ret;
	ret << "--benchmark-warmup" << QString::number(40) << "--benchmark-trial" << QString::number(20)
			<< ExtraLaunchParametersParser::ParseForMiningSetup(_MiningSetup, Enums::DeviceType::NVIDIA)
			<< "--cuda" << "--cuda-devices";
	ret.removeAll("");
	return ret;
}
