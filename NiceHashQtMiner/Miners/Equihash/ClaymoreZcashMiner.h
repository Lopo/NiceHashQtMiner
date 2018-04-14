#ifndef MINERS_EQUIHASH_CLAYMOREZCASHMINER_H_
#define MINERS_EQUIHASH_CLAYMOREZCASHMINER_H_

#include "Miners/ClaymoreBaseMiner.h"


class ClaymoreZcashMiner
	: public ClaymoreBaseMiner
{
public:
	ClaymoreZcashMiner();
	void Start(QString url, QString btcAdress, QString worker) override;

protected:
	double DevFee() override;
	QStringList BenchmarkCreateCommandLine(Algorithm* algorithm, int time) override;

private:
	const static QString LookForStart/*="ZEC - Total Speed:"*/;
};

#endif
