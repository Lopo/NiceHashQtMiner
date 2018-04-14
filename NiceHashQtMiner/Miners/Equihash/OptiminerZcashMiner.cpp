#include "OptiminerZcashMiner.h"
#include "Miners/Parsing/ExtraLaunchParametersParser.h"
#include "Devices/ComputeDeviceManager.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Utils/Helpers.h"
#include "Algorithm.h"
#include <QtConcurrent/QtConcurrentRun>
#include <QJsonObject>
#include <QJsonDocument>


OptiminerZcashMiner::OptiminerZcashMiner()
	: Miner("OptiminerZcashMiner", 60*1000*5)
{
	_ConectionType=Enums::NhmConectionType::NONE;
}

void OptiminerZcashMiner::Start(QString url, QString btcAdress, QString worker)
{
	QString username=GetUsername(btcAdress, worker);
	LastCommandLine=GetDevicesCommandString() << "-m" << QString::number(ApiPort()) << "-s" << url << "-u" << username << "-p" << "x";
	ProcessHandle=_Start();

	_startApi=new QElapsedTimer;
	_startApi->start();
}

void OptiminerZcashMiner::_Stop(Enums::MinerStopType willswitch)
{
	Stop_cpu_ccminer_sgminer_nheqminer(willswitch);
}

QStringList OptiminerZcashMiner::GetDevicesCommandString()
{
	QStringList extraParams=ExtraLaunchParametersParser::ParseForMiningSetup(_MiningSetup, Enums::DeviceType::AMD);
	QStringList deviceStringCommand({"-c", QString::number(ComputeDeviceManager.Avaliable.AmdOpenCLPlatformNum)});
	QStringList ids;
	foreach (MiningPair* mPair, *_MiningSetup->MiningPairs) {
		ids.append({"-d", QString::number(mPair->Device->ID)});
		}
	deviceStringCommand << ids
			<< extraParams;

	return deviceStringCommand;
}

ApiData* OptiminerZcashMiner::GetSummaryAsync()
{
	CurrentMinerReadStatus=Enums::MinerApiReadStatus::NONE;
	ApiData* ad=new ApiData(_MiningSetup->CurrentAlgorithmType);

	if (!_skipApiCheck) {
		JsonApiResponse* resp=nullptr;
		try {
			QString* respStr=QtConcurrent::run(this, static_cast<QString*(OptiminerZcashMiner::*)(int, QString, bool, bool)>(&Miner::GetApiDataAsync), ApiPort(), GetHttpRequestNhmAgentStrin(""), true, false).result();
			if (respStr!=nullptr && respStr->contains('{')) {
				int start=respStr->indexOf('{');
				if (start>-1) {
					QString respStrJson=respStr->mid(start);
					resp=JsonApiResponse::fromJson(respStrJson.trimmed());
					}
				}
			}
		catch (std::exception ex) {
			Helpers::ConsolePrint("OptiminerZcashMiner", QString("GetSummary exception: ")+ex.what());
			}

		if (resp!=nullptr && resp->solution_rate!=nullptr) {
			const QString totalKey="Total";
			const QString _5SKey="5s";
			if (resp->solution_rate->contains(totalKey)) {
				QMap<QString, double>* totalSolutionRateDict=resp->solution_rate->value(totalKey, nullptr);
				if (totalSolutionRateDict!=nullptr && totalSolutionRateDict->contains(_5SKey)) {
					ad->Speed=totalSolutionRateDict->value(_5SKey);
					CurrentMinerReadStatus=Enums::MinerApiReadStatus::GOT_READ;
					}
				}
			if (!ad->Speed) {
				CurrentMinerReadStatus=Enums::MinerApiReadStatus::READ_SPEED_ZERO;
				}
			delete resp;
			}
		}
	else if (_skipApiCheck && (_startApi->elapsed()/1000)>_waitSeconds) {
		_startApi->invalidate();
		_skipApiCheck=false;
		}
	return ad;
}

QStringList OptiminerZcashMiner::BenchmarkCreateCommandLine(Algorithm* algorithm, int time)
{
	int t=time/9; // sgminer needs 9x more than this miner so reduce benchmark  speed
	return GetDevicesCommandString() << "--benchmark" << QString::number(t);
}

void OptiminerZcashMiner::BenchmarkOutputErrorDataReceivedImpl(QString outdata)
{
	CheckOutdata(outdata);
}

bool OptiminerZcashMiner::BenchmarkParseLine(QString outdata)
{
	const QString find="Benchmark:";
	if (outdata.contains(find)) {
		int start=outdata.indexOf(find)+find.length();
		QString itersAndVars=outdata.mid(start).trimmed();
		QStringList ar=itersAndVars.split(' ');
		if (ar.count()>=4) {
			// gets sols/s
			BenchmarkAlgorithm->BenchmarkSpeed=Helpers::ParseDouble(ar.value(2));
			return true;
			}
		}
	return false;
}

OptiminerZcashMiner::JsonApiResponse* OptiminerZcashMiner::JsonApiResponse::fromJson(QString str) // @todo implement/finish
{
	QJsonObject o=QJsonDocument::fromJson(str.toLatin1()).object();
	OptiminerZcashMiner::JsonApiResponse* ret=new OptiminerZcashMiner::JsonApiResponse;
	ret->uptime=o.value("uptime").toDouble();

	ret->solution_rate=new QMap<QString, QMap<QString, double>*>;
	QJsonObject sro=o.value("solution_rate").toObject();
	foreach (QJsonValue srk, sro.keys()) {
		QJsonObject srio=sro.value(srk.toString()).toObject();
		QMap<QString, double>* _sri=new QMap<QString, double>;
		foreach (QJsonValue srik, srio.keys()) {
			QString k=srik.toString();
			_sri->insert(k, srio.value(k).toDouble());
			}
		ret->solution_rate->insert(srk.toString(), _sri);
		}

	QJsonObject so=o.value("share").toObject();
	foreach (QJsonValue sok, so.keys()) {
		ret->share.insert(sok.toString(), so.value(sok.toString()).toInt());
		}

	QJsonObject iro=o.value("iteration_rate").toObject();
	foreach (QJsonValue irk, iro.keys()) {
		QJsonObject irio=sro.value(irk.toString()).toObject();
		QMap<QString, double> _sri;
		foreach (QJsonValue irik, irio.keys()) {
			QString k=irik.toString();
			_sri.insert(k, irio.value(k).toDouble());
			}
		ret->iteration_rate.insert(irk.toString(), _sri);
		}

	QJsonObject s=o.value("stratum").toObject();
	ret->stratum.target=s.value("target").toString();
	ret->stratum.connected=s.value("connected").toBool();
	ret->stratum.connection_failures=s.value("connection_failures").toInt();
	ret->stratum.host=s.value("host").toString();
	ret->stratum.port=s.value("port").toInt();

	return ret;
}
