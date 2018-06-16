#include "Miners/Miner.h"
#include "Utils/AlgorithmNiceHashNames.h"
#include "Grouping/MiningSetup.h"
#include "Miners/Grouping/MiningPair.h"
#include "Configs/ConfigManager.h"
#include "Miners/MinersSettingsManager.h"
#include "Miners/MinersApiPortsManager.h"
#if WITH_AMD
#	include "Miners/Sgminer.h"
#endif
#include "Utils/Logger.h"
#include "Utils/Helpers.h"
#include "Configs/Data/GeneralConfig.h"
#include "International.h"
#include "Globals.h"
#include "Qt/LException.h"
#include "Interfaces/IBenchmarkComunicator.h"
#include "PInvoke/NiceHashProcess.h"
#include "Algorithms/Algorithm.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Configs/Data/MinerSystemVariablesConfig.h"
#include "WinPort/Process.h"
#include "Algorithms/DualAlgorithm.h"
#include <QDir>
#include <QtConcurrent/QtConcurrentRun>
#include <QApplication>
#include <QTcpSocket>
#include <boost/thread.hpp>
#include <exception>
#include <csignal>


ApiData::ApiData(Enums::AlgorithmType algorithmID, Enums::AlgorithmType secondaryAlgorithmID/*=Enums::AlgorithmType::NONE*/)
{
	AlgorithmID=algorithmID;
	SecondaryAlgorithmID=secondaryAlgorithmID;
	AlgorithmName=AlgorithmNiceHashNames::GetName(Helpers::DualAlgoFromAlgos(algorithmID, secondaryAlgorithmID));
	Speed=0.0;
	SecondarySpeed=0.0;
	PowerUsage=0.0;
}

long Miner::_MinerIDCount=0;

Miner::Miner(QString minerDeviceName, int maxCDTime)
{
	_ConectionType=Enums::NhmConectionType::STRATUM_TCP;
	MiningSetup_=new MiningSetup(nullptr);
	_IsInit=false;
	MinerID=_MinerIDCount++;

	MinerDeviceName=minerDeviceName;

	_WorkingDirectory="";

	_IsRunning=false;
	PreviousTotalMH=0.0;

	LastCommandLine.clear();

	_IsApiReadException=false;
	// Only set minimize if hide is false (specific miners will override true after)
//	setIsNeverHideMiningWindow(ConfigManager.generalConfig->MinimizeMiningWindows && !ConfigManager.generalConfig->HideMiningWindows);
	IsKillAllUsedMinerProcs=false;
	_maxCooldownTimeInMilliseconds=maxCDTime;
	//
	Helpers::ConsolePrint(MinerTag(), "NEW MINER CREATED");
}

Miner::~Miner()
{
	// free the port
	MinersApiPortsManager::RemovePort(_ApiPort);
	Helpers::ConsolePrint(MinerTag(), "MINER DESTROYED");
}

void Miner::SetWorkingDirAndProgName(QString fullPath)
{
	_WorkingDirectory="";
	_Path=fullPath;
	int lastIndex=fullPath.lastIndexOf('/')+1;
	if (lastIndex>0) {
		_WorkingDirectory=fullPath.left(lastIndex);
		_MinerExeName=fullPath.mid(lastIndex);
		}
}

void Miner::SetApiPort()
{
	if (_IsInit) {
		QList<int>* reservedPorts=MinersSettingsManager::GetPortsListFor(MiningSetup_->MiningPairs->at(0)->algorithm->MinerBaseType, MiningSetup_->MinerPath, MiningSetup_->MiningPairs->at(0)->algorithm->NiceHashID);
		_ApiPort=-1; // not set
		foreach (int reservedPort, *reservedPorts) {
			if (MinersApiPortsManager::IsPortAvailable(reservedPort)) {
				_ApiPort=reservedPort;
				break;
				}
			}
		if (_ApiPort==-1) {
			_ApiPort=MinersApiPortsManager::GetAvailablePort();
			}
		}
}

void Miner::InitMiningSetup(MiningSetup* miningSetup)
{
	MiningSetup_=miningSetup;
	_IsInit=MiningSetup_->IsInit;
	SetApiPort();
	SetWorkingDirAndProgName(MiningSetup_->MinerPath);
}

void Miner::InitBenchmarkSetup(MiningPair* benchmarkPair)
{
	InitMiningSetup(new MiningSetup(new QList<MiningPair*> {benchmarkPair}));
	BenchmarkAlgorithm=benchmarkPair->algorithm;
}

QString Miner::MinerTag()
{
	if (_minerTag==nullptr) {
		const QString mask="%1-MINER_ID(%2)-DEVICE_IDs(%3)";
		// no devices set
		if (!_IsInit) {
			return mask.arg(MinerDeviceName).arg(MinerID).arg("NOT_SET");
			}
		// contains ids
		QStringList ids;
		foreach (MiningPair* cdevs, *MiningSetup_->MiningPairs) {
			ids.append(QString::number(cdevs->Device->ID));
			}
		_minerTag=new QString(mask.arg(MinerDeviceName).arg(MinerID).arg(ids.join(",")));
		}
	return *_minerTag;
}

QString Miner::ProcessTag(const MinerPidData pidData)
{
	return QString("[pid(%1)|bin(%2)]").arg(pidData.Pid).arg(pidData.MinerBinPath);
}

QString Miner::ProcessTag()
{
	if (_currentPidData==nullptr) {
		return "PidData is NULL";
		}
	return ProcessTag(*_currentPidData);
}

