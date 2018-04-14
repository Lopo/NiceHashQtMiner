#ifndef CONFIGS_CONFIGJSONFILE_DEVICEBENCHMARKCONFIGFILE_H_
#define CONFIGS_CONFIGJSONFILE_DEVICEBENCHMARKCONFIGFILE_H_

#include "Configs/ConfigJsonFile/ConfigFile.h"
#include "Configs/Data/DeviceBenchmarkConfig.h"


class DeviceBenchmarkConfigFile
	: public ConfigFile<DeviceBenchmarkConfig>
{
public:
	DeviceBenchmarkConfigFile(QString deviceUuid);

private:
	static const QString BenchmarkPrefix;

	static QString GetName(QString deviceUuid, QString old="");
};

#endif
