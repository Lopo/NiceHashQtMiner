#include "Miners/Parsing/ExtraLaunchParametersParser.h"
#include "config.h"
#include "Utils/Helpers.h"
#include "Miners/Grouping/MiningPair.h"
#include "Miners/Parsing/MinerOption.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Miners/Grouping/MiningSetup.h"
#include "Algorithms/Algorithm.h"
#include "Miners/Parsing/MinerOptionPackage.h"
#include "Miners/Parsing/ExtraLaunchParameters.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Algorithms/DualAlgorithm.h"
#include <QMap>
#include <QSet>


const QString ExtraLaunchParametersParser::Tag="ExtraLaunchParametersParser";
const QString ExtraLaunchParametersParser::MinerOptionTypeNone="MinerOptionType_NONE";

bool ExtraLaunchParametersParser::_showLog=true;

bool ExtraLaunchParametersParser::_prevHasIgnoreParam=false;
int ExtraLaunchParametersParser::_logCount=0;


void ExtraLaunchParametersParser::LogParser(QString msg)
{
	if (_showLog) {
		Helpers::ConsolePrint(Tag, msg);
		}
}

// exception ...
int ExtraLaunchParametersParser::GetEqmCudaThreadCount(MiningPair* pair)
{
	if (pair->CurrentExtraLaunchParameters.contains("-ct")) {
		QList<MinerOption*>* eqmCudaOptions=new QList<MinerOption*>({
			new MinerOption("CUDA_Solver_Thread", "-ct", "-ct", "1", Enums::MinerOptionFlagType::MultiParam, " ")
			});
		try {
			return Parse(new QList<MiningPair*> {pair}, eqmCudaOptions).join(' ')
				.trimmed().replace("-ct", "").trimmed().toInt();
			}
		catch (...) {}
		}
	return 1; // default
}

void ExtraLaunchParametersParser::IgnorePrintLogInit()
{
	_prevHasIgnoreParam=false;
	_logCount=0;
}

void ExtraLaunchParametersParser::IgnorePrintLog(QString param, QString ignoreParam, QList<MinerOption*>* ignoreLogOptions/*=nullptr*/)
{
	// AMD temp controll is separated and logs stuff is ignored
	bool printIgnore=true;
	if (ignoreLogOptions!=nullptr) {
		foreach (MinerOption* ignoreOption, *ignoreLogOptions) {
			if (param==ignoreOption->ShortName || param==ignoreOption->LongName) {
				printIgnore=false;
				_prevHasIgnoreParam=true;
				_logCount=0;
				break;
				}
			}
		}
	if (printIgnore && !_prevHasIgnoreParam) {
		LogParser(ignoreParam.arg(param));
		}
	if (_logCount==1) {
		_prevHasIgnoreParam=false;
		_logCount=0;
		}
	++_logCount;
}

