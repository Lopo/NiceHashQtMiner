#ifndef CONFIGS_CONFIGJSONFILE_MINERSYSTEMVARIABLESCONFIGFILE_H_
#define CONFIGS_CONFIGJSONFILE_MINERSYSTEMVARIABLESCONFIGFILE_H_

#include "Configs/ConfigJsonFile/ConfigFile.h"
#include "Configs/Data/MinerSystemVariablesConfig.h"


class MinerSystemVariablesConfigFile
	: public ConfigFile<MinerSystemVariablesConfig>
{
public:
	MinerSystemVariablesConfigFile();
};

#endif
