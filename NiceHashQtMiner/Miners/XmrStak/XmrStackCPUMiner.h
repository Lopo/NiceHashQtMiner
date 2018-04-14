#ifndef MINERS_XMRSTAK_XMRSTACKCPUMINER_H_
#define MINERS_XMRSTAK_XMRSTACKCPUMINER_H_

#include "Miners/XmrStak/XmrStakConfig.h"
#include "Miners/XmrStak/XmrStak.h"


class XmrStackCPUMinerConfig
	: public XmrStakConfig
{
public:
	XmrStackCPUMinerConfig(int numberOfthreads, QString poolAddr, QString wallet, int port);
	void Inti_cpu_threads_conf(bool low_power_mode, bool no_prefetch, bool affine_to_cpu, bool isHyperThreading);

	int cpu_thread_num=0;
	QList<QJsonObject>* cpu_threads_conf=nullptr;
	QString use_slow_memory="warn";
	bool nicehash_nonce=true;
	bool* aes_override=nullptr;

	QString currency="cryptonight";

	QString toJsonString();
};

class XmrStackCPUMiner
	: public XmrStak
{
	Q_OBJECT
public:
	XmrStackCPUMiner();

protected:
	void prepareConfigFile(QString pool, QString wallet) override;
	NiceHashProcess* _Start() override;
	QProcess* BenchmarkStartProcess(QStringList CommandLine) override;
};

#endif
