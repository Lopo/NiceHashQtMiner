#ifndef MINERS_EQUIHASH_DTSM_H_
#define MINERS_EQUIHASH_DTSM_H_

#include "Miners/Miner.h"


class Dtsm
	: public Miner
{
public:
	Dtsm();
	void Start(QString url, QString btcAdress, QString worker) override;
	ApiData* GetSummaryAsync() override;

	class DtsmGpuResult
	{
	public:
		double sol_ps=0;
	};

	class DtsmResponse
	{
	public:
		QList<DtsmGpuResult*>* result=nullptr;
		static DtsmResponse* fromJson(QString json);
	};

protected:
	void _Stop(Enums::MinerStopType willswitch) override;
	QStringList BenchmarkCreateCommandLine(Algorithm* algorithm, int time) override;
	void BenchmarkThreadRoutine(QStringList commandLine) override;
	void ProcessBenchLinesAlternate(QStringList lines) override;
	void BenchmarkOutputErrorDataReceivedImpl(QString outdata) override;
	bool BenchmarkParseLine(QString outdata) override;
	bool IsApiEof(uchar third, uchar second, uchar last) override;

private:
	const double DevFee=2.0;
	const QString LookForStart="sol/s: ";
	const QString LookForEnd="sol/w";

	int _benchmarkTime=60;

	QStringList GetStartCommand(QString url, QString btcAddress, QString worker);
	QStringList GetDeviceCommand();
};

#endif
