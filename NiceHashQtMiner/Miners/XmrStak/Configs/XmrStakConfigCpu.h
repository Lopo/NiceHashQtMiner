#ifndef MINERS_XMRSTAK_CONFIGS_XMRSTAKCONFIGCPU_H_
#define MINERS_XMRSTAK_CONFIGS_XMRSTAKCONFIGCPU_H_

#include "Miners/XmrStak/Configs/IasJsonObject.h"
#include <QList>
#include <QJsonObject>


class XmrStakConfigCpu
	: public IasJsonObject
{
public:
	XmrStakConfigCpu(int numberOfthreads);
	void Inti_cpu_threads_conf(bool low_power_mode, bool no_prefetch, bool affine_to_cpu, bool isHyperThreading);

	QList<QJsonObject> cpu_threads_conf;

	QJsonObject asJsonObject() override;
	static XmrStakConfigCpu* fromJson(QString json);

private:
	int cpu_thread_num;
};

#endif
