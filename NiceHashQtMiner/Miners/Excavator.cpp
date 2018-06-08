#include "Miners/Excavator.h"
#include "Algorithms/Algorithm.h"
#include "Algorithms/DualAlgorithm.h"
#include "Globals.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Utils/Helpers.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTcpSocket>
#include <QMetaEnum>


ExcavatorCommand::ExcavatorCommand(QString iMethod, QStringList iParams)
{
	method=iMethod;
	params=iParams;
}

QJsonObject ExcavatorCommand::asJObject()
{
	QJsonObject o;
	o.insert("id", id);
	o.insert("method", method);
	o.insert("params", QJsonArray::fromStringList(params));
	return o;
}

ExcavatorAction::ExcavatorAction(int iTime, int iLoop/*=0*/)
{
	time=iTime;
	loop=iLoop;
}

QJsonObject ExcavatorAction::asJObject()
{
	QJsonObject o;
	o.insert("time", time);
	if (loop) {
		o.insert("loop", loop);
		}
	QJsonArray a;
	foreach (ExcavatorCommand cmd, commands) {
		a.append(cmd.asJObject());
		}
	o.insert("commands", a);
	return o;
}


Excavator::Excavator(Enums::AlgorithmType secondaryAlgorithmType/*=Enums::AlgorithmType::NONE*/)
	: Miner("excavator", 60*1000*5)
{
	_ConectionType=Enums::NhmConectionType::NONE;
//	IsNeverHideMiningWindow=true;
	SecondaryAlgorithmType=secondaryAlgorithmType;
}

bool Excavator::IsDual()
{
	return SecondaryAlgorithmType!=Enums::AlgorithmType::NONE;
}

void Excavator::Start(QString url, QString btcAdress, QString worker)
{
	QString cName=GetConfigFileName();
	QFile f(WorkingDirectory()+cName);
	f.open(QIODevice::Text|QIODevice::WriteOnly);
	f.write(jsonConfig(url, btcAdress, worker).toLatin1());
	f.close();
	LastCommandLine=QStringList({"-c", cName, "-p", QString::number(ApiPort())});
	ProcessHandle=_Start();
}

QStringList Excavator::BenchmarkCreateCommandLine(::Algorithm* algorithm, int time)
{
	_benchmarkTimeWait=time;
	QString cName=GetConfigFileName();
	QFile f(WorkingDirectory()+cName);
	f.open(QIODevice::Text|QIODevice::WriteOnly);
	f.write(jsonBenchConfig(GetServiceUrl(algorithm->NiceHashID), Globals::GetBitcoinUser(), ConfigManager.generalConfig->WorkerName.trimmed()).toLatin1());
	f.close();
	QStringList ret={"-c", cName, "-p", "0"};
	return ret;
}

bool Excavator::BenchmarkParseLine(QString outdata)
{
	if (outdata.contains(totSpeed)) {
		try {
			int speedStart=outdata.indexOf(totSpeed);
			QString speed=outdata.mid(speedStart, outdata.length()-speedStart).remove(totSpeed);
			speed=speed.mid(0, speed.lastIndexOf("/s")+2);
			if (IsDual()) {
				QStringList splitDual=speed.trimmed().split('&');
				BenchmarkAlgorithm->BenchmarkSpeed(GetNumber(splitDual.at(0).trimmed()));
				DualAlgorithm* dualBenchAlgo=qobject_cast<DualAlgorithm*>(BenchmarkAlgorithm);
				if (dualBenchAlgo!=nullptr) {
					dualBenchAlgo->SecondaryBenchmarkSpeed(GetNumber(splitDual.at(1).trimmed()));
					}
				return true;
				}
			if (speed.trimmed().split(' ').count()>=2) {
				double spd=GetNumber(speed);

				// wrong benchmark workaround over 3gh/s is considered false
				if (MiningSetup_->CurrentAlgorithmType==Enums::AlgorithmType::Pascal && spd>3.0d*1000000000.0d) {
					return false;
					}

				BenchmarkAlgorithm->BenchmarkSpeed(spd);
				return true;
				}
			}
		catch (...) { }
		}
	return false;
}

