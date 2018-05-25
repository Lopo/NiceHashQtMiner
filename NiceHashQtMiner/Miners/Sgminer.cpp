#include "Miners/Sgminer.h"
#include "Devices/ComputeDeviceManager.h"
#include "WinPort/Process.h"
#include "Utils/Helpers.h"
#include "Miners/Parsing/ExtraLaunchParametersParser.h"
#include "Configs/ConfigManager.h"
#include "Globals.h"
#include "Configs/Data/GeneralConfig.h"
#include "Switching/NiceHashSma.h"
#include "Qt/LException.h"
#include "Algorithms/Algorithm.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "International.h"
#include "Switching/NHSmaData.h"
#include <QDateTime>
#include <QtConcurrent/QtConcurrentRun>
#include <csignal>


Sgminer::Sgminer()
	: Miner("sgminer_AMD", 90*1000)
{
	_gpuPlatformNumber=ComputeDeviceManager.Avaliable.AmdOpenCLPlatformNum;
	IsKillAllUsedMinerProcs=true;
}

void Sgminer::KillSgminer()
{
	foreach (pid_t pid, *Process::GetProcessesByName("sgminer")) {
		try {
			::kill(pid, SIGKILL);
			}
		catch (std::exception& e) {
			Helpers::ConsolePrint(MinerDeviceName, e.what());
			}
		}
}

void Sgminer::EndBenchmarkProcces()
{
	if (_BenchmarkProcessStatus!=Enums::BenchmarkProcessStatus::Killing && _BenchmarkProcessStatus!=Enums::BenchmarkProcessStatus::DoneKilling) {
		_BenchmarkProcessStatus=Enums::BenchmarkProcessStatus::Killing;
		try {
			Helpers::ConsolePrint("BENCHMARK", QString("Trying to kill benchmark process %1 algorithm %2").arg(BenchmarkProcessPath).arg(BenchmarkAlgorithm->AlgorithmName()));
			KillSgminer();
			}
		catch (...) {}
//		finally {
			_BenchmarkProcessStatus=Enums::BenchmarkProcessStatus::DoneKilling;
			Helpers::ConsolePrint("BENCHMARK", QString("Benchmark process %1 algorithm %2 KILLED").arg(BenchmarkProcessPath).arg(BenchmarkAlgorithm->AlgorithmName()));
//			}
		}
}

void Sgminer::_Stop(Enums::MinerStopType willswitch)
{
	Stop_cpu_ccminer_sgminer_nheqminer(willswitch);
}

void Sgminer::Start(QString url, QString btcAdress, QString worker)
{
	if (!IsInit()) {
		Helpers::ConsolePrint(MinerTag(), "MiningSetup is not initialized exiting Start()");
		return;
		}
	QString username=GetUsername(btcAdress, worker);

	LastCommandLine=QStringList() << "--gpu-platform" << QString::number(_gpuPlatformNumber)
			<< "-k" << MiningSetup_->MinerName
			<< "--url="+url
			<< "--userpass="+username
			<< "-p" << "x"
			<< "--api-listen"
			<< "--api-port="+QString::number(ApiPort())
			<< ExtraLaunchParametersParser::ParseForMiningSetup(MiningSetup_, Enums::DeviceType::AMD)
			<< "--device" << GetDevicesCommandString();

	ProcessHandle=_Start();
}

