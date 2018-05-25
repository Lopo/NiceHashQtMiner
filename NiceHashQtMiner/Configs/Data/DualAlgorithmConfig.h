#ifndef CONFIGS_DATA_DUALALGORITHMCONFIG_H_
#define CONFIGS_DATA_DUALALGORITHMCONFIG_H_

#include "Enums.h"
#include <QString>
#include <QMap>

class DualAlgorithmConfig
{
public:
	QString Name=""; // // Used as an indicator for easier user interaction
	Enums::AlgorithmType SecondaryNiceHashID=Enums::AlgorithmType::NONE;
	QMap<int, double> IntensitySpeeds;
	QMap<int, double> SecondaryIntensitySpeeds;
	bool TuningEnabled=true;
	int TuningStart=25;
	int TuningEnd=250;
	int TuningInterval=25;
	QMap<int, double> IntensityPowers;
	bool UseIntensityPowers=false;

	void FixSettingsBounds();
};

#endif