void Miner::KillAllUsedMinerProcesses()
{
	QList<MinerPidData*> toRemovePidData;
	Helpers::ConsolePrint(MinerTag(), "Trying to kill all miner processes for this instance:");
	foreach (MinerPidData* pidData, *_allPidData) {
		try {
			Helpers::ConsolePrint(MinerTag(), QString("Trying to kill %1").arg(ProcessTag(*pidData)));
			if (!Process::CheckProcessById(pidData->Pid)) {
				throw std::exception();
				}
			try {
				kill(pidData->Pid, SIGTERM);
				}
			catch (std::exception& e) {
				Helpers::ConsolePrint(MinerTag(), QString("Exception killing %1, exMsg %2").arg(ProcessTag(*pidData)).arg(e.what()));
				}
			}
		catch (std::exception& e) {
			toRemovePidData.append(pidData);
			Helpers::ConsolePrint(MinerTag(), QString("Nothing to kill %1, exMsg %2").arg(ProcessTag(*pidData)).arg(e.what()));
			}
		}
	foreach (MinerPidData* x, *_allPidData) {
		if (toRemovePidData.contains(x)) {
			_allPidData->removeAll(x);
			}
		}
}

QString Miner::GetUsername(QString btcAdress, QString worker)
{
	if (worker.length()>0) {
		return btcAdress+"."+worker;
		}
	return btcAdress;
}

void Miner::Stop(Enums::MinerStopType willswitch/*=Enums::MinerStopType::SWITCH*/)
{
	if (_cooldownCheckTimer!=nullptr) {
		_cooldownCheckTimer->stop();
		}
	_Stop(willswitch);
	PreviousTotalMH=0.0;
	_IsRunning=false;
}

void Miner::End()
{
	_isEnded=true;
	Stop(Enums::MinerStopType::FORCE_END);
}

void Miner::Stop_cpu_ccminer_sgminer_nheqminer(Enums::MinerStopType willswitch)
{
	if (_IsRunning) {
		Helpers::ConsolePrint(MinerTag(), ProcessTag()+" Shutting down miner");
		}
	if (ProcessHandle!=nullptr) {
		try {
			ProcessHandle->Kill();
			}
		catch (...) {}
		ProcessHandle->Close();
		delete ProcessHandle;
		ProcessHandle=nullptr;

		// sgminer needs to be removed and kill by PID
		if (IsKillAllUsedMinerProcs) {
			KillAllUsedMinerProcesses();
			}
		}
}
/*
void Miner::KillProspectorClaymoreMinerBase(QString exeName) // @todo implement
{
//	foreach (QProcess process, ) {
//		}
}
*/
void Miner::KillProspectorClaymoreMinerBase(qint64 pid) // @todo implement
{
	::kill(pid, SIGTERM);
}

QStringList Miner::GetDevicesCommandString()
{
	QStringList ids;

	foreach (MiningPair* mPair, *MiningSetup_->MiningPairs) {
		ids.append(QString::number(mPair->Device->ID));
		}
	return QStringList() << ids.join(',');
}

int Miner::BenchmarkTimeoutInSeconds(int timeInSeconds)
{
	if (TimeoutStandard) {
		return timeInSeconds;
		}
	if (BenchmarkAlgorithm->NiceHashID==Enums::AlgorithmType::DaggerHashimoto) {
		return 5*60+120; // 5 minutes +2 minutes
		}
	if (BenchmarkAlgorithm->NiceHashID==Enums::AlgorithmType::CryptoNight || BenchmarkAlgorithm->NiceHashID==Enums::AlgorithmType::CryptoNightV7) {
		return 5*60+120; // 5 minutes +2 minutes
		}
	return timeInSeconds+120; // wait time +2 minutes
}

void Miner::BenchmarkStart(int time, IBenchmarkComunicator* benchmarkComunicator)
{
	BenchmarkCommunicator=benchmarkComunicator;
	BenchmarkTimeInSeconds=time;
	BenchmarkSignalFinnished=true;
	// check and kill
	BenchmarkHandle=nullptr;
	OnBenchmarkCompleteCalled=false;
	_benchmarkTimeOutStopWatch=nullptr;

	try {
		if (!QDir("logs").exists()) {
			QDir().mkdir("logs");
			}
		}
	catch (...) {}
	if (BenchLines!=nullptr) {delete BenchLines;}
	BenchLines=new QStringList;
	_benchmarkLogPath=QString("%1Log_%2_%3").arg(Logger.LogPath).arg(MiningSetup_->MiningPairs->at(0)->Device->Uuid()).arg(MiningSetup_->MiningPairs->at(0)->algorithm->AlgorithmStringID());

	QStringList commandLine=BenchmarkCreateCommandLine(BenchmarkAlgorithm, time);

	if (QThread::currentThread()!=&_thread) {
		moveToThread(&_thread);
		_thread.start();
		}
//	QMetaObject::invokeMethod(this, "BenchmarkThreadRoutine", Qt::QueuedConnection, Q_ARG(QString, commandLine));
	boost::thread(&Miner::BenchmarkThreadRoutine, this, commandLine);
}