ApiData* Excavator::GetSummaryAsync()
{
	CurrentMinerReadStatus=Enums::MinerApiReadStatus::NONE;
	ApiData* ad=new ApiData(MiningSetup_->CurrentAlgorithmType);

	JsonApiResponse* resp=nullptr;
	try {
		QTcpSocket client;
		client.connectToHost("127.0.0.1", ApiPort());
		client.waitForConnected(1000);
		client.write("{\"id\":1,\"method\":\"algorithm.list\",\"params\":[]}\n");
		client.flush();
		client.waitForReadyRead();
		resp=JsonApiResponse::fromJson(client.readAll());
		client.close();
		}
	catch (std::exception& ex) {
		Helpers::ConsolePrint("ERROR", ex.what());
		}

	if (resp!=nullptr && resp->error==nullptr) {
		ad->Speed=0;
		foreach (Algorithm algo, resp->algorithms) {
			if (algo.algorithm_id==MiningSetup_->CurrentAlgorithmType) {
				ad->Speed=algo.speed;
				}
			if (IsDual() && algo.algorithm_id==MiningSetup_->CurrentSecondaryAlgorithmType) {
				ad->SecondarySpeed=algo.speed;
				}
			}
		CurrentMinerReadStatus=Enums::MinerApiReadStatus::GOT_READ;
		if (!ad->Speed) {
			CurrentMinerReadStatus=Enums::MinerApiReadStatus::READ_SPEED_ZERO;
			}
		}
	return ad;
}

void Excavator::_Stop(Enums::MinerStopType willswitch)
{
	try {
		QTcpSocket client;
		client.connectToHost("127.0.0.1", ApiPort());
		client.waitForConnected(1000);
		client.write("{\"id\":1,\"method\":\"quit\",\"params\":[]}\n");
		client.flush();
		client.close();
		}
	catch (std::exception& ex) {
		Helpers::ConsolePrint("ERROR", ex.what());
		}
	Stop_cpu_ccminer_sgminer_nheqminer(willswitch);
}

void Excavator::BenchmarkOutputErrorDataReceivedImpl(QString outdata)
{
	CheckOutdata(outdata);
}

Excavator::JsonApiResponse* Excavator::JsonApiResponse::fromJson(QString json)
{
	JsonApiResponse* ret=new JsonApiResponse;
	QJsonObject o=QJsonDocument::fromJson(json.toLatin1()).object();
	ret->id=o.value("id").toInt();
	ret->error=o.value("error").isNull()? nullptr : new QJsonObject(o.value("error").toObject());

	foreach (QJsonValue av, o.value("algorithms").toArray()) {
		QJsonObject ao=av.toObject();
		Algorithm a;
		a.algorithm_id=ao.value("algorithm_id").toInt();
		a.name=ao.value("name").toString();
		a.speed=ao.value("speed").toDouble();
		a.uptime=ao.value("uptime").toDouble();
		a.benchmark=ao.value("benchmark").toBool();
		a.accepted_shares=ao.value("accepted_shares").toInt();
		a.rejected_shares=ao.value("rejected_shares").toInt();
		a.got_job=ao.value("got_job").toBool();
		a.received_jobs=ao.value("received_jobs").toInt();
		a.current_job_difficulty=ao.value("current_job_difficulty").toDouble();
		ret->algorithms.append(a);
		}

	return ret;
}

QString Excavator::GetConfigFileName()
{
	return QString("config_%1.cfg").arg(MiningSetup_->MiningPairs->at(0)->Device->ID);
}

