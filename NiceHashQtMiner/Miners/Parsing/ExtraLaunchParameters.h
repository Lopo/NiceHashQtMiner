#ifndef MINERS_PARSING_EXTRALAUNCHPARAMETERS_H_
#define MINERS_PARSING_EXTRALAUNCHPARAMETERS_H_

#include "Configs/ConfigJsonFile/ConfigFile.h"
#include "Miners/Parsing/MinerOptionPackage.h"


class MinerOptionPackageFile
	: public ConfigFile<MinerOptionPackage>
{
public:
	MinerOptionPackageFile(QString name);
};


class ExtraLaunchParameters
{
private:
	static QList<MinerOptionPackage*> Defaults;
	static QList<MinerOptionPackage*>* MinerOptionPackages;

public:
	static void InitializePackages();
	static MinerOptionPackage* GetMinerOptionPackageForMinerType(Enums::MinerType type);
};

#ifdef __ExtraLaunchParameters_cpp__
	ExtraLaunchParameters ExtraLaunchParameters;
#else
	extern ExtraLaunchParameters ExtraLaunchParameters;
#endif

#endif
