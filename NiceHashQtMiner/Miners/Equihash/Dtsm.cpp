#include "Miners/Equihash/Dtsm.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Miners/Parsing/ExtraLaunchParametersParser.h"
#include "Algorithms/Algorithm.h"
#include "Globals.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Utils/Helpers.h"
#include <QtConcurrent/QtConcurrentRun>
#include <QJsonObject>
#include <QJsonArray>


Dtsm::Dtsm()
	: Miner("dtsm", 60*1000*5)
{
	_ConectionType=Enums::NhmConectionType::NONE;
}

void Dtsm::Start(QString url, QString btcAdress, QString worker)
{
	LastCommandLine=GetStartCommand(url, btcAdress, worker);
	ProcessHandle=_Start();
}

QStringList Dtsm::GetStartCommand(QString url, QString btcAddress, QString worker)
{
	QStringList urls=url.split(':');
	QString server= urls.length()>0? urls[0] : "";
	QString port= urls.length()>1? urls[1] : "";
	return GetDeviceCommand()
			<< "--server" << server
			<< "--port" << port
			<< "--user" << btcAddress+"."+worker
			<< "--telemetry=127.0.0.1:"+QString::number(ApiPort());
}

QStringList Dtsm::GetDeviceCommand()
{
	QStringList ret({"--dev"});
	foreach (MiningPair* p, *MiningSetup_->MiningPairs) {
		ret << QString::number(p->Device->ID);
		}
	return ret << ExtraLaunchParametersParser::ParseForMiningSetup(MiningSetup_, Enums::DeviceType::NVIDIA);
}

void Dtsm::_Stop(Enums::MinerStopType willswitch)
{
	Stop_cpu_ccminer_sgminer_nheqminer(willswitch);
}

QStringList Dtsm::BenchmarkCreateCommandLine(Algorithm* algorithm, int time)
{
	QString url=GetServiceUrl(algorithm->NiceHashID);

	_benchmarkTime=std::max(time, 60);

	return GetStartCommand(url, Globals::GetBitcoinUser(), ConfigManager.generalConfig->WorkerName.trimmed()) << "--logfile="+GetLogFileName();
}

void Dtsm::BenchmarkThreadRoutine(QStringList commandLine)
{
	commandLine.removeAll("");
	BenchmarkThreadRoutineAlternate(commandLine, _benchmarkTime);
}

void Dtsm::ProcessBenchLinesAlternate(QStringList lines)
{
	double benchSum=0;
	int benchCount=0;

	foreach (QString line, lines) {
		BenchLines->append(line);
		QString lowered=line.toLower();

		int start=lowered.indexOf(LookForStart);
		if (start<=-1) {
			continue;
			}
		lowered=lowered.mid(start, lowered.length()-start).remove(LookForStart);
		int end=lowered.indexOf(LookForEnd);
		lowered=lowered.mid(0, end);
		bool ok;
		double speed=lowered.toDouble(&ok);
		if (ok) {
			benchSum+=speed;
			benchCount++;
			}
		}
	BenchmarkAlgorithm->BenchmarkSpeed((benchSum/std::max(1, benchCount))*(1-DevFee*0.01));
}

void Dtsm::BenchmarkOutputErrorDataReceivedImpl(QString outdata)
{ }

bool Dtsm::BenchmarkParseLine(QString outdata)
{
	return false;
}

ApiData* Dtsm::GetSummaryAsync()
{
	CurrentMinerReadStatus=Enums::MinerApiReadStatus::NONE;

	ApiData* ad=new ApiData(MiningSetup_->CurrentAlgorithmType);

	QString* response=QtConcurrent::run(this, static_cast<QString*(Dtsm::*)(int, QString, bool, bool)>(&Miner::GetApiDataAsync), ApiPort(), QString("{\"method\":\"getstat\",\"id\"=1}\n"), false, false).result();
	DtsmResponse* resp=nullptr;

	try {
		if (response!=nullptr) {
			resp=DtsmResponse::fromJson(*response);
			delete response;
			}
		}
	catch (QException& e) {
		Helpers::ConsolePrint(MinerTag(), e.what());
		}

	if (resp!=nullptr && resp->result!=nullptr) {
		ad->Speed=0;
		foreach (DtsmGpuResult* gpu, *resp->result) {
			ad->Speed+=gpu->sol_ps;
			}
		CurrentMinerReadStatus=Enums::MinerApiReadStatus::GOT_READ;
		delete resp;
		}
	if (!ad->Speed) {
		CurrentMinerReadStatus=Enums::MinerApiReadStatus::READ_SPEED_ZERO;
		}

	return ad;
}

bool Dtsm::IsApiEof(uchar third, uchar second, uchar last)
{
	return second==125 && last==10;
}

Dtsm::DtsmResponse* Dtsm::DtsmResponse::fromJson(QString json)
{
	QJsonParseError e;
	QJsonDocument d=QJsonDocument::fromJson(json.toLatin1(), &e);
	if (e.error) {
		return nullptr;
		}
	QJsonObject o=d.object();
	if (o.value("result")==QJsonValue::Undefined) {
		return nullptr;
		}

	DtsmResponse* ret=new DtsmResponse;
	ret->result=new QList<DtsmGpuResult*>;
	DtsmGpuResult* result;
	foreach (QJsonValue r, o.value("result").toArray()) {
		result=new DtsmGpuResult;
		result->sol_ps=r.toObject().value("sol_ps").toDouble();
		ret->result->append(result);
		}

	return ret;
}
