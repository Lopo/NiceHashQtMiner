#ifndef MINERS_ETHMINER_MINERETHERUMCUDA_H_
#define MINERS_ETHMINER_MINERETHERUMCUDA_H_

#include "MinerEtherum.h"


class MinerEtherumCUDA
	: public MinerEtherum
{
public:
	MinerEtherumCUDA();
	~MinerEtherumCUDA();
	void Start(QString url, QString btcAdress, QString worker) override;

protected:
	QStringList GetStartCommandStringPart(QString url, QString username) override;
	QStringList GetBenchmarkCommandStringPart(Algorithm* algorithm) override;

private:
	// reference to all MinerEtherumCUDA make sure to clear this after miner Stop
	// we make sure only ONE instance of MinerEtherumCUDA is running
	static QList<MinerEtherum*>* MinerEtherumCudaList;
};

#endif
