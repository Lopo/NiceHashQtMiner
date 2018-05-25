#ifndef CONFIGS_DATA_ALGORITHMCONFIG_H_
#define CONFIGS_DATA_ALGORITHMCONFIG_H_

#include "Enums.h"


class AlgorithmConfig
{
public:
	QString Name=""; // used as an indicator for easier user interaction
	Enums::AlgorithmType NiceHashID=Enums::AlgorithmType::NONE;
	Enums::AlgorithmType SecondaryNiceHashID=Enums::AlgorithmType::NONE;
	Enums::MinerBaseType MinerBaseType=Enums::MinerBaseType::NONE;
	QString MinerName=""; // probably not needed
	double BenchmarkSpeed=0;
	double SecondaryBenchmarkSpeed=0;
	QStringList ExtraLaunchParameters;
	bool Enabled=true;
	int LessThreads=0;
	double PowerUsage=0;

	static AlgorithmConfig* fromJsonObject(QJsonObject o);
	QJsonObject asJsonObject();
};

#endif