QProcess* Miner::BenchmarkStartProcess(QStringList commandLine)
{
	Helpers::ConsolePrint(MinerTag(), "Starting benchmark: "+commandLine.join(' '));

	QProcess* benchmarkHandle=new QProcess(this);
//	benchmarkHandle->setProgram(_MiningSetup->MinerPath);
	benchmarkHandle->setProgram("./"+_MinerExeName);

	// sgminer quickfix
/*	if (qobject_cast<Sgminer*>(this)) {
		BenchmarkProcessPath="/bin/sh "+BenchmarkHandle->program()+"";
		}
	else {*/
		BenchmarkProcessPath=benchmarkHandle->program();
		Helpers::ConsolePrint(MinerTag(), QString("Using miner: %1 (%2)").arg(benchmarkHandle->program()).arg(_WorkingDirectory));
		benchmarkHandle->setWorkingDirectory(_WorkingDirectory);
//		}
	// set sys variables
	if (MinersSettingsManager::MinerSystemVariables->contains(_Path)) {
		QProcessEnvironment env=QProcessEnvironment::systemEnvironment();
		foreach (QString envName, MinersSettingsManager::MinerSystemVariables->value(_Path)->keys()) {
			QString envValue=MinersSettingsManager::MinerSystemVariables->value(_Path)->value(envName);
			env.insert(envName, envValue);
			}
		benchmarkHandle->setProcessEnvironment(env);
		}

	commandLine.removeAll("");
	benchmarkHandle->setArguments(commandLine);
	connect(benchmarkHandle, SIGNAL(readyReadStandardError()), this, SLOT(BenchmarkOutputErrorDataReceived()));
	connect(benchmarkHandle, SIGNAL(readyReadStandardOutput()), this, SLOT(BenchmarkOutputErrorDataReceived()));
	connect(benchmarkHandle, SIGNAL(readyRead()), this, SLOT(BenchmarkOutputErrorDataReceived()));
	connect(benchmarkHandle, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(BenchmarkHandle_Exited(int, QProcess::ExitStatus)));
#if WITH_DEBUG
	connect(benchmarkHandle, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(BenchmarkErrorOccurred(QProcess::ProcessError)));
	connect(benchmarkHandle, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(BenchmarkStateChanged(QProcess::ProcessState)));
	connect(benchmarkHandle, SIGNAL(started()), this, SLOT(BenchmarkStarted()));
#endif
	benchmarkHandle->start(QIODevice::Text|QIODevice::ReadOnly);
	benchmarkHandle->waitForStarted(1000);
	if (benchmarkHandle->state()!=QProcess::ProcessState::Running) {
		delete benchmarkHandle;
		benchmarkHandle=nullptr;
		return nullptr;
		}

	_currentPidData=new MinerPidData;
	_currentPidData->MinerBinPath=benchmarkHandle->program();
	_currentPidData->Pid=benchmarkHandle->pid();
	_allPidData->append(_currentPidData);

	return benchmarkHandle;
}

void Miner::BenchmarkHandle_Exited(int exitCode, QProcess::ExitStatus exitStatus)
{
	BenchmarkSignalFinnished=true;
}

void Miner::BenchmarkOutputErrorDataReceived(/*QString data*/)
{
	if (_benchmarkTimeOutStopWatch==nullptr) {
		_benchmarkTimeOutStopWatch=new QElapsedTimer;
		_benchmarkTimeOutStopWatch->start();
		}
	else if ((_benchmarkTimeOutStopWatch->elapsed()/1000)>BenchmarkTimeoutInSeconds(BenchmarkTimeInSeconds)) {
		_benchmarkTimeOutStopWatch->invalidate();
		BenchmarkSignalTimedout=true;
		}

	QString outdata;
	if (BenchmarkHandle!=nullptr) {
		outdata=BenchmarkHandle->readAll();
		}
	if (!outdata.isEmpty()) {
		BenchmarkOutputErrorDataReceivedImpl(outdata);
		}
	// terminate process situations
	if (BenchmarkSignalQuit
		|| BenchmarkSignalFinnished
		|| BenchmarkSignalHanged
		|| BenchmarkSignalTimedout
		|| BenchmarkException!=nullptr
		) {
		FinishUpBenchmark();
		EndBenchmarkProcces();
		}
}

void Miner::FinishUpBenchmark()
{ }

void Miner::CheckOutdata(QString outdata)
{
	BenchLines->append(outdata.split('\n'));
	// ccminer, cpuminer
	if (outdata.contains("Cuda error")) {
		BenchmarkException=new LException("CUDA error");
		}
	if (outdata.contains("is not supported")) {
		BenchmarkException=new LException("N/A");
		}
	if (outdata.contains("illegal memory access")) {
		BenchmarkException=new LException("CUDA error");
		}
	if (outdata.contains("unknown error")) {
		BenchmarkException=new LException("Unknown error");
		}
	if (outdata.contains("No servers could be used! Exiting.")) {
		BenchmarkException=new LException("No pools or work can be used for benchmarking");
		}
	// Ethminer
	if (outdata.contains("No GPU device with sufficient memory was found")) {
		BenchmarkException=new LException("[daggerhashimoto] No GPU device with sufficient memory was found.");
		}
	// xmr-stak
	if (outdata.contains("Press any key to exit")) {
		BenchmarkException=new LException("Xmr-Stak erred, check its logs");
		}

	// lastly parse data
	if (BenchmarkParseLine(outdata)) {
		BenchmarkSignalFinnished=true;
		}
}

void Miner::InvokeBenchmarkSignalQuit()
{
	KillAllUsedMinerProcesses();
}