QStringList ExtraLaunchParametersParser::Parse(QList<MiningPair*>* miningPairs, QList<MinerOption*>* options, bool useIfDefaults/*=true*/, QList<MinerOption*>* ignoreLogOptions/*=nullptr*/, bool ignoreDcri/*=false*/)
{
	const QString ignoreParam="Cannot parse \"%1\", not supported, set to ignore, or wrong extra launch parameter settings";
	QList<QString>* optionsOrder=new QList<QString>;
	QMap<QString, QMap<QString, QString>*>* cdevOptions=new QMap<QString, QMap<QString, QString>*>;
	QMap<QString, bool>* isOptionDefaults=new QMap<QString, bool>;
	QMap<QString, bool>* isOptionExist=new QMap<QString, bool>;
	// init devs options, and defaults
	foreach (MiningPair* pair, *miningPairs) {
		QMap<QString, QString>* defaults=new QMap<QString, QString>;
		foreach (MinerOption* option, *options) {
			defaults->insert(option->Type, option->Default);
			}
		cdevOptions->insert(pair->Device->Uuid(), defaults);
		}
	// init order and params flags, and params list
	foreach (MinerOption* option, *options) {
		QString optionType=option->Type;
		optionsOrder->append(optionType);

		isOptionDefaults->insert(optionType, true);
		isOptionExist->insert(optionType, false);
		}
	// parse
	foreach (MiningPair* pair, *miningPairs) {
		LogParser("ExtraLaunch params \""+pair->CurrentExtraLaunchParameters.join(' ')+"\" for device UUID "+pair->Device->Uuid());
		QStringList parameters=pair->CurrentExtraLaunchParameters.join(' ').replace("=", "= ").split(' ');

		IgnorePrintLogInit();

		QString currentFlag=MinerOptionTypeNone;
		bool ignoringNextOption=false;
		foreach (QString param, parameters) {
			if (param=="") {
				continue;
				}
			if (currentFlag==MinerOptionTypeNone) {
				bool isIngored=true;
				foreach (MinerOption* option, *options) {
					if (param==option->ShortName || param==option->LongName) {
						isIngored=false;
						if (ignoreDcri && option->Type=="ClaymoreDual_dcri") {
							Helpers::ConsolePrint("CDTUNING", "Disabling dcri extra launch param");
							ignoringNextOption=true;
							}
						else {
							if (option->FlagType==Enums::MinerOptionFlagType::Uni) {
								isOptionExist->insert(option->Type, true);
								(*cdevOptions->value(pair->Device->Uuid()))[option->Type]="notNull"; // if Uni param is null it is not present
								}
							else { // Single and Multi param
								currentFlag=option->Type;
								}
							}
						}
					}
				if (isIngored) { // ignored
					if (ignoringNextOption) {
						// This is a paramater for an ignored option, silently ignore it
						ignoringNextOption=false;
						}
					else {
						IgnorePrintLog(param, ignoreParam, ignoreLogOptions);
						}
					}
				}
			else if (currentFlag!=MinerOptionTypeNone) {
				isOptionExist->insert(currentFlag, true);
				(*(*cdevOptions)[pair->Device->Uuid()])[currentFlag]=param;
				currentFlag=MinerOptionTypeNone;
				}
			else { // problem
				IgnorePrintLog(param, ignoreParam, ignoreLogOptions);
				}
			}
		}

	QStringList retVal;

	// check if is all defaults
	bool isAllDefault=true;
	foreach (MiningPair* pair, *miningPairs) {
		foreach (MinerOption* option, *options) {
			if (option->Default!=cdevOptions->value(pair->Device->Uuid())->value(option->Type)) {
				isAllDefault=false;
				isOptionDefaults->insert(option->Type, false);
				}
			}
		}

	if (!isAllDefault || useIfDefaults) {
		foreach (MinerOption* option, *options) {
			if (isOptionDefaults->value(option->Type) || !isOptionExist->value(option->Type) || !useIfDefaults) {
				continue;
				}
			switch (option->FlagType) {
				case Enums::MinerOptionFlagType::Uni:
					{ // uni params if one exist use or all must exist?
					bool isOptionInUse=false;
					foreach (MiningPair* pair, *miningPairs) {
						if (cdevOptions->value(pair->Device->Uuid())->value(option->Type, nullptr)!=nullptr) {
							isOptionInUse=true;
							break;
							}
						}
					if (isOptionInUse) {
						retVal << option->LongName;
						}
					}
					break;
				case Enums::MinerOptionFlagType::MultiParam:
					{
					QStringList* values=new QStringList;
					foreach (MiningPair* pair, *miningPairs) {
						values->append(cdevOptions->value(pair->Device->Uuid())->value(option->Type));
						}
					retVal << (option->LongName.contains('=')? QStringList({option->LongName+values->join(option->Separator)}) : QStringList({option->LongName, values->join(option->Separator)}));
					}
					break;
				case Enums::MinerOptionFlagType::SingleParam:
					{
					QSet<QString>* values=new QSet<QString>;
					foreach (MiningPair* pair, *miningPairs) {
						values->insert(cdevOptions->value(pair->Device->Uuid())->value(option->Type));
						}
					QString setValue=option->Default;
					if (values->count()>=1) {
						// Always take first
						setValue=values->values().first();
						}
					retVal << (option->LongName.contains('=')? QStringList({option->LongName+setValue}) : QStringList({option->LongName, setValue}));
					}
					break;
				case Enums::MinerOptionFlagType::DuplicateMultiParam:
					{
					QStringList values;
					foreach (MiningPair* pair, *miningPairs) {
						values << option->LongName << cdevOptions->value(pair->Device->Uuid())->value(option->Type);
						}
					retVal << values;
					}
					break;
				}
			}
		}

	LogParser("Final extra launch params parse \""+retVal.join(' ')+"\"");

	return retVal;
}

