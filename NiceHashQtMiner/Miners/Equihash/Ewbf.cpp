#include "Miners/Equihash/Ewbf.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Miners/Parsing/ExtraLaunchParametersParser.h"
#include "WinPort/Process.h"
#include "Utils/Helpers.h"
#include "Globals.h"
#include "Algorithms/Algorithm.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Qt/LException.h"
#include <QDir>
#include <QTextStream>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonArray>
#include <csignal>


Ewbf::Ewbf(QString name/*="ewbf"*/)
	: Miner(name, 60*1000*5)
{
	_ConectionType=Enums::NhmConectionType::NONE;
//	IsNeverHideMiningWindow=true;
}

void Ewbf::Start(QString url, QString btcAdress, QString worker)
{
	LastCommandLine=GetStartCommand(url, btcAdress, worker);
	ProcessHandle=_Start();
}

QStringList Ewbf::GetStartCommand(QString url, QString btcAddress, QString worker)
{
	QStringList ret=GetDevicesCommandString()
			<< "--server" << url.split(':')[0]
			<< "--user" << btcAddress+"."+worker << "--pass" << "x" << "--port"
			<< url.split(':')[1] << "--api" << "127.0.0.1:"+QString::number(ApiPort());
	if (!ret.contains("--fee")) {
		ret << "--fee" << "0";
		}
	return ret;
}

QStringList Ewbf::GetDevicesCommandString()
{
	QStringList deviceStringCommand={"--cuda_devices"};
	foreach (MiningPair* nvidiaPair, *this->MiningSetup_->MiningPairs) {
		deviceStringCommand << QString::number(nvidiaPair->Device->ID);
		}

	deviceStringCommand << ExtraLaunchParametersParser::ParseForMiningSetup(MiningSetup_, Enums::DeviceType::NVIDIA);

	return deviceStringCommand;
}

void Ewbf::KillMinerBase(QString exeName)
{
	foreach (pid_t pid, *Process::GetProcessesByName(exeName)) {
		try {
			::kill(pid, SIGKILL);
			}
		catch (std::exception& e) {
			Helpers::ConsolePrint(MinerDeviceName, e.what());
			}
		}
}

QStringList Ewbf::BenchmarkCreateCommandLine(Algorithm* algorithm, int time)
{
	CleanOldLogs();

	QString server=Globals::GetLocationURL(algorithm->NiceHashID, Globals::MiningLocation[ConfigManager.generalConfig->ServiceLocation], _ConectionType);
	QStringList ret=QStringList() << "--log" << "2" << "--logfile" << GetLogFileName() << GetStartCommand(server, Globals::GetBitcoinUser(), ConfigManager.generalConfig->WorkerName.trimmed());
	_benchmarkTimeWait=std::max(time*3, 90); // EWBF takes a long time to get started
	return ret;
}

void Ewbf::BenchmarkThreadRoutine(QStringList commandLine)
{
	BenchmarkSignalQuit=false;
	BenchmarkSignalHanged=false;
	BenchmarkSignalFinnished=false;
	BenchmarkException=nullptr;

	QThread::msleep(ConfigManager.generalConfig->MinerRestartDelayMS);

	try {
		Helpers::ConsolePrint("BENCHMARK", "Benchmark starts");
		Helpers::ConsolePrint(MinerTag(), "Benchmark should end in : "+QString::number(_benchmarkTimeWait)+" seconds");
		BenchmarkHandle=BenchmarkStartProcess(commandLine);
		BenchmarkHandle->waitForFinished(_benchmarkTimeWait+2);
		QElapsedTimer* _benchmarkTimer=new QElapsedTimer;
		_benchmarkTimer->start();
		_BenchmarkProcessStatus=Enums::BenchmarkProcessStatus::Running;
		bool keepRunning=true;
		while (keepRunning && IsActiveProcess(BenchmarkHandle->processId())) {
			if (_benchmarkTimer->elapsed()/1000>=(_benchmarkTimeWait+2)
				|| BenchmarkSignalQuit
				|| BenchmarkSignalFinnished
				|| BenchmarkSignalHanged
				|| BenchmarkSignalTimedout
				|| BenchmarkException!=nullptr
				) {
				QString imageName=MinerExeName();
				// maybe will have to KILL process
				KillMinerBase(imageName);
				if (BenchmarkSignalTimedout) {
					throw LException("Benchmark timedout");
					}
				if (BenchmarkException!=nullptr) {
					throw *BenchmarkException;
					}
				if (BenchmarkSignalQuit) {
					throw LException("Termined by user request");
					}
				if (BenchmarkSignalFinnished) {
					break;
					}
				keepRunning=false;
				break;
				}
			// wait a second reduce CPU load
			QThread::msleep(1000);
			}
		}
	catch (LException& ex) {
		BenchmarkThreadRoutineCatch(ex);
		}
	catch (std::exception& ex) {
		BenchmarkThreadRoutineCatch(ex);
		}
//	finally {
		BenchmarkAlgorithm->BenchmarkSpeed(0);
		// find latest log file
		QString latestLogFile="";
		QDir dirInfo(WorkingDirectory());
		foreach (QString file, dirInfo.entryList({GetLogFileName()})) {
			latestLogFile=file;
			break;
			}
		// read file log
		QFile logFile(WorkingDirectory()+latestLogFile);
		if (logFile.exists()) {
			QStringList lines;
			bool read=false;
			uint8_t iteration=0;
			logFile.open(QIODevice::ReadOnly);
			QTextStream logStream(&logFile);
			while (!read) {
				if (iteration<10) {
					try {
						while (true) {
							QString line=logStream.readLine();
							if (line.isNull()) {
								break;
								}
							lines.append(line);
							}
						read=true;
						Helpers::ConsolePrint(MinerTag(), "Successfully read log after "+QString::number(iteration)+" iterations");
						}
					catch (std::exception& ex) {
						Helpers::ConsolePrint(MinerTag(), ex.what());
						QThread::msleep(1000);
						}
					iteration++;
					}
				else {
					read=true; // Give up after 10s
					Helpers::ConsolePrint(MinerTag(), "Gave up on iteration "+QString::number(iteration));
					}
				}
			logFile.close();

//			bool addBenchLines=!bench_lines->count();
			foreach (QString line, lines) {
				if (!line.isEmpty()) {
					BenchLines->append(line);
					QString lineLowered=line.toLower();
					if (lineLowered.contains(LookForStart)) {
						_benchmarkSum+=GetNumber(lineLowered);
						++_benchmarkReadCount;
						}
					}
				}
			if (_benchmarkReadCount>0) {
				BenchmarkAlgorithm->BenchmarkSpeed(_benchmarkSum/_benchmarkReadCount);
				}
			}
		BenchmarkThreadRoutineFinish();
//		}
}