QString Excavator::jsonConfig(QString url, QString btcAdress, QString worker)
{
	ExcavatorAction* act;
	QJsonArray cfg;

	act=new ExcavatorAction();
	act->commands.append(ExcavatorCommand("subscribe", {QString("nhmp.%1.nicehash.com:3200").arg(Globals::MiningLocation[ConfigManager.generalConfig->ServiceLocation]), GetUsername(btcAdress, worker)}));
	cfg.append(act->asJObject());

	act=new ExcavatorAction(2);
	if (!IsDual()) {
		act->commands.append(ExcavatorCommand("algorithm.add", {MiningSetup_->MinerName}));
		}
	else {
		act->commands.append(ExcavatorCommand("algorithm.add", {"daggerhashimoto"}));
		switch (SecondaryAlgorithmType) {
			case Enums::AlgorithmType::Sia:
				act->commands.append(ExcavatorCommand("algorithm.add", {"sia"}));
				break;
			case Enums::AlgorithmType::Pascal:
				act->commands.append(ExcavatorCommand("algorithm.add", {"pascal"}));
				break;
			case Enums::AlgorithmType::Decred:
				act->commands.append(ExcavatorCommand("algorithm.add", {"decred"}));
				break;
			default:
				break;
			}
		}
	cfg.append(act->asJObject());

	act=new ExcavatorAction(2);
	QString algo= IsDual()
			? QString("daggerhashimoto_%1").arg(QMetaEnum::fromType<Enums::AlgorithmType>().valueToKey((int)SecondaryAlgorithmType)).toLower()
			: MiningSetup_->MinerName;
	foreach (MiningPair* nvidiaPair, *MiningSetup_->MiningPairs) {
		act->commands.append(ExcavatorCommand("worker.add", {algo, QString::number(nvidiaPair->Device->ID)}));
		}
	cfg.append(act->asJObject());

	act=new ExcavatorAction(10, 15);
//	foreach (MiningPair* nvidiaPair, *_MiningSetup->MiningPairs) {
//		act->commands.append(ExcavatorCommand("worker.print.speed", {QString::number(nvidiaPair->Device->ID)}));
//		}
	act->commands.append(ExcavatorCommand("algorithm.print.speeds", {}));
	cfg.append(act->asJObject());

	return QJsonDocument(cfg).toJson(QJsonDocument::Indented);
}

QString Excavator::jsonBenchConfig(QString url, QString btcAdress, QString worker)
{
	ExcavatorAction* act;
	QJsonArray cfg;

	act=new ExcavatorAction();
	act->commands.append(ExcavatorCommand("subscribe", {QString("nhmp.%1.nicehash.com:3200").arg(Globals::MiningLocation[ConfigManager.generalConfig->ServiceLocation]), GetUsername(btcAdress, worker)}));
	cfg.append(act->asJObject());

	act=new ExcavatorAction(2);
	if (!IsDual()) {
		act->commands.append(ExcavatorCommand("algorithm.add", {MiningSetup_->MinerName, "benchmark"}));
		}
	else {
		act->commands.append(ExcavatorCommand("algorithm.add", {"daggerhashimoto", "benchmark"}));
		switch (SecondaryAlgorithmType) {
			case Enums::AlgorithmType::Sia:
				act->commands.append(ExcavatorCommand("algorithm.add", {"sia", "benchmark"}));
				break;
			case Enums::AlgorithmType::Pascal:
				act->commands.append(ExcavatorCommand("algorithm.add", {"pascal", "benchmark"}));
				break;
			case Enums::AlgorithmType::Decred:
				act->commands.append(ExcavatorCommand("algorithm.add", {"decred", "benchmark"}));
				break;
			default:
				break;
			}
		}
	cfg.append(act->asJObject());

	act=new ExcavatorAction(3);
	if (!IsDual()) {
		act->commands.append(ExcavatorCommand("worker.add", {MiningSetup_->MinerName, "0"}));
		}
	else {
		act->commands.append(ExcavatorCommand("worker.add", {QString("daggerhashimoto_%1").arg(QMetaEnum::fromType<Enums::AlgorithmType>().valueToKey((int)SecondaryAlgorithmType)).toLower(), "0"}));
		}
	cfg.append(act->asJObject());

	act=new ExcavatorAction(_benchmarkTimeWait+3+(MiningSetup_->CurrentAlgorithmType==Enums::AlgorithmType::DaggerHashimoto? 5 : 0));
	act->commands.append(ExcavatorCommand("worker.print.speed", {"0"}));
	act->commands.append(ExcavatorCommand("quit", {}));
	cfg.append(act->asJObject());

	return QJsonDocument(cfg).toJson(QJsonDocument::Indented);
}

double Excavator::GetNumber(QString speed)
{
	QStringList splitSrs=speed.trimmed().split(' ');
	if (splitSrs.count()>=2) {
		QString speedStr=splitSrs.at(0);
		QString postfixStr=splitSrs.at(1);
		double spd=speedStr.toDouble();
		if (postfixStr.contains("kH/s")) {
			spd*=1000;
			}
		else if (postfixStr.contains("MH/s")) {
			spd*=1000000;
			}
		else if (postfixStr.contains("GH/s")) {
			spd*=1000000000;
			}
		else if (postfixStr.contains("TH/s")) {
			spd*=1000000000000;
			}

		return spd;
		}
	return 0;
}
