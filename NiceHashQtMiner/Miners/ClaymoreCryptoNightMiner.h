#ifndef MINERS_CLAYMORECRYPTONIGHTMINER_H_
#define MINERS_CLAYMORECRYPTONIGHTMINER_H_

#include "Miners/ClaymoreBaseMiner.h"


class ClaymoreCryptoNightMiner
	: public ClaymoreBaseMiner
{
//	Q_OBJECT
public:
	ClaymoreCryptoNightMiner();
	void Start(QString url, QString btcAdress, QString worker) override;

protected:
	QStringList BenchmarkCreateCommandLine(Algorithm* algorithm, int time) override;
};

#endif