QStringList ExtraLaunchParametersParser::ParseForMiningSetup(MiningSetup* miningSetup, Enums::DeviceType deviceType, bool showLog/*=true*/)
{
	return ParseForMiningPairs(miningSetup->MiningPairs, deviceType, showLog);
}

QStringList ExtraLaunchParametersParser::ParseForMiningPair(MiningPair* miningPair, Enums::AlgorithmType algorithmType, Enums::DeviceType deviceType, bool showLog/*=true*/)
{
	return ParseForMiningPairs(new QList<MiningPair*>({miningPair}), deviceType, showLog);
}

Enums::MinerType ExtraLaunchParametersParser::GetMinerType(Enums::DeviceType deviceType, Enums::MinerBaseType minerBaseType, Enums::AlgorithmType algorithmType)
{
	switch (minerBaseType) {
		case Enums::MinerBaseType::OptiminerAMD:
			return Enums::MinerType::OptiminerZcash;
		case Enums::MinerBaseType::sgminer:
			return Enums::MinerType::sgminer;
		case Enums::MinerBaseType::ccminer:
		case Enums::MinerBaseType::ccminer_alexis:
		case Enums::MinerBaseType::experimental:
			if (algorithmType==Enums::AlgorithmType::CryptoNight) {
				return Enums::MinerType::ccminer_CryptoNight;
				}
			return Enums::MinerType::ccminer;
		case Enums::MinerBaseType::Claymore:
			switch (algorithmType) {
				case Enums::AlgorithmType::CryptoNight:
				case Enums::AlgorithmType::CryptoNightV7:
					return Enums::MinerType::ClaymoreCryptoNight;
				case Enums::AlgorithmType::Equihash:
					return Enums::MinerType::ClaymoreZcash;
				case Enums::AlgorithmType::DaggerHashimoto:
					return Enums::MinerType::ClaymoreDual;
				default:
					break;
				}
			break;
		case Enums::MinerBaseType::Claymore_old:
			if (algorithmType==Enums::AlgorithmType::CryptoNight) {
				return Enums::MinerType::ClaymoreCryptoNight;
				}
			break;
		case Enums::MinerBaseType::ethminer:
#if WITH_AMD
			if (deviceType==Enums::DeviceType::AMD) {
				return Enums::MinerType::ethminer_OCL;
				}
#endif
#if WITH_NVIDIA
			if (deviceType==Enums::DeviceType::NVIDIA) {
				return Enums::MinerType::ethminer_CUDA;
				}
#endif
			break;
		case Enums::MinerBaseType::nheqminer:
			switch (deviceType) {
				case Enums::DeviceType::CPU:
					return Enums::MinerType::nheqminer_CPU;
#if WITH_AMD
				case Enums::DeviceType::AMD:
					return Enums::MinerType::nheqminer_AMD;
#endif
#if WITH_NVIDIA
				case Enums::DeviceType::NVIDIA:
					return Enums::MinerType::eqm_CUDA;
#endif
				default:
					break;
				}
			break;
		case Enums::MinerBaseType::eqm:
			if (deviceType==Enums::DeviceType::CPU) {
				return Enums::MinerType::eqm_CPU;
				}
#if WITH_NVIDIA
			if (deviceType==Enums::DeviceType::NVIDIA) {
				return Enums::MinerType::eqm_CUDA;
				}
#endif
			break;
		case Enums::MinerBaseType::excavator:
			return Enums::MinerType::excavator;
		case Enums::MinerBaseType::EWBF:
			return Enums::MinerType::EWBF;
		case Enums::MinerBaseType::Xmrig:
			return Enums::MinerType::Xmrig;
		case Enums::MinerBaseType::dtsm:
			return Enums::MinerType::dtsm;
		default:
			return Enums::MinerType::NONE;
		}

	return Enums::MinerType::NONE;
}

