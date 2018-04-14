#ifndef CONFIGS_CONFIGJSONFILE_GENERALCONFIGFILE_H_
#define CONFIGS_CONFIGJSONFILE_GENERALCONFIGFILE_H_

#include "Configs/ConfigJsonFile/ConfigFile.h"
#include "Configs/Data/GeneralConfig.h"


class GeneralConfigFile
	: public ConfigFile<GeneralConfig>
{
public:
	GeneralConfigFile();
};

#endif
