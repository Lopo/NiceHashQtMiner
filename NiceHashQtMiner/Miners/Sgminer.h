#ifndef MINERS_SGMINER_H_
#define MINERS_SGMINER_H_


#include "Miners/Miner.h"


class Sgminer
	: public Miner
{
//	Q_OBJECT
public:
	Sgminer();
	void KillSgminer(); // use ONLY for exiting a benchmark
	void EndBenchmarkProcces() override;
	void Start(QString url, QString btcAdress, QString worker) override;
	ApiData* GetSummaryAsync() override;


protected:
	void _Stop(Enums::MinerStopType willswitch) override;
	QStringList BenchmarkCreateCommandLine(Algorithm* algorithm, int time) override;
	bool BenchmarkParseLine(QString outdata) override;
	void BenchmarkThreadRoutineStartSettup() override;
	void BenchmarkOutputErrorDataReceivedImpl(QString outdata) override;
	QString GetFinalBenchmarkString() override;
	void BenchmarkThreadRoutine(QStringList commandLine) override;
//	QProcess* BenchmarkStartProcess(QStringList CommandLine) override;

private:
	int _gpuPlatformNumber=-1;
	QElapsedTimer* _benchmarkTimer=new QElapsedTimer;
};

#endif
