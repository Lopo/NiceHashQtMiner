#ifndef MINERS_EQUIHASH_NHEQMINER_H_
#define MINERS_EQUIHASH_NHEQMINER_H_

#include "Miners/Equihash/NhEqBase.h"


class NhEqMiner
	: public NhEqBase
{
//	Q_OBJECT
public:
	NhEqMiner();
	void Start(QString url, QString btcAdress, QString worker) override;

protected:
	NiceHashProcess* _Start() override;
	QStringList GetDevicesCommandString() override;
	QProcess* BenchmarkStartProcess(QStringList commandLine) override;
	bool BenchmarkParseLine(QString outdata) override;
};

#endif
