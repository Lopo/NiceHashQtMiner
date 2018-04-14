#ifndef MINERS_MINER_H_
#define MINERS_MINER_H_

#include "config.h"
#include "Miners/Grouping/MiningSetup.h"
#include <QProcess>
#include <QTimer>
#include <QElapsedTimer>
#include <QException>
#include <QThread>

class LException;
class IBenchmarkComunicator;
class MiningPair;
class NiceHashProcess;
class Algorithm;
class Enums;

class ApiData //done
	: public QObject
{
	Q_OBJECT
public:
	ApiData(Enums::AlgorithmType algorithmID, Enums::AlgorithmType secondaryAlgorithmID=Enums::AlgorithmType::NONE);
	Enums::AlgorithmType DualAlgorithmID();

	Enums::AlgorithmType AlgorithmID;
	Enums::AlgorithmType SecondaryAlgorithmID;
	QString AlgorithmName;
	double Speed;
	double SecondarySpeed;
};


class MinerPidData //done
{
public:
	QString MinerBinPath;
	qint64 Pid=-1;
};


class Miner
	: public QObject
{
	Q_OBJECT
	// MINER_ID_COUNT used to identify miners creation
	Q_PROPERTY(long MinerIDCount READ MinerIDCount)
	Q_PROPERTY(Enums::NhmConectionType ConectionType READ ConectionType)
	Q_PROPERTY(int ApiPort READ ApiPort)
	Q_PROPERTY(bool IsApiReadException READ IsApiReadException) // if miner has no API bind port for reading curently only CryptoNight on ccminer
//	Q_PROPERTY(bool IsNeverHideMiningWindow READ IsNeverHideMiningWindow)
	// mining algorithm stuff
	Q_PROPERTY(bool IsInit READ IsInit)
	Q_PROPERTY(bool IsRunning READ IsRunning)
	Q_PROPERTY(QString Path READ Path)
	// the defaults will be
	Q_PROPERTY(QString WorkingDirectory READ WorkingDirectory)
	Q_PROPERTY(QString MinerExeName READ MinerExeName)

	Q_PROPERTY(Enums::BenchmarkProcessStatus BenchmarkProcessStatus READ BenchmarkProcessStatus)

public:
	inline Enums::NhmConectionType ConectionType() const { return _ConectionType; };

	QString MinerDeviceName;
	inline bool IsApiReadException() const { return _IsApiReadException; };
//	inline bool IsNeverHideMiningWindow() const { return _IsNeverHideMiningWindow; };
	inline bool IsRunning() const { return _IsRunning; };

	bool BenchmarkSignalQuit=false;
	bool BenchmarkSignalHanged=false;
	bool BenchmarkSignalTimedout=false;

	inline Enums::BenchmarkProcessStatus BenchmarkProcessStatus() const { return _BenchmarkProcessStatus; };

	bool IsUpdatingApi=false;

	virtual void InitMiningSetup(MiningSetup* miningSetup);
	void InitBenchmarkSetup(MiningPair* benchmarkPair);
	// TAG for identifying miner
	QString MinerTag();
	QString ProcessTag();
	void KillAllUsedMinerProcesses();
	virtual void Start(QString url, QString btcAdress, QString worker)=0;
	virtual void Stop(Enums::MinerStopType willswitch=Enums::MinerStopType::SWITCH);
	void End();

	int BenchmarkTimeoutInSeconds(int timeInSeconds);
	virtual void BenchmarkStart(int time, IBenchmarkComunicator* benchmarkComunicator);
	void InvokeBenchmarkSignalQuit();
	virtual void EndBenchmarkProcces();

	virtual ApiData* GetSummaryAsync()=0;
	QString* GetApiDataAsync(int port, QString dataToSend, bool exitHack=false, bool overrideLoop=false); // was protected
	ApiData* GetSummaryCpuAsync(QString method="", bool overrideLoop=false); // was protected
	ApiData* GetSummaryCpuCcminerAsync(); // was protected

protected:
	inline long MinerIDCount() const { return _MinerIDCount; };
	Enums::NhmConectionType _ConectionType;
	// used to identify miner instance
	long MinerID=0;
	inline int ApiPort() const { return _ApiPort; };
	bool _IsApiReadException=false;
//	bool _IsNeverHideMiningWindow=false;
	inline bool IsInit() const { return _IsInit; };
	MiningSetup* _MiningSetup=nullptr;
	// sgminer/zcash claymore workaround
	bool IsKillAllUsedMinerProcs=false;
	bool _IsRunning;
	inline QString Path() const { return _Path; };
	QStringList LastCommandLine;
	double PreviousTotalMH=0; // TODO check this
	QString WorkingDirectory() const { return _WorkingDirectory; };
	QString MinerExeName() const { return _MinerExeName; };
	NiceHashProcess* ProcessHandle=nullptr;

	bool BenchmarkSignalFinnished=false;
	IBenchmarkComunicator* BenchmarkCommunicator=nullptr;
	bool OnBenchmarkCompleteCalled=false;
	Algorithm* BenchmarkAlgorithm=nullptr;
	Enums::BenchmarkProcessStatus _BenchmarkProcessStatus;
	QString BenchmarkProcessPath;
	QProcess* BenchmarkHandle=nullptr;
	LException* BenchmarkException=nullptr;
	int BenchmarkTimeInSeconds=0;

	QStringList* BenchLines=nullptr;

	Enums::MinerApiReadStatus CurrentMinerReadStatus;

	const QString HttpHeaderDelimiter="\r\n\r\n";

	Miner(QString minerDeviceName, int maxCDTime);
	~Miner();

	void SetWorkingDirAndProgName(QString fullPath);
	QString GetUsername(QString btcAdress, QString worker);
	virtual void _Stop(Enums::MinerStopType willswitch)=0;
	void Stop_cpu_ccminer_sgminer_nheqminer(Enums::MinerStopType willswitch);
//	void KillProspectorClaymoreMinerBase(QString exeName);
	void KillProspectorClaymoreMinerBase(qint64 pid);
	virtual QStringList GetDevicesCommandString();

	virtual QStringList BenchmarkCreateCommandLine(Algorithm* algorithm, int time)=0;
	virtual QProcess* BenchmarkStartProcess(QStringList CommandLine);
	virtual void BenchmarkOutputErrorDataReceivedImpl(QString outdata)=0;
	void CheckOutdata(QString outdata);
	double BenchmarkParseLine_cpu_ccminer_extra(QString outdata);
	virtual void BenchmarkThreadRoutineStartSettup();
	void BenchmarkThreadRoutineCatch(LException ex);
	void BenchmarkThreadRoutineCatch(QException ex);
	void BenchmarkThreadRoutineCatch(std::exception ex);
	virtual QString GetFinalBenchmarkString();
	void BenchmarkThreadRoutineFinish();
	void BenchmarkThreadRoutineAlternate(QStringList commandLine, int benchmarkTimeWait);
	void CleanAllOldLogs();
	virtual void ProcessBenchLinesAlternate(QStringList lines);
	virtual bool BenchmarkParseLine(QString outdata)=0;
	QString GetServiceUrl(Enums::AlgorithmType algo);
	bool IsActiveProcess(qint64 pid);

	virtual NiceHashProcess* _Start();
	void StartCoolDownTimerChecker();
	virtual void Miner_Exited();
	void ScheduleRestart(int ms);
	void Restart();
	virtual bool IsApiEof(uchar third, uchar second, uchar last);
//	QString* GetApiDataAsync(int port, QString dataToSend, bool exitHack=false, bool overrideLoop=false); // QtConcurrent needs public
//	ApiData* GetSummaryCpuAsync(QString method="", bool overrideLoop=false); // QtConcurrent needs public
	QString GetHttpRequestNhmAgentStrin(QString cmd);
//	ApiData* GetSummaryCpuCcminerAsync(); // QtConcurrent needs public

private:
	QThread* _thread0=QThread::currentThread();
	QThread _thread;

	static long _MinerIDCount;
	QString* _minerTag=nullptr;
	int _ApiPort=0;
	bool _IsInit=false;
	QString _Path;
	QString _WorkingDirectory;
	QString _MinerExeName;
	MinerPidData* _currentPidData=nullptr;
	QList<MinerPidData*>* _allPidData=new QList<MinerPidData*>;

	QElapsedTimer* _benchmarkTimeOutStopWatch=nullptr;

	QString _benchmarkLogPath="";

	const int MinCooldownTimeInMilliseconds=5*1000;

	int _maxCooldownTimeInMilliseconds=0;
	QTimer* _cooldownCheckTimer=nullptr;
	int _currentCooldownTimeInSeconds=MinCooldownTimeInMilliseconds;
	int _currentCooldownTimeInSecondsLeft=MinCooldownTimeInMilliseconds;
	const int IsCooldownCheckTimerAliveCap=15;
	bool _needsRestart=false;

	bool _isEnded=false;

	void SetApiPort();
	QString ProcessTag(const MinerPidData pidData);
//	void BenchmarkOutputErrorDataReceived(QString data);

	void CoolDown();
	void CoolUp();

protected Q_SLOTS:
	void BenchmarkHandle_Exited(int exitCode, QProcess::ExitStatus exitStatus);
	void BenchmarkOutputErrorDataReceived();
	virtual void BenchmarkThreadRoutine(QStringList commandLine);
	virtual void Miner_Exited(int exitCode, QProcess::ExitStatus exitStatus);
	virtual void Miner_Error(QProcess::ProcessError error);
	void MinerCoolingCheck_Tick();
#if WITH_DEBUG
	void BenchmarkErrorOccurred(QProcess::ProcessError error);
	void BenchmarkStateChanged(QProcess::ProcessState newState);
	void BenchmarkStarted();
#endif
};

#endif
