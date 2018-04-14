#ifndef CONFIGS_DATA_DEVICEBENCHMARKCONFIG_H_
#define CONFIGS_DATA_DEVICEBENCHMARKCONFIG_H_

#include "Configs/Data/AlgorithmConfig.h"
#include <QList>
#include <QJsonDocument>


class DeviceBenchmarkConfig
{
public:
	QString DeviceUUID="";
	QString DeviceName="";
	QList<AlgorithmConfig*>* AlgorithmSettings=new QList<AlgorithmConfig*>;

	static DeviceBenchmarkConfig* fromJson(QString json);
	QString asJson(QJsonDocument::JsonFormat format=QJsonDocument::JsonFormat::Indented);
};

#endif
