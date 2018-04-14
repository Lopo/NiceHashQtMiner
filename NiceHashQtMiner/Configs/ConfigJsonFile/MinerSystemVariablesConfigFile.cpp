#include "Configs/ConfigJsonFile/MinerSystemVariablesConfigFile.h"
#include "Configs/ConfigJsonFile/FOLDERS.h"


MinerSystemVariablesConfigFile::MinerSystemVariablesConfigFile()
	: ConfigFile(Folders::Config, "MinerSystemVariables.json", "MinerSystemVariables_old.json")
{
}
