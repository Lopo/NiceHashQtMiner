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
			else {
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
		foreach (Worker w, resp->algorithms.first().workers) {
			ad->Speed+=w.speed.first();
			}
		if (IsDual()) {
			foreach (Worker w, resp->algorithms.first().workers) {
				ad->SecondarySpeed+=w.speed.at(1);
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
		foreach (QJsonValue wv, ao.value("workers").toArray()) {
			QJsonObject wo=wv.toObject();
			Worker w;
			w.worker_id=wo.value("worker_id").toInt();
			foreach (QJsonValue sv, wo.value("speed").toArray()) {
				w.speed.append(sv.toDouble());
				}
			a.workers.append(w);
			}
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

	QString algo=MiningSetup_->MinerName;
	if (IsDual()) {
		algo+=QString("_%1").arg(QMetaEnum::fromType<Enums::AlgorithmType>().valueToKey((int)SecondaryAlgorithmType));
		}
	QString wallet=GetUsername(btcAdress, worker);
	QStringList base({algo, url, wallet});
	if (IsDual()) {
		base << Globals::GetLocationURL(SecondaryAlgorithmType, Globals::MiningLocation[ConfigManager.generalConfig->ServiceLocation], _ConectionType) << wallet;
		}
	act=new ExcavatorAction();
	act->commands.append(ExcavatorCommand("algorithm.add", base));
	cfg.append(act->asJObject());

	act=new ExcavatorAction(1);
	foreach (MiningPair* nvidiaPair, *MiningSetup_->MiningPairs) {
		act->commands.append(ExcavatorCommand("worker.add", {"0", QString::number(nvidiaPair->Device->ID)}));
		}
	cfg.append(act->asJObject());

	act=new ExcavatorAction(10, 15);
//	foreach (MiningPair* nvidiaPair, *_MiningSetup->MiningPairs) {
//		act->commands.append(ExcavatorCommand("worker.print.speed", {QString::number(nvidiaPair->Device->ID)}));
//		}
	act->commands.append(ExcavatorCommand("algorithm.print.speeds", {"0"}));
	cfg.append(act->asJObject());

	return QJsonDocument(cfg).toJson(QJsonDocument::Indented);
}

QString Excavator::jsonBenchConfig(QString url, QString btcAdress, QString worker)
{
	ExcavatorAction* act;
	QJsonArray cfg;

	QString algo= !IsDual()
		? MiningSetup_->MinerName
		: QString("daggerhashimoto_%1").arg(QMetaEnum::fromType<Enums::AlgorithmType>().valueToKey((int)SecondaryAlgorithmType)).toLower();
	QString wallet=GetUsername(btcAdress, worker);

	QStringList base({algo, url, wallet});
	if (IsDual()) {
		base << Globals::GetLocationURL(SecondaryAlgorithmType, Globals::MiningLocation[ConfigManager.generalConfig->ServiceLocation], _ConectionType) << wallet;
		}
	act=new ExcavatorAction();
	act->commands.append(ExcavatorCommand("algorithm.add", base));
	cfg.append(act->asJObject());

	act=new ExcavatorAction(3);
	foreach (MiningPair* nvidiaPair, *MiningSetup_->MiningPairs) {
		if (IsDual()) {
			act->commands.append(ExcavatorCommand("worker.add", {"0", QString::number(nvidiaPair->Device->ID)}));
			}
		else {
			act->commands.append(ExcavatorCommand("worker.add", {"0", QString::number(nvidiaPair->Device->ID), "M=1"}));
			}
		}
	cfg.append(act->asJObject());

	act=new ExcavatorAction(_benchmarkTimeWait+3+(MiningSetup_->CurrentAlgorithmType==Enums::AlgorithmType::DaggerHashimoto? 5 : 0));
	act->commands.append(ExcavatorCommand("algorithm.print.speeds", {"0"}));
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
