#include "Configs/ConfigJsonFile/GeneralConfigFile.h"
#include "Configs/ConfigJsonFile/FOLDERS.h"


GeneralConfigFile::GeneralConfigFile()
	: ConfigFile(Folders::Config, "General.json", "General_old.json")
{
}
