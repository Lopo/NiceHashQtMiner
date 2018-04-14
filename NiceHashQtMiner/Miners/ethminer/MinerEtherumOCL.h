#ifndef MINERS_ETHMINER_MINERETHERUMOCL_H_
#define MINERS_ETHMINER_MINERETHERUMOCL_H_

#include "MinerEtherum.h"


class MinerEtherumOCL
	: public MinerEtherum
{
public:
	MinerEtherumOCL();
	~MinerEtherumOCL();
	void Start(QString url, QString btcAdress, QString worker) override;

protected:
	QStringList GetStartCommandStringPart(QString url, QString username) override;
	QStringList GetBenchmarkCommandStringPart(Algorithm* algorithm) override;

private:
	// reference to all MinerEtherumOCL make sure to clear this after miner Stop
	// we make sure only ONE instance of MinerEtherumOCL is running
	static QList<MinerEtherum*>* MinerEtherumOclList;

	int _gpuPlatformNumber;
};

#endif
