#ifndef MINERS_XMRSTAK_XMRSTAKAMD_H_
#define MINERS_XMRSTAK_XMRSTAKAMD_H_

#include "Miners/XmrStak/XmrStakConfig.h"
#include "Miners/XmrStak/XmrStak.h"



class XmrStakGPUSettings
{
public:
	XmrStakGPUSettings(int index, int intensity, int worksize=8, bool affine_to_cpu=false);

	int index=0;
	int intensity=1000;
	int worksize=8;
	bool affine_to_cpu=false;

	QJsonObject toJsonObject();
};


class XmrStakAMDConfig
	: public XmrStakConfig
{
public:
	XmrStakAMDConfig(QString poolAddr, QString wallet, int port);
	void Initialize_gpu_threads_conf(QList<XmrStakGPUSettings*>* gpuSettings);

	int gpu_thread_num=6;
	QList<QJsonObject>* gpu_threads_conf=nullptr;
	int platform_index=0;

	QString toJsonString();
};


class XmrStakAMD
	: public XmrStak
{
public:
	XmrStakAMD();
	ApiData* GetSummaryAsync() override;

protected:
	void prepareConfigFile(QString pool, QString wallet) override;

};

#endif