double Miner::BenchmarkParseLine_cpu_ccminer_extra(QString outdata)
{
	//parse line
	if (outdata.contains("Benchmark: ") && outdata.contains("/s")) {
		int i=outdata.indexOf("Benchmark:");
		int k=outdata.indexOf("/s");
		QString hashspeed=outdata.mid(i+11, k-i-9);
		Helpers::ConsolePrint("BENCHMARK", "Final Speed: "+hashspeed);

		// save speed
		int b=hashspeed.indexOf(' ');
		if (b<0) {
			for (int j=hashspeed.length()-1; j>=0; --j) {
				if (hashspeed[j].digitValue()==-1) {
					continue;
					}
				b=j;
				break;
				}
			}
		if (b>=0) {
			QString speedStr=hashspeed.mid(0, b);
			double spd=Helpers::ParseDouble(speedStr);
			if (hashspeed.contains("kH/s")) {
				spd*=1000;
				}
			else if (hashspeed.contains("MH/s")) {
				spd*=1000000;
				}
			else if (hashspeed.contains("GH/s")) {
				spd*=1000000000;
				}
			else if (hashspeed.contains("TH/s")) {
				spd*=1000000000000;
				}
			return spd;
			}
		}
	return 0.0d;
}

void Miner::EndBenchmarkProcces()
{
	if (BenchmarkHandle!=nullptr && _BenchmarkProcessStatus!=Enums::BenchmarkProcessStatus::Killing && _BenchmarkProcessStatus!=Enums::BenchmarkProcessStatus::DoneKilling) {
		_BenchmarkProcessStatus=Enums::BenchmarkProcessStatus::Killing;
		try {
			Helpers::ConsolePrint("BENCHMARK", QString("Trying to kill benchmark process %1 algorithm %2").arg(BenchmarkProcessPath).arg(BenchmarkAlgorithm->AlgorithmName()));
			BenchmarkHandle->kill();
			BenchmarkHandle->close();
			KillAllUsedMinerProcesses();
			}
		catch (...) {
//			return;
			}
//		finally {
			_BenchmarkProcessStatus=Enums::BenchmarkProcessStatus::DoneKilling;
			Helpers::ConsolePrint("BENCHMARK", QString("Benchmark process %1 algorithm %2 KILLED").arg(BenchmarkProcessPath).arg(BenchmarkAlgorithm->AlgorithmName()));
//			}
		}
}

void Miner::BenchmarkThreadRoutineStartSettup()
{
//	BenchmarkHandle->BeginErrorReadLine();
//	BenchmarkHandle->BeginOutputReadLine();
}

void Miner::BenchmarkThreadRoutineCatch(LException ex)
{
	BenchmarkAlgorithm->BenchmarkSpeed(0);

	Helpers::ConsolePrint(MinerTag(), QString("Benchmark exception: ")+ex.what());
	if (BenchmarkCommunicator!=nullptr && !OnBenchmarkCompleteCalled) {
		OnBenchmarkCompleteCalled=true;
		BenchmarkCommunicator->OnBenchmarkComplete(false, GetFinalBenchmarkString());
		}
}

void Miner::BenchmarkThreadRoutineCatch(QException ex)
{
	BenchmarkAlgorithm->BenchmarkSpeed(0);

	Helpers::ConsolePrint(MinerTag(), QString("Benchmark exception: ")+ex.what());
	if (BenchmarkCommunicator!=nullptr && !OnBenchmarkCompleteCalled) {
		OnBenchmarkCompleteCalled=true;
		BenchmarkCommunicator->OnBenchmarkComplete(false, GetFinalBenchmarkString());
		}
}

void Miner::BenchmarkThreadRoutineCatch(std::exception ex)
{
	BenchmarkAlgorithm->BenchmarkSpeed(0);

	Helpers::ConsolePrint(MinerTag(), QString("Benchmark exception: ")+ex.what());
	if (BenchmarkCommunicator!=nullptr && !OnBenchmarkCompleteCalled) {
		OnBenchmarkCompleteCalled=true;
		BenchmarkCommunicator->OnBenchmarkComplete(false, GetFinalBenchmarkString());
		}
}

QString Miner::GetFinalBenchmarkString()
{
	return BenchmarkSignalTimedout && !TimeoutStandard
		? International::GetText("Benchmark_Timedout")
		: International::GetText("Benchmark_Terminated");
}

void Miner::BenchmarkThreadRoutineFinish()
{
	Enums::BenchmarkProcessStatus status=Enums::BenchmarkProcessStatus::Finished;

	if (!BenchmarkAlgorithm->BenchmarkNeeded()) {
		status=Enums::BenchmarkProcessStatus::Success;
		}
	try {
		QFile f(_benchmarkLogPath);
		f.open(QIODevice::Append|QIODevice::Text);
		QTextStream sw(&f);
		foreach (QString line, *BenchLines) {
			sw << line << endl;
			}
		f.close();
		}
	catch (...) {}

	_BenchmarkProcessStatus=status;
	DualAlgorithm* dualAlg=qobject_cast<DualAlgorithm*>(BenchmarkAlgorithm);
	if (dualAlg!=nullptr) {
		if (!dualAlg->TuningEnabled) {
			// Tuning will report speed
			Helpers::ConsolePrint("BENCHMARK", "Final speed: "+Helpers::FormatDualSpeedOutput(dualAlg->BenchmarkSpeed(), dualAlg->SecondaryBenchmarkSpeed(), dualAlg->DualNiceHashID()));
			}
		}
	else {
		Helpers::ConsolePrint("BENCHMARK", "Final Speed: "+Helpers::FormatDualSpeedOutput(BenchmarkAlgorithm->BenchmarkSpeed(), 0, BenchmarkAlgorithm->NiceHashID));
		}

	Helpers::ConsolePrint("BENCHMARK", "Benchmark ends");
	if (BenchmarkCommunicator!=nullptr && !OnBenchmarkCompleteCalled) {
		OnBenchmarkCompleteCalled=true;
		bool isOK= status==Enums::BenchmarkProcessStatus::Success;
		BenchmarkCommunicator->OnBenchmarkComplete(isOK, isOK? "" : GetFinalBenchmarkString());
		}
	moveToThread(_thread0);
}

