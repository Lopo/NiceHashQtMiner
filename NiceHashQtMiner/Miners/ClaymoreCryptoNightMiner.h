#ifndef MINERS_CLAYMORECRYPTONIGHTMINER_H_
#define MINERS_CLAYMORECRYPTONIGHTMINER_H_

#include "Miners/ClaymoreBaseMiner.h"


class ClaymoreCryptoNightMiner
	: public ClaymoreBaseMiner
{
//	Q_OBJECT
public:
	ClaymoreCryptoNightMiner(bool isOld=false);
	void Start(QString url, QString btcAdress, QString worker) override;

protected:
	double DevFee() override;
	QStringList GetDevicesCommandString() override;
	QStringList BenchmarkCreateCommandLine(Algorithm* algorithm, int time) override;

private:
	bool _isOld;
	static const QString _LookForStart/*="XMR - Total Speed:"*/;
	static const QString LookForStartOld/*="hashrate ="*/;
};

#endif
