#ifndef MINERS_EWBF_H_
#define MINERS_EWBF_H_

#include "Miners/Miner.h"


class Ewbf
	: public Miner
{
	Q_OBJECT
public:
	Ewbf();
	void Start(QString url, QString btcAdress, QString worker) override;
	ApiData* GetSummaryAsync() override;

protected:
	QStringList GetDevicesCommandString() override;
	void KillMinerBase(QString exeName);
	QStringList BenchmarkCreateCommandLine(Algorithm* algorithm, int time) override;
	void BenchmarkThreadRoutine(QStringList commandLine) override;
	void CleanAllOldLogs();
	void BenchmarkOutputErrorDataReceivedImpl(QString outdata) override;
	bool BenchmarkParseLine(QString outdata) override;
	double GetNumber(QString outdata);
	double GetNumber(QString outdata, QString lookForStart, QString lookForEnd);
	void _Stop(Enums::MinerStopType willswitch) override;

private:
	class Result
	{
	public:
		uint gpuid=0;
		uint cudaid=0;
		QString busid;
		uint gpu_status=0;
		int solver=0;
		int temperature=0;
		uint gpu_power_usage=0;
		uint speed_sps=0;
		uint accepted_shares=0;
		uint rejected_shares=0;
	};
	class JsonApiResponse
	{
	public:
		uint id;
		QString method;
		QJsonObject* error=nullptr;
		QList<Result> result;

		static JsonApiResponse* fromJson(QString json);
	};

	int _benchmarkTimeWait=2*45;
	int _benchmarkReadCount=0;
	double _benchmarkSum=0.0d;
	const QString LookForStart="total speed: ";
	const QString LookForEnd="sol/s";
	const double DevFee=2.0;

	QStringList GetStartCommand(QString url, QString btcAddress, QString worker);
};

#endif
