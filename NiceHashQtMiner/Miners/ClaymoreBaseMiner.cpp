#include "Miners/ClaymoreBaseMiner.h"
#include "Utils/Helpers.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Parsing/ExtraLaunchParametersParser.h"
#include "Devices/ComputeDeviceManager.h"
#include "Algorithms/Algorithm.h"
#include "Algorithms/DualAlgorithm.h"
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <cmath>


ClaymoreBaseMiner::ClaymoreBaseMiner(QString minerDeviceName, int maxCDTime=60*1000*5)
	: Miner(minerDeviceName, maxCDTime)
{
	_ConectionType=Enums::NhmConectionType::STRATUM_SSL;
	IsKillAllUsedMinerProcs=true;
}

// return true if a secondary algo is being used
bool ClaymoreBaseMiner::IsDual()
{
	return SecondaryAlgorithmType!=Enums::AlgorithmType::NONE;
}

ApiData* ClaymoreBaseMiner::GetSummaryAsync()
{
	CurrentMinerReadStatus=Enums::MinerApiReadStatus::NONE;
	ApiData* ad=new ApiData(MiningSetup_->CurrentAlgorithmType, MiningSetup_->CurrentSecondaryAlgorithmType);

	JsonApiResponse* resp=nullptr;
	try {
		QTcpSocket client;
		QByteArray bytesToSend="{\"id\":0,\"jsonrpc\":\"2.0\",\"method\":\"miner_getstat1\"}\n";
		client.connectToHost("127.0.0.1", ApiPort());
		client.waitForConnected();
		client.write(bytesToSend);
		client.flush();
		client.waitForReadyRead();
		resp=JsonApiResponse::fromJson(client.readAll());
		client.close();
		}
	catch (QException& ex) {
		Helpers::ConsolePrint(MinerTag(), QString("GetSummary exception: ")+ex.what());
		}

	if (resp!=nullptr && resp->error==nullptr) {
		if (resp->result!=nullptr && resp->result->count()>4) {
			QStringList speeds=resp->result->at(3).split(';');
			QStringList secondarySpeeds= IsDual()? resp->result->at(5).split(';') : QStringList();
			ad->Speed=0;
			ad->SecondarySpeed=0;
			foreach (QString speed, speeds) {
				double tmpSpeed=0;
//				try {
					tmpSpeed=speed.toDouble();
//					}
//				catch (...) {
//					tmpSpeed=0;
//					}
				ad->Speed+=tmpSpeed;
				}
			foreach (QString speed, secondarySpeeds) {
				double tmpSpeed=0;
//				try {
					tmpSpeed=speed.toDouble();
//					}
//				catch (...) {
//					tmpSpeed=0;
//					}
				ad->SecondarySpeed+=tmpSpeed;
				}
			ad->Speed*=ApiReadMult;
			ad->SecondarySpeed*=ApiReadMult;
			CurrentMinerReadStatus=Enums::MinerApiReadStatus::GOT_READ;
			}
		if (!ad->Speed) {
			CurrentMinerReadStatus=Enums::MinerApiReadStatus::READ_SPEED_ZERO;
			}
		// some claymore miners have this issue reporting negative speeds in that case restart miner
		if (ad->Speed<0) {
			Helpers::ConsolePrint(MinerTag(), "Reporting negative speeds will restart...");
			Restart();
			}
		}
	return ad;
}

void ClaymoreBaseMiner::_Stop(Enums::MinerStopType willswitch)
{
	Stop_cpu_ccminer_sgminer_nheqminer(willswitch);
}

QStringList ClaymoreBaseMiner::DeviceCommand(int amdCount/*=1*/)
{
	return {"-di"};
}

QStringList ClaymoreBaseMiner::GetDevicesCommandString()
{
	// First by device type (AMD then NV), then by bus ID index
	QList<MiningPair*>* sortedMinerPairs=new QList<MiningPair*>;
	std::sort(MiningSetup_->MiningPairs->begin(), MiningSetup_->MiningPairs->end(), [](const MiningPair* a, const MiningPair* b){ return a->Device->DeviceType!=b->Device->DeviceType? a->Device->DeviceType>b->Device->DeviceType : a->Device->IDByBus < b->Device->IDByBus;});
	QStringList extraParams=ExtraLaunchParametersParser::ParseForMiningPairs(sortedMinerPairs, Enums::DeviceType::AMD);

	QStringList ids;
	QStringList intensities;

	int amdDeviceCount=ComputeDeviceManager.Query.AmdDevices->count();
	Helpers::ConsolePrint("ClaymoreIndexing", QString("Found %1 AMD devices").arg(amdDeviceCount));

	foreach (MiningPair* mPair, *MiningSetup_->MiningPairs) {
		int id=mPair->Device->IDByBus;
		if (id<0) { // should never happen
			Helpers::ConsolePrint("ClaymoreIndexing", QString("ID by Bus too low: %1 skipping device").arg(id));
			continue;
			}
		if (mPair->Device->DeviceType==Enums::DeviceType::NVIDIA) {
			Helpers::ConsolePrint("ClaymoreIndexing", QString("NVIDIA device increasing index by %1").arg(amdDeviceCount));
			id+=amdDeviceCount;
			}
		if (id>9) { // New >10 GPU support in CD9.8
			if (id<36) { // CD supports 0-9 and a-z indexes, so 36 GPUs
				char idchar=(char)(id+87); // 10=97(a), 11=98(b), etc
				ids.append(QString(idchar));
				}
			else {
				Helpers::ConsolePrint("ClaymoreIndexing", QString("ID %1 too high, ignoring").arg(id));
				}
			}
		else {
			ids.append(QString::number(id));
			}

		DualAlgorithm* algo=qobject_cast<DualAlgorithm*>(mPair->algorithm);
		if (algo!=nullptr && algo->TuningEnabled) {
			intensities << QString::number(algo->CurrentIntensity);
			}
		}
	QStringList deviceStringCommand=DeviceCommand(amdDeviceCount) << ids.join("");
	QStringList intensityStringCommand;
	if (intensities.count()) {
		intensityStringCommand << "-dcri" << intensities.join(',');
		}

	return deviceStringCommand << intensityStringCommand << extraParams;
}

