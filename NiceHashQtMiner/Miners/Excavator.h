#ifndef MINERS_EXCAVATOR_H_
#define MINERS_EXCAVATOR_H_

#include "Miners/Miner.h"


class ExcavatorCommand
{
public:
	ExcavatorCommand(QString iMethod, QStringList iParams);

	int id=1;
	QString method;
	QStringList params;

	QJsonObject asJObject();
};

class ExcavatorAction
{
public:
	ExcavatorAction(int iTime=0, int iLoop=0);

	int time=0;
	int loop=0;
	QList<ExcavatorCommand> commands;

	QJsonObject asJObject();
};


class Excavator
	: public Miner
{
	Q_OBJECT
public:
	Excavator(Enums::AlgorithmType secondaryAlgorithmType=Enums::AlgorithmType::NONE);
	bool IsDual();
	void Start(QString url, QString btcAdress, QString worker) override;
	ApiData* GetSummaryAsync() override;

protected:
	QStringList BenchmarkCreateCommandLine(Algorithm* algorithm, int time) override;
	bool BenchmarkParseLine(QString outdata) override;
	void _Stop(Enums::MinerStopType willswitch) override;
	void BenchmarkOutputErrorDataReceivedImpl(QString outdata) override;

	Enums::AlgorithmType SecondaryAlgorithmType=Enums::AlgorithmType::NONE;

private:
	class Worker
	{
	public:
		int worker_id;
		QList<double> speed;
	};

	class Algorithm
	{
	public:
		int algorithm_id;
		QList<Worker> workers;
	};

	class JsonApiResponse
	{
	public:
		int id;
		QList<Algorithm> algorithms;
		QJsonObject* error=nullptr;

		static JsonApiResponse* fromJson(QString json);
	};

	const QString totSpeed="total speed:";

	int _benchmarkTimeWait=0;
	QString jsonConfig(QString url, QString btcAdress, QString worker);
	QString jsonBenchConfig(QString url, QString btcAdress, QString worker);
	QString GetConfigFileName();
	double GetNumber(QString speed);
};

#endif
