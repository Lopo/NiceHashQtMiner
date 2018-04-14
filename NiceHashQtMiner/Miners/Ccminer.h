#ifndef MINERS_CCMINER_H_
#define MINERS_CCMINER_H_

#include "Miners/Miner.h"
#include "Grouping/MinerPaths.h"


class Ccminer
	: public Miner
{
	Q_OBJECT
public:
	Ccminer();

	void Start(QString url, QString btcAdress, QString worker) override;
	ApiData* GetSummaryAsync() override;

protected:
	void _Stop(Enums::MinerStopType willswitch) override;
	QStringList BenchmarkCreateCommandLine(Algorithm* algorithm, int time) override;
	bool BenchmarkParseLine(QString outdata) override;
	void BenchmarkOutputErrorDataReceivedImpl(QString outdata) override;

private:
	int _cryptonightTotalCount=0;

	double _cryptonightTotal=0;
	const int _cryptonightTotalDelim=2;

	bool _benchmarkException() const { return (WorkingDirectory()+MinerExeName())==MinerPaths.Data.CcminerCryptonight || (WorkingDirectory()+MinerExeName())==MinerPaths.Data.CcminerKlausT;};
};

#endif