void Miner::BenchmarkThreadRoutine(QStringList commandLine)
{
	BenchmarkSignalQuit=false;
	BenchmarkSignalHanged=false;
	BenchmarkSignalFinnished=false;
	BenchmarkException=nullptr;

	QThread::msleep(ConfigManager.generalConfig->MinerRestartDelayMS);

	try {
		Helpers::ConsolePrint("BENCHMARK", "Benchmark starts");
		BenchmarkHandle=BenchmarkStartProcess(commandLine);

		BenchmarkThreadRoutineStartSettup();
		// don't use wait for it breaks everything
		_BenchmarkProcessStatus=Enums::BenchmarkProcessStatus::Running;
		bool exited=BenchmarkHandle->waitForFinished((BenchmarkTimeoutInSeconds(BenchmarkTimeInSeconds)+20)*1000);
		if (BenchmarkSignalTimedout && !TimeoutStandard) {
			throw LException("Benchmark timedout");
			}
		if (BenchmarkException!=nullptr) {
			throw *BenchmarkException;
			}
		if (BenchmarkSignalQuit) {
			throw LException("Termined by user request");
			}
		if (BenchmarkSignalHanged || !exited) {
			throw LException("Miner is not responding");
			}
		if (BenchmarkSignalFinnished) {
			}
		}
	catch (LException& ex) {
		BenchmarkThreadRoutineCatch(ex);
//		return;
		}
//	finally {
		BenchmarkThreadRoutineFinish();
//		}
}

