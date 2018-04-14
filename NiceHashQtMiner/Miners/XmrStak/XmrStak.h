#ifndef MINERS_XMRSTAK_XMRSTAK_H_
#define MINERS_XMRSTAK_XMRSTAK_H_

#include "Miners/Miner.h"


class XmrStak
	: public Miner
{
public:
	ApiData* GetSummaryAsync() override;
	void Start(QString url, QString btcAdress, QString worker) override;

protected:
	XmrStak(QString name, int maxCDTime);
	virtual void prepareConfigFile(QString pool, QString wallet)=0;
	QString GetConfigFileName();
	void _Stop(Enums::MinerStopType willswitch) override;
	bool IsApiEof(uchar third, uchar second, uchar last) override;
	QStringList BenchmarkCreateCommandLine(Algorithm* algorithm, int time) override;
	bool BenchmarkParseLine(QString outdata) override;
	void BenchmarkOutputErrorDataReceivedImpl(QString outdata) override;
};

#endif