void ClaymoreBaseMiner::BenchmarkThreadRoutine(QStringList commandLine)
{
	DualAlgorithm* dualBenchAlgo=qobject_cast<DualAlgorithm*>(BenchmarkAlgorithm);
	if (dualBenchAlgo!=nullptr && dualBenchAlgo->TuningEnabled) {
		int stepsLeft=(int)ceil((double)(dualBenchAlgo->TuningEnd-dualBenchAlgo->CurrentIntensity)/(dualBenchAlgo->TuningInterval))+1;
		Helpers::ConsolePrint("CDTUNING", QString("%1 tuning steps remain, should complete in %2 seconds").arg(stepsLeft).arg(stepsLeft*BenchmarkTimeWait));
		Helpers::ConsolePrint("CDTUNING", QString("Starting benchmark for intensity %1 out of %2").arg(dualBenchAlgo->CurrentIntensity).arg(dualBenchAlgo->TuningEnd));
		}

	_benchmarkReadCount=0;
	_benchmarkSum=0;
	_secondaryBenchmarkReadCount=0;
	_secondaryBenchmarkSum=0;

	BenchmarkThreadRoutineAlternate(commandLine, BenchmarkTimeWait);
}

void ClaymoreBaseMiner::ProcessBenchLinesAlternate(QStringList lines)
{
	foreach (QString line, lines) {
		if (!line.isEmpty()) {
			BenchLines->append(line);
			QString lineLowered=line.toLower();
			if (lineLowered.contains(LookForStart)) {
				double got=GetNumber(lineLowered);
				if (!IgnoreZero || got>0) {
					_benchmarkSum+=got;
					++_benchmarkReadCount;
					}
				}
			else if (!SecondaryLookForStart.isEmpty() && lineLowered.contains(SecondaryLookForStart)) {
				double got=GetNumber(lineLowered, SecondaryLookForStart, LookForEnd);
				if (IgnoreZero || got>0) {
					_secondaryBenchmarkSum+=got;
					++_secondaryBenchmarkReadCount;
					}
				}
			}
		}

	if (_benchmarkReadCount>0) {
		double speed=_benchmarkSum/_benchmarkReadCount;
		BenchmarkAlgorithm->BenchmarkSpeed(speed);
		DualAlgorithm* dualBenchAlgo=qobject_cast<DualAlgorithm*>(BenchmarkAlgorithm);
		if (dualBenchAlgo!=nullptr) {
			double secondarySpeed=_secondaryBenchmarkSum/std::max(1, _secondaryBenchmarkReadCount);
			if (dualBenchAlgo->TuningEnabled) {
				dualBenchAlgo->SetIntensitySpeedsForCurrent(speed, secondarySpeed);
				}
			else {
				dualBenchAlgo->SecondaryBenchmarkSpeed(secondarySpeed);
				}
			}
		}
}

void ClaymoreBaseMiner::BenchmarkOutputErrorDataReceivedImpl(QString outdata)
{
	CheckOutdata(outdata);
}

bool ClaymoreBaseMiner::BenchmarkParseLine(QString outdata)
{
	Helpers::ConsolePrint("BENCHMARK", outdata);
	return false;
}

double ClaymoreBaseMiner::GetNumber(QString outdata)
{
	return GetNumber(outdata, LookForStart, LookForEnd);
}

double ClaymoreBaseMiner::GetNumber(QString outdata, QString LOOK_FOR_START, QString LOOK_FOR_END)
{
	try {
		double mult=1;
		int speedStart=outdata.indexOf(LOOK_FOR_START);
		QString speed=outdata.mid(speedStart, outdata.length()-speedStart).replace(LOOK_FOR_START, "");
		speed=speed.mid(0, speed.indexOf(LOOK_FOR_END));

		if (speed.contains('k')) {
			mult=1000;
			speed=speed.remove(QChar('k'));
			}
		else if (speed.contains('m')) {
			mult=1000000;
			speed=speed.remove(QChar('m'));
			}
		else if (speed.contains('g')) {
			mult=1000000000;
			speed=speed.remove(QChar('g'));
			}
		speed=speed.trimmed();
		return (speed.toDouble()*mult)*(1.0-DevFee*0.01);
		}
	catch (QException& ex) {
		Helpers::ConsolePrint("GetNumber", QString(ex.what())+" | args => "+outdata+" | "+LOOK_FOR_END+" | "+LOOK_FOR_START);
		}
	return 0;
}

ClaymoreBaseMiner::JsonApiResponse* ClaymoreBaseMiner::JsonApiResponse::fromJson(QByteArray json)
{
	QJsonDocument d=QJsonDocument::fromJson(json);
	if (d.isEmpty()) {
		return nullptr;
		}
	QJsonObject o=d.object();
	JsonApiResponse* ret=new JsonApiResponse;
	if (o.contains("result")) {
		ret->result=new QStringList;
		foreach(QJsonValue s, o.value("result").toArray()) {
			ret->result->append(s.toString());
			}
		}
	ret->id=o.value("id").toInt();
	if (o.contains("error") && !o.value("error").isNull()) {
		ret->error=new QJsonObject(o.value("error").toObject());
		}
	return ret;
}
