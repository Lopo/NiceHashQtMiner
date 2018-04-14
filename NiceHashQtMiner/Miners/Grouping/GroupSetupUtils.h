#ifndef MINERS_GROUPING_GROUPSETUPUTILS_H_
#define MINERS_GROUPING_GROUPSETUPUTILS_H_

#include "Enums.h"
#include <QMap>
#include <tuple>
class Algorithm;
class ComputeDevice;
class MiningDevice;


class GroupSetupUtils
{
public:
	static bool IsAlgoMiningCapable(Algorithm* algo);
	static std::tuple<ComputeDevice*, Enums::DeviceMiningStatus>* GetDeviceMiningStatus(ComputeDevice* device);
	static QList<MiningDevice*>* GetMiningDevices(QList<ComputeDevice*>* devices, bool log);
	static void AvarageSpeeds(QList<MiningDevice*>* miningDevs);

private:
	static const QString Tag;
	static std::tuple<QList<MiningDevice*>*, QList<std::tuple<ComputeDevice*, Enums::DeviceMiningStatus>*>*>* GetMiningAndNonMiningDevices(QList<ComputeDevice*>* devices);
	static QString GetDisabledDeviceStatusString(std::tuple<ComputeDevice*, Enums::DeviceMiningStatus>* devStatusTuple);
	static void LogMiningNonMiningStatuses(QList<MiningDevice*>* enabledDevices, QList<std::tuple<ComputeDevice*, Enums::DeviceMiningStatus>*>* disabledDevicesStatuses);
};


class SpeedSumCount
{
public:
	double Speed=0;
	double SecondarySpeed=0;
	int Count=0;
	double GetAvarage();
	double GetSecondaryAvarage();
};


class AvaragerGroup
{
public:
	QString DeviceName;
	QStringList* UuidList=new QStringList;
	// algo_id, speed_sum, speed_count
	QMap<QString, SpeedSumCount*>* BenchmarkSums=new QMap<QString, SpeedSumCount*>;
	QMap<QString, QList<double>*>* CalculateAverages();
	void AddAlgorithms(QList<Algorithm*>* algos);
};

#endif
