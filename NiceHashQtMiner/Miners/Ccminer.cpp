#include "Miners/Ccminer.h"
#include "Utils/Helpers.h"
#include "Parsing/ExtraLaunchParametersParser.h"
#include "Algorithms/Algorithm.h"
#include "PInvoke/NiceHashProcess.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "WinPort/Process.h"
#include <QtConcurrent/QtConcurrentRun>
#include <QDir>


Ccminer::Ccminer()
	: Miner("ccminer_NVIDIA", /*_MiningSetup->MinerPath==MinerPaths.Data.CcminerX11Gost? 60*1000*3 : 60*1000*/60*1000*3)
{ }

void Ccminer::Start(QString url, QString btcAdress, QString worker)
{
	if (!IsInit()) {
		Helpers::ConsolePrint(MinerTag(), "MiningSetup is not initialized exiting Start()");
		return;
		}
	QString username=GetUsername(btcAdress, worker);

	_IsApiReadException=MiningSetup_->MinerPath==MinerPaths.Data.CcminerCryptonight;

	QString algo="";
	QString apiBind="";
	if (!_IsApiReadException) {
		algo="--algo="+MiningSetup_->MinerName;
		apiBind="--api-bind="+QString::number(ApiPort());
		}

	LastCommandLine=QStringList() << algo
			<< "--url="+url
			<< "--userpass="+username+":x"
			<< apiBind
			<< "--devices" << GetDevicesCommandString()
			<< ExtraLaunchParametersParser::ParseForMiningSetup(MiningSetup_, Enums::DeviceType::NVIDIA);
	LastCommandLine.removeAll("");

	ProcessHandle=_Start();
}

void Ccminer::_Stop(Enums::MinerStopType willswitch)
{
	Stop_cpu_ccminer_sgminer_nheqminer(willswitch);
}

QStringList Ccminer::BenchmarkCreateCommandLine(Algorithm* algorithm, int time)
{
	QStringList commandLine=QStringList() << "--algo="+algorithm->MinerName
			<< "--no-color"
			<< "--benchmark"
			<< (_benchmarkException()? QStringList() : QStringList({"--time-limit", QString::number(time)}))
			<< ExtraLaunchParametersParser::ParseForMiningSetup(MiningSetup_, Enums::DeviceType::NVIDIA)
			<< "--devices" << GetDevicesCommandString();

	// cryptonight exception helper variables
	_cryptonightTotalCount=BenchmarkTimeInSeconds/_cryptonightTotalDelim;
	_cryptonightTotal=0.0d;

	return commandLine;
}

bool Ccminer::BenchmarkParseLine(QString outdata)
{
	// cryptonight exception
	if (_benchmarkException()) {
		int speedLength= (BenchmarkAlgorithm->NiceHashID==Enums::AlgorithmType::CryptoNight)? 6 : 8;
		if (outdata.contains("Total: ")) {
			int st=outdata.indexOf("Total:")+7;
			int len=outdata.length()-speedLength-st;

			QString parse=outdata.mid(st, len).trimmed();
			bool ok;
			double tmp=parse.toDouble(&ok);

			// save speed
			int i=outdata.indexOf("Benchmark:");
			int k=outdata.indexOf("/s");
			QString hashspeed=outdata.mid(i+11, k-i-9);
//			int b=hashspeed.indexOf(' ');
			if (hashspeed.contains("kH/s")) {
				tmp*=1000;
				}
			else if (hashspeed.contains("MH/s")) {
				tmp*=1000000;
				}
			else if (hashspeed.contains("GH/s")) {
				tmp*=1000000000;
				}

			_cryptonightTotal+=tmp;
			_cryptonightTotalCount--;
			}
		if (_cryptonightTotalCount<=0) {
			BenchmarkAlgorithm->BenchmarkSpeed(_cryptonightTotal/((double)BenchmarkTimeInSeconds/_cryptonightTotalDelim));
			BenchmarkSignalFinnished=true;
			}

		return false;
		}

	double lastSpeed=BenchmarkParseLine_cpu_ccminer_extra(outdata);
	if (lastSpeed>0.0d) {
		BenchmarkAlgorithm->BenchmarkSpeed(lastSpeed);
		return true;
		}

	bool ok;
	lastSpeed=outdata.toDouble(&ok);
	if (ok) {
		BenchmarkAlgorithm->BenchmarkSpeed(lastSpeed);
		return true;
		}
	return false;
}

void Ccminer::BenchmarkOutputErrorDataReceivedImpl(QString outdata)
{
	CheckOutdata(outdata);
}

ApiData* Ccminer::GetSummaryAsync()
{
	// CryptoNight does not have api bind port
	if (!_IsApiReadException) {
		return QtConcurrent::run(this, static_cast<ApiData*(Ccminer::*)()>(&Miner::GetSummaryCpuCcminerAsync)).result();
		}
	// check if running
	if (ProcessHandle==nullptr) {
		CurrentMinerReadStatus=Enums::MinerApiReadStatus::RESTART;
		Helpers::ConsolePrint(MinerTag(), ProcessTag()+" Could not read data from CryptoNight Process is null");
		return nullptr;
		}
	try {
//		Process::GetProcessById(ProcessHandle->Id);
		if (!Process::CheckProcessById(ProcessHandle->Id)) {
			Helpers::ConsolePrint(MinerTag(), ProcessTag()+QString(" Could not read data from CryptoNight reason: pid not found").arg(ProcessHandle->Id));
			return nullptr; // will restart outside
			}
		}
	catch (std::invalid_argument& ex) { // !!!
		CurrentMinerReadStatus=Enums::MinerApiReadStatus::RESTART;
		Helpers::ConsolePrint(MinerTag(), ProcessTag()+" Could not read data from CryptoNight reason: "+ex.what());
		return nullptr; // will restart outside
		}
	catch (std::exception& ex) { // !!! InvalidOperationException
		CurrentMinerReadStatus=Enums::MinerApiReadStatus::RESTART;
		Helpers::ConsolePrint(MinerTag(), ProcessTag()+" Could not read data from CryptoNight reason: "+ex.what());
		return nullptr; // will restart outside
		}

	double totalSpeed=0.0d;
	foreach (MiningPair* miningPair, *MiningSetup_->MiningPairs) {
		Algorithm* algo=miningPair->Device->GetAlgorithm(Enums::MinerBaseType::ccminer, Enums::AlgorithmType::CryptoNight, Enums::AlgorithmType::NONE);
		if (algo!=nullptr) {
			totalSpeed+=algo->BenchmarkSpeed();
			}
		}

	ApiData* cryptoNightData=new ApiData(MiningSetup_->CurrentAlgorithmType);
	cryptoNightData->Speed=totalSpeed;
	CurrentMinerReadStatus=Enums::MinerApiReadStatus::GOT_READ;
	// check if speed zero
	if (!cryptoNightData->Speed) {
		CurrentMinerReadStatus=Enums::MinerApiReadStatus::READ_SPEED_ZERO;
		}
	return cryptoNightData;
}
