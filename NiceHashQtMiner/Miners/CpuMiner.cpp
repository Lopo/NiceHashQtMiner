#include "Miners/CpuMiner.h"
#include "Utils/Helpers.h"
#include "Parsing/ExtraLaunchParametersParser.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "PInvoke/CPUID.h"
#include "PInvoke/NiceHashProcess.h"
#include "Algorithms/Algorithm.h"
#include <QProcess>

CpuMiner::CpuMiner()
	: Miner("cpuminer_CPU", 3600000)
{
}

void CpuMiner::Start(QString url, QString btcAdress, QString worker)
{
	if (!IsInit()) {
		Helpers::ConsolePrint(MinerTag(), "MiningSetup is not initialized exiting Start()");
		return;
		}

	QString username=GetUsername(btcAdress, worker);

	LastCommandLine=QStringList()
		<< "--algo" << miningSetup()->MinerName
		<< "--url" << url
		<< (QString("--userpass=")+username+":x")
		<< ExtraLaunchParametersParser::ParseForMiningSetup(miningSetup(), Enums::DeviceType::CPU)
		<< ("--api-bind="+QString::number(ApiPort()));

	ProcessHandle=_Start();
}

ApiData* CpuMiner::GetSummaryAsync()
{
	return GetSummaryCpuCcminerAsync();
}

void CpuMiner::_Stop(Enums::MinerStopType willswitch)
{
	Stop_cpu_ccminer_sgminer_nheqminer(willswitch);
}

NiceHashProcess* CpuMiner::_Start()
{
	NiceHashProcess* p=Miner::_Start();

	ulong affinityMask=miningSetup()->MiningPairs->value(0)->Device->AffinityMask();
	if (affinityMask!=0 && p!=nullptr) {
		NiceHashQtMiner::CPUID::AdjustAffinity(p->Id, affinityMask);
		}

	return p;
}

QStringList CpuMiner::BenchmarkCreateCommandLine(Algorithm* algorithm, int time)
{
	return QStringList() << ("--algo="+algorithm->MinerName)
			<< "--benchmark"
			<< ExtraLaunchParametersParser::ParseForMiningSetup(miningSetup(), Enums::DeviceType::CPU)
			<< "--time-limit" << QString::number(time);
}

QProcess* CpuMiner::BenchmarkStartProcess(QStringList CommandLine)
{
	QProcess* benchmarkHandle=Miner::BenchmarkStartProcess(CommandLine);

	ulong affinityMask=miningSetup()->MiningPairs->value(0)->Device->AffinityMask();
	if (affinityMask!=0 && benchmarkHandle!=nullptr) {
		NiceHashQtMiner::CPUID::AdjustAffinity(benchmarkHandle->pid(), affinityMask);
		}

	return benchmarkHandle;
}

bool CpuMiner::BenchmarkParseLine(QString outdata)
{
	bool ok=false;
	double lastSpeed=outdata.toDouble(&ok);
	if (!ok) {
		return false;
		}

	BenchmarkAlgorithm->BenchmarkSpeed(lastSpeed);
	return true;
}

void CpuMiner::BenchmarkOutputErrorDataReceivedImpl(QString outdata)
{
	CheckOutdata(outdata);
}