QStringList Sgminer::BenchmarkCreateCommandLine(Algorithm* algorithm, int time)
{
	QString url=Globals::GetLocationURL(algorithm->NiceHashID, Globals::MiningLocation[ConfigManager.generalConfig->ServiceLocation], _ConectionType);

	// demo for benchmark
	QString username=Globals::DemoUser;

	if (ConfigManager.generalConfig->WorkerName.length()>0) {
		username+="."+ConfigManager.generalConfig->WorkerName.trimmed();
		}

	// cd to the cgminer for the process bins
//	commandLine+="-c \"cd "+WorkingDirectory()+" && ./sgminer "
	QStringList commandLine=
//		QStringList({"./sgminer"})
			QStringList()
			<< "--gpu-platform" << QString::number(_gpuPlatformNumber)
			<< "-k" << algorithm->MinerName
			<< "--url="+url
			<< "--userpass="+username
			<< "-p" << "x"
//			<< "--api-listen"
//			<< "--api-port="+QString::number(ApiPort())
			<< "--sched-stop" << QDateTime::currentDateTime().addSecs(time).toString("HH:mm")
			<< "-T" << "--log" << "10" << "--log-file" << "dump.txt"
			<< ExtraLaunchParametersParser::ParseForMiningSetup(MiningSetup_, Enums::DeviceType::AMD)
			<< "--device" << GetDevicesCommandString();

//	commandLine+=" && rm dump.txt\"";
//	commandLine << "&&" << "rm" << "dump.txt";
//	commandLine+=" >/tmp/dbg.txt 2>&1\"";

	commandLine.removeAll("");
	return commandLine;
}

bool Sgminer::BenchmarkParseLine(QString outdata)
{
	if (outdata.contains("Average hashrate:") && outdata.contains("/s") && BenchmarkAlgorithm->NiceHashID!=Enums::AlgorithmType::DaggerHashimoto) {
		int i=outdata.indexOf(": ");
		int k=outdata.indexOf("/s");

		// save speed
		QString hashSpeed=outdata.mid(i+2, k-i+2);
		Helpers::ConsolePrint("BENCHMARK", "Final Speed: "+hashSpeed);

		hashSpeed=hashSpeed.mid(0, hashSpeed.indexOf(' '));
		double speed=hashSpeed.toDouble();

		if (outdata.contains("Kilohash")) {
			speed*=1000;
			}
		else if (outdata.contains("Megahash")) {
			speed*=1000000;
			}
		else if (outdata.contains("Gigahash")) {
			speed*=1000000000;
			}

		BenchmarkAlgorithm->BenchmarkSpeed(speed);
		return true;
		}
	if (outdata.contains(QString("GPU%1").arg(MiningSetup_->MiningPairs->at(0)->Device->ID)) && outdata.contains("s):") && BenchmarkAlgorithm->NiceHashID==Enums::AlgorithmType::DaggerHashimoto) {
		int i=outdata.indexOf("s):");
		int k=outdata.indexOf("(avg)");

		// save speed
		QString hashSpeed=outdata.mid(i+3, k-i+3).trimmed().remove("(avg):");
		Helpers::ConsolePrint("BENCHMARK", QString("Final Speed: %1").arg(hashSpeed));

		double mult=1;
		if (hashSpeed.contains('K')) {
			hashSpeed=hashSpeed.replace('K', ' ');
			mult=1000;
			}
		else if (hashSpeed.contains('M')) {
			hashSpeed=hashSpeed.replace('M', ' ');
			mult=1000000;
			}
		else if (hashSpeed.contains('G')) {
			hashSpeed=hashSpeed.replace('G', ' ');
			mult=1000000000;
			}

		hashSpeed=hashSpeed.mid(0, hashSpeed.indexOf(' '));
		double speed=hashSpeed.toDouble()*mult;

		BenchmarkAlgorithm->BenchmarkSpeed(speed);

		return true;
		}
	return false;
}

void Sgminer::BenchmarkThreadRoutineStartSettup()
{
	// sgminer extra settings
	Enums::AlgorithmType nhDataIndex=BenchmarkAlgorithm->NiceHashID;

	if (!NHSmaData::HasData()) {
		Helpers::ConsolePrint("BENCHMARK", "Skipping sgminer benchmark because there is no internet connection. Sgminer needs internet connection to do benchmarking.");
		throw LException("No internet connection");
		}

	double paying;
	NHSmaData::TryGetPaying(nhDataIndex, paying);
	if (!paying) {
		Helpers::ConsolePrint("BENCHMARK", QString("Skipping sgminer benchmark because there is no work on Nicehash.com [algo: %1(%2)]").arg(BenchmarkAlgorithm->AlgorithmName()).arg(nhDataIndex));
		throw LException("No work can be used for benchmarking");
		}

	_benchmarkTimer->restart();
	Miner::BenchmarkThreadRoutineStartSettup();
}