void Ewbf::BenchmarkOutputErrorDataReceivedImpl(QString outdata)
{
	CheckOutdata(outdata);
}

bool Ewbf::BenchmarkParseLine(QString outdata)
{
	Helpers::ConsolePrint("BENCHMARK", outdata);
	return false;
}

double Ewbf::GetNumber(QString outdata)
{
	return  GetNumber(outdata, LookForStart, LookForEnd);
}

double Ewbf::GetNumber(QString outdata, QString lookForStart, QString lookForEnd)
{
	try {
		double mult=1;
		int speedStart=outdata.indexOf(lookForStart);
		QString speed=outdata.mid(speedStart, outdata.length()-speedStart)
				.replace(lookForStart, "");
		speed=speed.mid(0, speed.indexOf(lookForEnd));

		if (speed.contains("k")) {
			mult=1000;
			speed=speed.replace("k", "");
			}
		else if (speed.contains("m")) {
			mult=1000000;
			speed=speed.replace("m", "");
			}
		speed=speed.trimmed();
		return (speed.toDouble()*mult);
		}
	catch (std::exception& ex) {
		Helpers::ConsolePrint("GetNumber", QString(ex.what())+" | args => "+outdata+" | "+lookForEnd+" | "+lookForStart);
		}
	return 0;
}

ApiData* Ewbf::GetSummaryAsync()
{
	CurrentMinerReadStatus=Enums::MinerApiReadStatus::NONE;
	ApiData* ad=new ApiData(MiningSetup_->CurrentAlgorithmType);
	JsonApiResponse* resp=nullptr;
	QTcpSocket client(this);
	try {
		client.connectToHost("127.0.0.1", ApiPort());
		client.waitForConnected();
		client.write("{\"method\":\"getstat\"}\n");
		client.flush();
		client.waitForReadyRead();
		resp=JsonApiResponse::fromJson(client.readAll());
		client.close();
		}
	catch (std::exception& ex) {
		Helpers::ConsolePrint(MinerTag(), ex.what());
		}

	if (resp!=nullptr && resp->error==nullptr) {
		ad->Speed=0;
		foreach (Result t1, resp->result) {
			ad->Speed+=t1.speed_sps;
			}
		CurrentMinerReadStatus=Enums::MinerApiReadStatus::GOT_READ;
		if (!ad->Speed) {
			CurrentMinerReadStatus=Enums::MinerApiReadStatus::READ_SPEED_ZERO;
			}
		}

	return ad;
}

void Ewbf::_Stop(Enums::MinerStopType willswitch)
{
	Stop_cpu_ccminer_sgminer_nheqminer(willswitch);
}

Ewbf::JsonApiResponse* Ewbf::JsonApiResponse::fromJson(QString json)
{
	if (!json.trimmed().length()) {
		return nullptr;
		}
	try {
		QJsonObject o=QJsonDocument::fromJson(json.toLatin1()).object();
		JsonApiResponse* ret=new JsonApiResponse;
		ret->id=o.value("id").toInt();
		ret->method=o.value("method").toString();
		if (!o.value("error").isNull()) {
			ret->error=new QJsonObject(o.value("error").toObject());
			}
		foreach (QJsonValue v, o.value("result").toArray()) {
			Result r;
			QJsonObject vo=v.toObject();
			r.gpuid=vo.value("gpuid").toInt();
			r.cudaid=vo.value("cudaid").toInt();
			r.busid=vo.value("busid").toString();
//			r.name=vo.value("name").toString();
			r.gpu_status=vo.value("gpu_status").toInt();
			r.solver=vo.value("solver").toInt();
			r.temperature=vo.value("temperature").toInt();
			r.gpu_power_usage=vo.value("gpu_power_usage").toInt();
			r.speed_sps=vo.value("speed_sps").toInt();
			r.accepted_shares=vo.value("accepted_shares").toInt();
			r.rejected_shares=vo.value("rejected_shares").toInt();
//			r.start_time=vo.value("start_time").toInt();
			ret->result.append(r);
			}
		return ret;
		}
	catch (...) {
		return nullptr;
		}
}