QStringList ExtraLaunchParametersParser::ParseForMiningPairs(const QList<MiningPair*>* miningPairs, Enums::DeviceType deviceType, bool showLog/*=true*/)
{
	_showLog=showLog;

	Enums::MinerBaseType minerBaseType=Enums::MinerBaseType::NONE;
	Enums::AlgorithmType algorithmType=Enums::AlgorithmType::NONE;
	bool ignoreDcri=false;
	if (miningPairs->count()>0) {
		Algorithm* algo=miningPairs->at(0)->algorithm;
		if (algo!=nullptr) {
			algorithmType=algo->NiceHashID;
			minerBaseType=algo->MinerBaseType;
			DualAlgorithm* dualAlgo=qobject_cast<DualAlgorithm*>(algo);
			if (dualAlgo!=nullptr && dualAlgo->TuningEnabled) {
				ignoreDcri=true;
				}
			}
		}

	Enums::MinerType minerType=GetMinerType(deviceType, minerBaseType, algorithmType);

	MinerOptionPackage* minerOptionPackage=ExtraLaunchParameters::GetMinerOptionPackageForMinerType(minerType);

	QList<MiningPair*>* setMiningPairs=new QList<MiningPair*>; // !!! copy
	foreach (MiningPair* mPair, *miningPairs) {
		setMiningPairs->push_back(new MiningPair((*mPair)));
		}
	// handle exceptions and package parsing
	// CPU exception
	if (deviceType==Enums::DeviceType::CPU && minerType!=Enums::MinerType::Xmrig) {
		CheckAndSetCpuPairs(setMiningPairs);
		}
	//ethminer exception
	if (minerType==Enums::MinerType::ethminer_OCL || minerType==Enums::MinerType::ethminer_CUDA) {
		// use if missing compute device for correct mapping
		// init fakes workaround
		QList<MiningPair*>* cDevsMappings=new QList<MiningPair*>;
		{
			int id=-1;
			Algorithm* fakeAlgo=new Algorithm(Enums::MinerBaseType::ethminer, Enums::AlgorithmType::DaggerHashimoto, "daggerhashimoto");
			foreach (MiningPair* pair, *setMiningPairs) {
				while (++id!=pair->Device->ID) {
					ComputeDevice* fakeCdev=new ComputeDevice(id);
					cDevsMappings->append(new MiningPair(fakeCdev, fakeAlgo));
					}
				cDevsMappings->append(pair);
				}
		}
		// reset setMiningPairs
		setMiningPairs=cDevsMappings;
		}
	// sgminer exception handle intensity types
	if (minerType==Enums::MinerType::sgminer) {
		// rawIntensity overrides xintensity, xintensity overrides intensity
		QList<MinerOption*>* sgminerIntensities=new QList<MinerOption*>({
			new MinerOption("Intensity", "-I", "--intensity", "d", Enums::MinerOptionFlagType::MultiParam, ","), // default is "d" check if -1 works
			new MinerOption("Xintensity", "-X", "--xintensity", "-1", Enums::MinerOptionFlagType::MultiParam, ","), // default none
			new MinerOption("Rawintensity", "", "--rawintensity", "-1", Enums::MinerOptionFlagType::MultiParam, ",") // default none
			});
		QMap<QString, bool>* containsIntensity=new QMap<QString, bool>({
			{"Intensity", false},
			{"Xintensity", false},
			{"Rawintensity", false}
			});
		// check intensity and xintensity, the latter overrides so change accordingly
		foreach (MiningPair* cDev, *setMiningPairs) {
			foreach (MinerOption* intensityOption, *sgminerIntensities) {
				if (!intensityOption->ShortName.isEmpty() && cDev->CurrentExtraLaunchParameters.contains(intensityOption->ShortName)) {
					cDev->CurrentExtraLaunchParameters=cDev->CurrentExtraLaunchParameters.replaceInStrings(intensityOption->ShortName, intensityOption->LongName);
					containsIntensity->insert(intensityOption->Type, true);
					}
				if (cDev->CurrentExtraLaunchParameters.contains(intensityOption->LongName)) {
					containsIntensity->insert(intensityOption->Type, true);
					}
				}
			}
		// replace
		if (containsIntensity->value("Intensity") && containsIntensity->value("Xintensity")) {
			LogParser("Sgminer replacing --intensity with --xintensity");
			foreach (MiningPair* cDev, *setMiningPairs) {
				cDev->CurrentExtraLaunchParameters=cDev->CurrentExtraLaunchParameters.replaceInStrings("--intensity", "--xintensity");
				}
			}
		if (containsIntensity->value("Xintensity") && containsIntensity->value("Rawintensity")) {
			LogParser("Sgminer replacing --xintensity with --rawintensity");
			foreach (MiningPair* cDev, *setMiningPairs) {
				cDev->CurrentExtraLaunchParameters=cDev->CurrentExtraLaunchParameters.replaceInStrings("--xintensity", "--rawintensity");
				}
			}
		}

	QStringList ret;
	QStringList general=Parse(setMiningPairs, minerOptionPackage->GeneralOptions, false, minerOptionPackage->TemperatureOptions, ignoreDcri);
#if WITH_AMD
	// temp control and parse
	if (ConfigManager.generalConfig->DisableAMDTempControl) {
		LogParser("DisableAMDTempControl is TRUE, temp control parameters will be ignored");
		ret=general;
		}
	else {
		LogParser("AMD parsing temperature control parameters");
		QStringList temp=Parse(setMiningPairs, minerOptionPackage->TemperatureOptions, false, minerOptionPackage->GeneralOptions, ignoreDcri);

		ret=general+temp;
		}
#else
	ret=general;
#endif
	return ret;
}

