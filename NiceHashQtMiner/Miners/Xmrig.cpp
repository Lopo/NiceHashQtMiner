#include "Miners/Xmrig.h"
#include "Parsing/ExtraLaunchParametersParser.h"
#include "Globals.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Algorithm.h"
#include "Utils/Helpers.h"
#include <QtConcurrent/QtConcurrentRun>
#include <QRegularExpression>


Xmrig::Xmrig()
	: Miner("Xmrig", 60*1000*5)
{ }

void Xmrig::Start(QString url, QString btcAdress, QString worker)
{
	LastCommandLine=GetStartCommand(url, btcAdress, worker);
	ProcessHandle=_Start();
}

QStringList Xmrig::GetStartCommand(QString url, QString btcAdress, QString worker)
{
	return QStringList() << "-o" << url << "-u" << QString("%1.%2:x").arg(btcAdress).arg(worker) << "--nicehash" << ExtraLaunchParametersParser::ParseForMiningSetup(_MiningSetup, Enums::DeviceType::CPU) << "--api-port" << QString::number(ApiPort());
}

void Xmrig::_Stop(Enums::MinerStopType willswitch)
{
	Stop_cpu_ccminer_sgminer_nheqminer(willswitch);
}

ApiData* Xmrig::GetSummaryAsync()
{
	return QtConcurrent::run(this, static_cast<ApiData*(Xmrig::*)(QString, bool)>(&Miner::GetSummaryCpuAsync), QString(""), false).result();
}

bool Xmrig::IsApiEof(uchar third, uchar second, uchar last)
{
	return third==0x7d && second==0xa && last==0x7d;
}

QStringList Xmrig::BenchmarkCreateCommandLine(Algorithm* algorithm, int time)
{
	QString server=Globals::GetLocationURL(algorithm->NiceHashID, Globals::MiningLocation[ConfigManager.generalConfig->ServiceLocation], _ConectionType);
	_benchmarkTimeWait=time;
	return GetStartCommand(server, Globals::GetBitcoinUser(), ConfigManager.generalConfig->WorkerName.trimmed())
		<< "-l" << "benchmark_log.txt" << "--print-time=2";
}

void Xmrig::BenchmarkThreadRoutine(QStringList commandLine)
{
	BenchmarkThreadRoutineAlternate(commandLine, _benchmarkTimeWait);
}

void Xmrig::ProcessBenchLinesAlternate(QStringList lines)
{
	// Xmrig reports 2.5s and 60s averages, so prefer to use 60s values for benchmark
	// but fall back on 2.5s values if 60s time isn't hit
	double twoSecTotal=0;
	double sixtySecTotal=0;
	int twoSecCount=0;
	int sixtySecCount=0;

	foreach (QString line, lines) {
		BenchLines->append(line);
		QString lineLowered=line.toLower();
		if (!lineLowered.contains(LookForStart)) {
			continue;
			}
		QStringList speeds=QRegularExpression(LookForStart+" (.+?) "+LookForEnd).match(lineLowered).captured(1).split(' ');
		bool ok=false;
		double sixtySecSpeed=speeds[1].toDouble(&ok);
		if (ok) {
			sixtySecTotal+=sixtySecSpeed;
			++sixtySecCount;
			}
		else {
			double twoSecSpeed=speeds[0].toDouble(&ok);
			if (ok) {
				twoSecTotal+=twoSecSpeed;
				++twoSecCount;
				}
			}
		}

	if (sixtySecCount>0 && sixtySecTotal>0) {
		// Run iff 60s averages are reported
		BenchmarkAlgorithm->BenchmarkSpeed=sixtySecTotal/sixtySecCount;
		}
	else if (twoSecCount>0) {
		// Run iff no 60s averages are reported but 2.5s are
		BenchmarkAlgorithm->BenchmarkSpeed=twoSecTotal/twoSecCount;
		}
}

void Xmrig::BenchmarkOutputErrorDataReceivedImpl(QString outdata)
{
	CheckOutdata(outdata);
}

bool Xmrig::BenchmarkParseLine(QString outdata)
{
	Helpers::ConsolePrint(MinerTag(), outdata);
	return false;
}