void Miner::BenchmarkThreadRoutineAlternate(QStringList commandLine, int benchmarkTimeWait)
{
	CleanOldLogs();

	BenchmarkSignalQuit=false;
	BenchmarkSignalHanged=false;
	BenchmarkSignalFinnished=false;
	BenchmarkException=nullptr;

	QThread::msleep(ConfigManager.generalConfig->MinerRestartDelayMS);

	try {
		Helpers::ConsolePrint("BENCHMARK", "Benchmark starts");
		Helpers::ConsolePrint(MinerTag(), "Benchmark should end in : "+QString::number(benchmarkTimeWait)+" seconds");
		BenchmarkHandle=BenchmarkStartProcess(commandLine);
		BenchmarkHandle->waitForFinished(benchmarkTimeWait+2);
		QElapsedTimer* benchmarkTimer=new QElapsedTimer;
		benchmarkTimer->start();
		_BenchmarkProcessStatus=Enums::BenchmarkProcessStatus::Running;
		bool keepRunning=true;
		while (keepRunning && IsActiveProcess(BenchmarkHandle->processId())) {
			if ((benchmarkTimer->elapsed()/1000)>=(benchmarkTimeWait+2)
				|| BenchmarkSignalQuit
				|| BenchmarkSignalFinnished
				|| BenchmarkSignalHanged
				|| BenchmarkSignalTimedout
				|| BenchmarkException!=nullptr
				) {
//				KillProspectorClaymoreMinerBase(_MinerExeName);
				KillProspectorClaymoreMinerBase(_currentPidData->Pid);
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
			QThread::msleep(1000); // wait a second reduce CPU load
			}
		}
	catch (LException& ex) {
		BenchmarkThreadRoutineCatch(ex);
		}
//	finally {
		BenchmarkAlgorithm->BenchmarkSpeed(0);
		QString latestLogFile="";
		QDir dirInfo(_WorkingDirectory);
		foreach (QString file, dirInfo.entryList({GetLogFileName()}, QDir::Files)) {
			latestLogFile=file;
			break;
			}
		if (BenchmarkHandle!=nullptr) {
			BenchmarkHandle->waitForFinished(10000);
			}
		// read file log
		if (QFile::exists(_WorkingDirectory+latestLogFile)) {
			QFile file(_WorkingDirectory+latestLogFile);
			file.open(QIODevice::ReadOnly|QIODevice::Text);
			QStringList lines=QString(file.readAll()).remove(QChar('\r')).split('\n');
			file.close();
			ProcessBenchLinesAlternate(lines);
			}
		BenchmarkThreadRoutineFinish();
//		}
}

void Miner::CleanOldLogs()
{
	// clean old logs
	try {
		QDir dirInfo(_WorkingDirectory);
		if (dirInfo.exists()) {
			foreach (QString file, dirInfo.entryList({GetLogFileName()}, QDir::Files)) {
				QFile::remove(_WorkingDirectory+file);
				}
			}
		}
	catch (...) {}
}

QString Miner::GetDeviceID()
{
	QStringList ids;
	foreach (MiningPair* x, *MiningSetup_->MiningPairs) {
		ids << QString::number(x->Device->ID);
		}
	QString idStr=ids.join(',');

	if (!IsMultiType) {
		return idStr;
		}

	// Miners that use multiple dev types need to also discriminate based on that
	QStringList types;
	foreach (MiningPair* x, *MiningSetup_->MiningPairs) {
		types << QMetaEnum::fromType<Enums::DeviceType>().valueToKey((int)x->Device->DeviceType);
		}
	return types.join(',')+"-"+idStr;
}

QString Miner::GetLogFileName()
{
	return GetDeviceID()+"_log.txt";
}

void Miner::ProcessBenchLinesAlternate(QStringList lines)
{ }

QString Miner::GetServiceUrl(Enums::AlgorithmType algo)
{
	return Globals::GetLocationURL(algo, Globals::MiningLocation[ConfigManager.generalConfig->ServiceLocation], _ConectionType);
}

bool Miner::IsActiveProcess(qint64 pid)
{
	try {
		return QDir(QString("/proc/%1").arg(pid)).exists();
		}
	catch (...) {
		return false;
		}
}

NiceHashProcess* Miner::_Start()
{
	// never start when ended
	if (_isEnded) {
		return nullptr;
		}

	PreviousTotalMH=0.0;
	if (!LastCommandLine.join("").length()) {
		return nullptr;
		}

	NiceHashProcess* P=new NiceHashProcess;

	if (_WorkingDirectory.length()>1) {
		P->setWorkingDirectory(_WorkingDirectory);
		}
	if (MinersSettingsManager::MinerSystemVariables->contains(_Path)) {
		QProcessEnvironment env=QProcessEnvironment::systemEnvironment();
		foreach (QString envName, (*MinersSettingsManager::MinerSystemVariables)[_Path]->keys()) {
			QString envValue=(*MinersSettingsManager::MinerSystemVariables)[_Path]->value(envName);
			env.insert(envName, envValue);
			}
		P->setProcessEnvironment(env);
		}

//	P->setProgram(_Path);
	P->setProgram("./"+_MinerExeName);
//	P->ExitEvent=(QFunctionPointer)QOverload<void>::of(&Miner::Miner_Exited);
//	P->ExitEvent=(QFunctionPointer)&Miner::Miner_Exited;
	connect(P, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(Miner_Exited(int, QProcess::ExitStatus)));
	connect(P, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(Miner_Error(QProcess::ProcessError)));

	LastCommandLine.removeAll("");
	P->setArguments(LastCommandLine);

	try {
		if (P->Start()) {
			_IsRunning=true;

			_currentPidData=new MinerPidData;
			_currentPidData->MinerBinPath=_WorkingDirectory+P->program();
			_currentPidData->Pid=P->Id;
			_allPidData->append(_currentPidData);

			Helpers::ConsolePrint(MinerTag(), "Starting miner "+ProcessTag()+" "+LastCommandLine.join(' '));

			StartCoolDownTimerChecker();

			return P;
			}
		Helpers::ConsolePrint(MinerTag(), "NOT STARTED "+ProcessTag()+" "+LastCommandLine.join(' '));
		return nullptr;
		}
	catch (LException& ex) {
		Helpers::ConsolePrint(MinerTag(), ProcessTag()+" _Start: "+ex.what());
		return nullptr;
		}
	catch (QException& ex) {
		Helpers::ConsolePrint(MinerTag(), ProcessTag()+" _Start: "+ex.what());
		return nullptr;
		}
}

void Miner::StartCoolDownTimerChecker()
{
	if (ConfigManager.generalConfig->CoolDownCheckEnabled) {
		Helpers::ConsolePrint(MinerTag(), ProcessTag()+" Starting cooldown checker");
		if (_cooldownCheckTimer!=nullptr && _cooldownCheckTimer->isActive()) {
			_cooldownCheckTimer->stop();
			}
		// cool down init
		if (_cooldownCheckTimer!=nullptr) {delete _cooldownCheckTimer;}
		_cooldownCheckTimer=new QTimer(this);
		_cooldownCheckTimer->setInterval(MinCooldownTimeInMilliseconds);
		connect(_cooldownCheckTimer, SIGNAL(timeout()), this, SLOT(MinerCoolingCheck_Tick()));
		_cooldownCheckTimer->start();
		_currentCooldownTimeInSecondsLeft= _currentCooldownTimeInSeconds= MinCooldownTimeInMilliseconds;
		}
	else {
		Helpers::ConsolePrint(MinerTag(), "Cooldown checker disabled");
		}
	CurrentMinerReadStatus=Enums::MinerApiReadStatus::NONE;
}

void Miner::Miner_Exited()
{
	ScheduleRestart(5000);
}

void Miner::ScheduleRestart(int ms)
{
	int restartInMs= ConfigManager.generalConfig->MinerRestartDelayMS>ms
		? ConfigManager.generalConfig->MinerRestartDelayMS
		: ms;
	Helpers::ConsolePrint(MinerTag(), ProcessTag()+QString(" Miner_Exited Will restart in %1 ms").arg(restartInMs));
	if (ConfigManager.generalConfig->CoolDownCheckEnabled) {
		CurrentMinerReadStatus=Enums::MinerApiReadStatus::RESTART;
		_needsRestart=true;
		_currentCooldownTimeInSecondsLeft=restartInMs;
		}
	else {
		// directly restart since cooldown checker not running
		QThread::msleep(restartInMs);
		Restart();
		}
}

void Miner::Restart()
{
	if (_isEnded) {
		return;
		}
	Helpers::ConsolePrint(MinerTag(), ProcessTag()+" Restarting miner..");
	Stop(Enums::MinerStopType::END); // stop miner first
	QThread::msleep(ConfigManager.generalConfig->MinerRestartDelayMS);
	if (ProcessHandle!=nullptr) {delete ProcessHandle;}
	ProcessHandle=_Start(); // start with old command line
}

bool Miner::IsApiEof(uchar third, uchar second, uchar last)
{
	return false;
}

QString* Miner::GetApiDataAsync(int port, QString dataToSend, bool exitHack/*=false*/, bool overrideLoop/*=false*/)
{
//	QtConcurrent::run<QString*>();
/*	QTcpSocket* client=new QTcpSocket(this);
	connect(client, &QTcpSocket::connected, [=](){ std::cout << "connected" << std::endl;});
	connect(client, &QTcpSocket::disconnected, [=](){ std::cout << "disconnected" << std::endl;});
	connect(client, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error), [=](QAbstractSocket::SocketError err){ std::cout << "error " << err << std::endl;});
	connect(client, &QTcpSocket::hostFound, [=](){ std::cout << "hostFound" << std::endl;});
	connect(client, &QTcpSocket::bytesWritten, [=](qint64 b){ std::cout << "bytesWritten " << b << std::endl;});
	connect(client, &QTcpSocket::readyRead, [=](){
		std::cout << "readyRead" << std::endl;
		quint64 b=client->bytesAvailable();
		std::cout << b << "B" << std::endl;
		QByteArray ba=client->readAll();
		std::cout << ba.toStdString() << std::endl;
		});
	client->connectToHost("127.0.0.1", port);
	client->waitForConnected();
	client->write(DataToSend.toLatin1());
	client->flush();
	return nullptr;
*/
	QString* responseFromServer=nullptr;
	QTcpSocket tcpc(this);
	try {
		tcpc.connectToHost("127.0.0.1", port);
		tcpc.waitForConnected();
		tcpc.write(dataToSend.toLatin1());
		tcpc.flush();

		QByteArray incomingBuffer, in;
		int prevOffset=-1;
		int offset=0;
		bool fin=false;
		int r;

		while (!fin && tcpc.state()==QAbstractSocket::SocketState::ConnectedState) {
			tcpc.waitForReadyRead();
			in=tcpc.readAll();
			r=in.length();
			incomingBuffer+=in;
			in.clear();
			for (int i=offset; i<offset+r; i++) {
				if (((char)incomingBuffer[i])==0x7C || ((char)incomingBuffer[i])==0x00
					|| (i>2 && IsApiEof(incomingBuffer[i-2], incomingBuffer[i-1], incomingBuffer[i]))
					|| overrideLoop
					) {
					fin=true;
					break;
					}
				}
			offset+=r;
			if (exitHack) {
				if (prevOffset==offset) {
					fin=true;
					break;
					}
				prevOffset=offset;
				}
			}
		tcpc.close();

		if (offset>0) {
			responseFromServer=new QString(incomingBuffer);
			}
		}
	catch (std::exception& ex) {
		Helpers::ConsolePrint(MinerTag(), ProcessTag()+" GetAPIData reason: "+ex.what());
		return nullptr;
		}
	return responseFromServer;
}

ApiData* Miner::GetSummaryCpuAsync(QString method/*=""*/, bool overrideLoop/*=false*/)
{
	ApiData* ad=new ApiData(MiningSetup_->CurrentAlgorithmType);

	try {
		CurrentMinerReadStatus=Enums::MinerApiReadStatus::WAIT;
		QString dataToSend=GetHttpRequestNhmAgentStrin(method);
		QString* respStr=QtConcurrent::run(this, &Miner::GetApiDataAsync, _ApiPort, dataToSend, false, false).result();

		if (respStr==nullptr || respStr->isEmpty()) {
			CurrentMinerReadStatus=Enums::MinerApiReadStatus::NETWORK_EXCEPTION;
			throw LException("Response is empty!");
			}
		if (respStr->indexOf("HTTP/1.1 200 OK")>-1) {
			QString sub=respStr->mid(respStr->indexOf(HttpHeaderDelimiter)+HttpHeaderDelimiter.length());
			delete respStr;
			respStr=new QString(sub);
			}
		else {
			throw LException(QString("Response not HTTP formed! %1").arg(*respStr).toLatin1());
			}

		QJsonParseError pe;
		QJsonDocument resp=QJsonDocument::fromJson(respStr->toLatin1(), &pe);

		if (!resp.isNull()) {
			QJsonArray totals=resp.object().value("hashrate").toObject().value("total").toArray();
			foreach (QJsonValue total, totals) {
				if (total.isNull()) continue;
				ad->Speed=total.toDouble();
				break;
				}
			if (!ad->Speed) {
				CurrentMinerReadStatus=Enums::MinerApiReadStatus::READ_SPEED_ZERO;
				}
			else {
				CurrentMinerReadStatus=Enums::MinerApiReadStatus::GOT_READ;
				}
			}
		else {
			throw LException(QString("Response does not contain speed data: %1").arg(respStr->trimmed()).toLatin1());
			}
		}
	catch (LException& ex) {
		Helpers::ConsolePrint(MinerTag(), ex.what());
		}

	return ad;
}

QString Miner::GetHttpRequestNhmAgentStrin(QString cmd)
{
	return "GET /"+cmd+" HTTP/1.1\r\n"
		+"Host: 127.0.0.1\r\n"
		+"User-Agent: NiceHashQtMiner/"+qApp->applicationVersion()+"\r\n"
		+"\r\n";
}

ApiData* Miner::GetSummaryCpuCcminerAsync()
{
//	QString aname;
	ApiData* ad=new ApiData(MiningSetup_->CurrentAlgorithmType);

	QString dataToSend=GetHttpRequestNhmAgentStrin("summary");

	QString* resp=QtConcurrent::run(this, &Miner::GetApiDataAsync, _ApiPort, dataToSend, false, false).result();
	if (resp==nullptr) {
		Helpers::ConsolePrint(MinerTag(), ProcessTag()+" summary is null");
		CurrentMinerReadStatus=Enums::MinerApiReadStatus::NONE;
		return nullptr;
		}

	try {
		QStringList resps=resp->split(';', QString::SplitBehavior::SkipEmptyParts);
		foreach (QString res, resps) {
			QStringList optval=res.split('=', QString::SplitBehavior::SkipEmptyParts);
			if (optval.length()!=2) {
				continue;
				}
			if (optval[0]=="ALGO") {
//				aname=optval[1];
				}
			else if (optval[0]=="KHS") {
				ad->Speed=optval[1].toDouble()*1000; // HPS
				}
			}
		}
	catch (...) {
		Helpers::ConsolePrint(MinerTag(), ProcessTag()+" Could not read data from API bind port");
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

void Miner::CoolDown()
{
	if (_currentCooldownTimeInSeconds>MinCooldownTimeInMilliseconds) {
		_currentCooldownTimeInSeconds=MinCooldownTimeInMilliseconds;
		Helpers::ConsolePrint(MinerTag(), QString("%1 Reseting cool time = %2 ms").arg(ProcessTag()).arg(MinCooldownTimeInMilliseconds));
		CurrentMinerReadStatus=Enums::MinerApiReadStatus::NONE;
		}
}

void Miner::CoolUp()
{
	_currentCooldownTimeInSeconds*=2;
	Helpers::ConsolePrint(MinerTag(), QString("%1 Cooling UP, cool time is %2 ms").arg(ProcessTag()).arg(_currentCooldownTimeInSeconds));
	if (_currentCooldownTimeInSeconds>_maxCooldownTimeInMilliseconds) {
		CurrentMinerReadStatus=Enums::MinerApiReadStatus::RESTART;
		Helpers::ConsolePrint(MinerTag(), ProcessTag()+" MAX cool time exceeded. RESTARTING");
		Restart();
		}
}

void Miner::MinerCoolingCheck_Tick()
{
	if (_isEnded) {
		End();
		return;
		}

	_currentCooldownTimeInSecondsLeft-=(int)_cooldownCheckTimer->interval();
	// if times up
	if (_currentCooldownTimeInSecondsLeft>0) {
		return;
		}
	if (_needsRestart) {
		_needsRestart=false;
		Restart();
		}
	else {
		switch (CurrentMinerReadStatus) {
			case Enums::MinerApiReadStatus::GOT_READ:
				CoolDown();
				break;
			case Enums::MinerApiReadStatus::READ_SPEED_ZERO:
				Helpers::ConsolePrint(MinerTag(), ProcessTag()+" READ SPEED ZERO, will cool up");
				CoolUp();
				break;
			case Enums::MinerApiReadStatus::RESTART:
				Restart();
				break;
			default:
				CoolUp();
				break;
			}
		}
	// set new times left from CoolUp/Down change
	_currentCooldownTimeInSecondsLeft=_currentCooldownTimeInSeconds;
}

void Miner::Miner_Exited(int exitCode, QProcess::ExitStatus exitStatus)
{
//	NiceHashProcess* proc=(NiceHashProcess*)sender();
//	qint64 ba=proc->bytesAvailable();
//	QProcess::ProcessError er=proc->error();
//Helpers::ConsolePrint("DEBUG", QString("Miner::Miner_Exited(%1, %2): %3").arg(exitCode).arg(exitStatus).arg(proc!=nullptr? QString(proc->readAll()) : ""));
#if WITH_DEBUG
	Helpers::ConsolePrint("DEBUG", QString("Miner::Miner_Exited(%1, %2)").arg(exitCode).arg(exitStatus));
	QProcess* sender=qobject_cast<QProcess*>(QObject::sender());
	Helpers::ConsolePrint("DEBUG", QString("stdErr: %1").arg(QString(sender->readAllStandardError())));
#endif
	ScheduleRestart(5000);
}

void Miner::Miner_Error(QProcess::ProcessError error)
{
#if WITH_DEBUG
	Helpers::ConsolePrint("DEBUG", QString("Miner::Miner_Error: %1").arg(error));
	QProcess* sender=qobject_cast<QProcess*>(QObject::sender());
	Helpers::ConsolePrint("DEBUG", QString("stdErr: %1").arg(QString(sender->readAllStandardError())));
#endif
	ScheduleRestart(5000);
}
#if WITH_DEBUG
void Miner::BenchmarkErrorOccurred(QProcess::ProcessError error)
{
	Helpers::ConsolePrint("DEBUG", QString("Miner::BenchmarkErrorOccurred: %1").arg(error));
	QProcess* sender=qobject_cast<QProcess*>(QObject::sender());
	Helpers::ConsolePrint("DEBUG", QString("stdErr: %1").arg(QString(sender->readAllStandardError())));
}

void Miner::BenchmarkStateChanged(QProcess::ProcessState newState)
{
	Helpers::ConsolePrint("DEBUG", QString("Miner::BenchmarkStateChanged(%1").arg(newState));
}

void Miner::BenchmarkStarted()
{
	Helpers::ConsolePrint("DEBUG", "Miner::BenchmarkStarted");
}
#endif
