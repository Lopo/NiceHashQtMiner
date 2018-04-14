#ifndef MINERS_EQUIHASH_OPTIMINERZCASHMINER_H_
#define MINERS_EQUIHASH_OPTIMINERZCASHMINER_H_

#include "Miners/Miner.h"
#include <QMap>


class OptiminerZcashMiner
	: public Miner
{
//	Q_OBJECT
public:
	OptiminerZcashMiner();

private:
	class Stratum
	{
	public:
		QString target;
		bool connected;
		int connection_failures;
		QString host;
		int port;
	};

	class JsonApiResponse
	{
	public:
		double uptime;
		QMap<QString, QMap<QString, double>*>* solution_rate;
		QMap<QString, double> share;
		QMap<QString, QMap<QString, double>> iteration_rate;
		Stratum stratum;

		static JsonApiResponse* fromJson(QString str);
	};

	QElapsedTimer* _startApi=nullptr; // give some time or else it will crash

	bool _skipApiCheck=true;
	int _waitSeconds=30;

	void Start(QString url, QString btcAdress, QString worker) override;
	ApiData* GetSummaryAsync() override;

protected:
	void _Stop(Enums::MinerStopType willswitch) override;
	QStringList GetDevicesCommandString() override;
	QStringList BenchmarkCreateCommandLine(Algorithm* algorithm, int time) override;
	void BenchmarkOutputErrorDataReceivedImpl(QString outdata) override;
	bool BenchmarkParseLine(QString outdata) override;
};

#endif