void Sgminer::BenchmarkOutputErrorDataReceivedImpl(QString outdata)
{
	if (_benchmarkTimer->elapsed()/1000>=BenchmarkTimeInSeconds) {
		QString* resp=QtConcurrent::run(this, static_cast<QString*(Sgminer::*)(int, QString, bool, bool)>(&Miner::GetApiDataAsync), ApiPort(), QString("quit"), false, false).result();
		if (resp==nullptr) {
			Helpers::ConsolePrint("BENCHMARK", "null response");
			return;
			}
		while (resp->endsWith('\0')) {
			resp->chop(1);
			}
		Helpers::ConsolePrint("BENCHMARK", "SGMiner Response: "+*resp);
		}
	if (_benchmarkTimer->elapsed()/1000>=BenchmarkTimeInSeconds+2) {
		_benchmarkTimer->invalidate(); // stop
		// this is safe in a benchmark
		KillSgminer();
		BenchmarkSignalHanged=true;
		}
	if (!BenchmarkSignalFinnished && outdata!=nullptr) {
		CheckOutdata(outdata);
		}
}

QString Sgminer::GetFinalBenchmarkString()
{
	if (BenchmarkAlgorithm->BenchmarkSpeed()<=0) {
		Helpers::ConsolePrint("sgminer_GetFinalBenchmarkString", International::GetText("sgminer_precise_try"));
		return International::GetText("sgminer_precise_try");
		}
	return Miner::GetFinalBenchmarkString();
}

void Sgminer::BenchmarkThreadRoutine(QStringList commandLine)
{
	BenchmarkSignalQuit=false;
	BenchmarkSignalHanged=false;
	BenchmarkSignalFinnished=false;
	BenchmarkException=nullptr;

	QThread::msleep(ConfigManager.generalConfig->MinerRestartDelayMS*3); // increase wait for sgminer

	try {
		Helpers::ConsolePrint("BENCHMARK", "Benchmark starts");
		BenchmarkHandle=BenchmarkStartProcess(commandLine);
		BenchmarkThreadRoutineStartSettup();
		_BenchmarkProcessStatus=Enums::BenchmarkProcessStatus::Running;
/*		while (true) {
			QString outdata=BenchmarkHandle->readAll();
			BenchmarkOutputErrorDataReceivedImpl(outdata);
			// terminate process situations
			if (BenchmarkSignalQuit
				|| BenchmarkSignalFinnished
				|| BenchmarkSignalHanged
				|| BenchmarkSignalTimedout
				|| BenchmarkException!=nullptr
				) {
				// this is safe in a benchmark
				KillSgminer();
				if (BenchmarkSignalTimedout) {
					throw LException("Benchmark timedout");
					}
				if (BenchmarkException!=nullptr) {
					throw BenchmarkException;
					}
				if (BenchmarkSignalQuit) {
					throw LException("Termined by user request");
					}
				if (BenchmarkSignalHanged) {
					throw LException("SGMiner is not responding");
					}
				if (BenchmarkSignalFinnished) {
					break;
					}
				}
			else {
				// wait a second reduce CPU load
				QThread::msleep(1000);
				}
			}*/
		bool exited=BenchmarkHandle->waitForFinished((BenchmarkTimeoutInSeconds(BenchmarkTimeInSeconds)+20)*1000);

		if (!exited) {
			KillSgminer();
			}

		if (BenchmarkSignalTimedout) {
			throw LException("Benchmark timedout");
			}
		if (BenchmarkException!=nullptr) {
			throw BenchmarkException;
			}
		if (BenchmarkSignalQuit) {
			throw LException("Termined by user request");
			}
		if (BenchmarkSignalHanged || !exited) {
			throw LException("SGMiner is not responding");
			}
		if (BenchmarkSignalFinnished) {
			}
		}
	catch (LException& ex) {
		BenchmarkThreadRoutineCatch(ex);
		}
	catch (std::exception& ex) {
		BenchmarkThreadRoutineCatch(ex);
		}
//	finally {
		BenchmarkThreadRoutineFinish();
//		}
}

