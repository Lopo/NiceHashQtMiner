#ifndef MINERS_XMRIG_H_
#define MINERS_XMRIG_H_

#include "Miners/Miner.h"


class Xmrig
	: public Miner
{
public:
	Xmrig();
	void Start(QString url, QString btcAdress, QString worker) override;
	ApiData* GetSummaryAsync() override;

protected:
	void _Stop(Enums::MinerStopType willswitch) override;
	bool IsApiEof(uchar third, uchar second, uchar last) override;
	QStringList BenchmarkCreateCommandLine(Algorithm* algorithm, int time) override;
	void BenchmarkThreadRoutine(QStringList commandLine) override;
	void ProcessBenchLinesAlternate(QStringList lines) override;
	void BenchmarkOutputErrorDataReceivedImpl(QString outdata) override;
	bool BenchmarkParseLine(QString outdata) override;

private:
	int _benchmarkTimeWait=120;
	const QString LookForStart25="speed 2.5s/60s/15m";
	const QString LookForStart10="speed 10s/60s/15m";
	const QString LookForEnd="h/s max";
	QStringList GetStartCommand(QString url, QString btcAdress, QString worker);
};

#endif
