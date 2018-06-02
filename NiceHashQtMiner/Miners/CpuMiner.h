#ifndef MINERS_CPUMINER_H_
#define MINERS_CPUMINER_H_

#include "Miners/Miner.h"

class CpuMiner
	: public Miner
{
public:
	CpuMiner();
	void Start(QString url, QString btcAdress, QString worker) override;
	ApiData* GetSummaryAsync() override;

protected:
	void _Stop(Enums::MinerStopType willswitch) override;
	NiceHashProcess* _Start() override;
	QStringList BenchmarkCreateCommandLine(Algorithm* algorithm, int time) override;
	QProcess* BenchmarkStartProcess(QStringList CommandLine) override;
	bool BenchmarkParseLine(QString outdata) override;
	void BenchmarkOutputErrorDataReceivedImpl(QString outdata) override;
};

#endif