void ExtraLaunchParametersParser::CheckAndSetCpuPairs(QList<MiningPair*>* miningPairs)
{
	foreach (MiningPair* pair, *miningPairs) {
		ComputeDevice* cDev=pair->Device;
		// extra thread check
		if (pair->CurrentExtraLaunchParameters.join("").contains("--threads=") || pair->CurrentExtraLaunchParameters.contains("-t")) {
			// nothing
			}
		else {
			pair->CurrentExtraLaunchParameters << "-t" << QString::number(GetThreads(cDev->Threads(), pair->algorithm->LessThreads));
			}
		}
}

int ExtraLaunchParametersParser::GetThreadsNumber(MiningPair* cpuPair)
{
	ComputeDevice* cDev=cpuPair->Device;
	Algorithm* algo=cpuPair->algorithm;
	// extra thread check
	if (algo->ExtraLaunchParameters.join("").contains("--threads=") || algo->ExtraLaunchParameters.contains("-t")) {
		QStringList strings=algo->ExtraLaunchParameters;
		int i=-1;
		for (int curI=0; curI<strings.length(); ++curI) {
			if (strings[curI]=="--threads=" || strings[curI]=="-t") {
				i=curI+1;
				break;
				}
			}
		if (i>-1 && strings.length()>i) {
			int numTr=cDev->Threads();
			bool ok;
			int p=strings[i].toInt(&ok);
			if (ok) {numTr=p;}
			if (numTr<=cDev->Threads()) {
				return numTr;
				}
			}
		}
	return GetThreads(cDev->Threads(), cpuPair->algorithm->LessThreads);
}

int ExtraLaunchParametersParser::GetThreads(int threads, int lessThreads)
{
	if (threads>lessThreads) {
		return threads-lessThreads;
		}
	return threads;
}

bool ExtraLaunchParametersParser::GetNoPrefetch(MiningPair* cpuPair)
{
	return cpuPair->algorithm->ExtraLaunchParameters.contains("--no_prefetch");
}

QList<int>* ExtraLaunchParametersParser::GetIntensityStak(MiningPair* pair)
{
	Algorithm* algo=pair->algorithm;
	QList<int>* intensities=new QList<int>;
	if (algo->ExtraLaunchParameters.contains("--intensity")) {
		QStringList strings=algo->ExtraLaunchParameters;
		int i=strings.indexOf("--intensity")+1;
		if (i>-1 && strings.count()>i) {
			QStringList intStrings=strings[i].split(',', QString::SplitBehavior::SkipEmptyParts);
			foreach (QString intString, intStrings) {
				bool ok=false;
				int is=intString.toInt(&ok);
				if (ok) {
					intensities->append(is);
					}
				}
			}
		}
	return intensities;
}
