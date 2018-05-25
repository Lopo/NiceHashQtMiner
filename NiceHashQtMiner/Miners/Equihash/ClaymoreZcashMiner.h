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
	QStringList BenchmarkCreateCommandLine(Algorithm* algorithm, int time) override;
};

#endif