ApiData* Sgminer::GetSummaryAsync()
{
	ApiData* ad=new ApiData(MiningSetup_->CurrentAlgorithmType);

	QString* resp=QtConcurrent::run(this, static_cast<QString*(Sgminer::*)(int, QString, bool, bool)>(&Miner::GetApiDataAsync), ApiPort(), QString("summary"), false, false).result();
	if (resp==nullptr) {
		CurrentMinerReadStatus=Enums::MinerApiReadStatus::NONE;
		return nullptr;
		}

	try {
		// Checks if all the GPUs are Alive first
		QString* resp2=QtConcurrent::run(this, static_cast<QString*(Sgminer::*)(int, QString, bool, bool)>(&Miner::GetApiDataAsync), ApiPort(), QString("devs"), false, false).result();
		if (resp2==nullptr) {
			CurrentMinerReadStatus=Enums::MinerApiReadStatus::NONE;
			return nullptr;
			}

		QStringList checkGpuStatus=resp2->split('|', QString::SkipEmptyParts);

		for (int i=1; i<checkGpuStatus.count()-1; i++) {
			if (checkGpuStatus[i].contains("Enabled=Y") && !checkGpuStatus[i].contains("Status=Alive")) {
				Helpers::ConsolePrint(MinerTag(), QString(ProcessTag()+" GPU %1: Sick/Dead/NoStart/Initialising/Disabled/Rejecting/Unknown").arg(i));
				CurrentMinerReadStatus=Enums::MinerApiReadStatus::WAIT;
				return nullptr;
				}
			}

		QStringList resps=resp->split('|', QString::SkipEmptyParts);

		if (resps[1].contains("SUMMARY")) {
			QStringList data=resps[1].split(',', QString::SkipEmptyParts);

			// Get miner's current total speed
			QStringList speed=data[4].split('=', QString::SkipEmptyParts);
			// Get miner's current total MH
			double totalMH=data[18].split('=', QString::SkipEmptyParts)[1].toDouble();
//			double totalMH=Double.Parse(data[18].Split(new char[] {
//				'=' }, StringSplitOptions.RemoveEmptyEntries)[1], new CultureInfo("en-US"));

			ad->Speed=speed[1].toDouble()*1000;

			if (totalMH<=PreviousTotalMH) {
				Helpers::ConsolePrint(MinerTag(), ProcessTag()+" SGMiner might be stuck as no new hashes are being produced");
				Helpers::ConsolePrint(MinerTag(), ProcessTag()+QString(" Prev Total MH: %1 .. Current Total MH: %2").arg(PreviousTotalMH).arg(totalMH));
				CurrentMinerReadStatus=Enums::MinerApiReadStatus::NONE;
				return nullptr;
				}

			PreviousTotalMH=totalMH;
			}
		else {
			ad->Speed=0;
			}
		}
	catch (...) {
		CurrentMinerReadStatus=Enums::MinerApiReadStatus::NONE;
		return nullptr;
		}

	CurrentMinerReadStatus=Enums::MinerApiReadStatus::GOT_READ;
	// check if speed zero
	if (!ad->Speed) {
		CurrentMinerReadStatus=Enums::MinerApiReadStatus::READ_SPEED_ZERO;
		}

	return ad;
}
/*
QProcess* Sgminer::BenchmarkStartProcess(QStringList CommandLine)
{
	FILE* handle=popen("./"+MinerExeName());
}
*/