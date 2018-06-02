#ifndef MINERS_XMRSTAK_XMRSTAK_H_
#define MINERS_XMRSTAK_XMRSTAK_H_

#include "Miners/Miner.h"
class IasJsonObject;


class XmrStak
	: public Miner
{
public:
	XmrStak(QString name="");
	ApiData* GetSummaryAsync() override;
	void Start(QString url, QString btcAdress, QString worker) override;

protected:
	void _Stop(Enums::MinerStopType willswitch) override;
	bool IsApiEof(uchar third, uchar second, uchar last) override;

	QStringList BenchmarkCreateCommandLine(Algorithm* algorithm, int time) override;
	void FinishUpBenchmark() override;
	bool BenchmarkParseLine(QString outdata) override;
	void BenchmarkOutputErrorDataReceivedImpl(QString outdata) override;

private:
	const QString ConfigName="config_nh.txt";
	const QString DefConfigName="config.txt";
	const QString DefPoolName="pools.txt";

	int _benchmarkCount;
	double _benchmarkSum;

	QStringList CreateLaunchCommand(QString configName, QMap<Enums::DeviceType, QString> devConfigs);
	QMap<Enums::DeviceType, QString> PrepareConfigFiles(QString url, QString btcAddress, QString worker, bool bench=false);

	QString GetDevConfigFileName(Enums::DeviceType type);
	QString GetBenchConfigName();
	QString GetPoolConfigName();

	QStringList DisableDevCmd(QList<Enums::DeviceType> usedDevs);
	QStringList DisableDevCmd(Enums::DeviceType usedDev);

	template<class T>
	T* ParseJsonFile(Enums::DeviceType type=Enums::DeviceType::CPU, QString filename="", bool fallback=false);
	QString WriteJsonFile(IasJsonObject* config, QString filename, QString genFrom="", bool isNv=false);
	QString WriteJsonFile(IasJsonObject* config, Enums::DeviceType type);
};

#endif
