#ifndef MINERS_PARSING_EXTRALAUNCHPARAMETERSPARSER_H_
#define MINERS_PARSING_EXTRALAUNCHPARAMETERSPARSER_H_

#include "Enums.h"
class MinerOption;
class MiningPair;
class MiningSetup;


class ExtraLaunchParametersParser
{
private:
	static const QString Tag/*="ExtraLaunchParametersParser"*/;
	static const QString MinerOptionTypeNone/*="MinerOptionType_NONE"*/;
	static bool _showLog;

	static void LogParser(QString msg);

	static bool _prevHasIgnoreParam;
	static int _logCount;

	static void IgnorePrintLogInit();
	static void IgnorePrintLog(QString param, QString ignoreParam, QList<MinerOption*>* ignoreLogOptions=nullptr);
	static QStringList Parse(QList<MiningPair*>* miningPairs, QList<MinerOption*>* options, bool useIfDefaults=true, QList<MinerOption*>* ignoreLogOptions=nullptr, bool ignoreDcri=false);

	static Enums::MinerType GetMinerType(Enums::DeviceType deviceType, Enums::MinerBaseType minerBaseType, Enums::AlgorithmType algorithmType);

	static void CheckAndSetCpuPairs(QList<MiningPair*>* miningPairs);

	static int GetThreads(int threads, int lessThreads);

public:
	static int GetEqmCudaThreadCount(MiningPair* pair);

	static QStringList ParseForMiningSetup(MiningSetup* miningSetup, Enums::DeviceType deviceType, bool showLog=true);
	static QStringList ParseForMiningPair(MiningPair* miningPair, Enums::AlgorithmType algorithmType, Enums::DeviceType deviceType, bool showLog=true);

	static QStringList ParseForMiningPairs(const QList<MiningPair*>* miningPairs, Enums::DeviceType deviceType, bool showLog=true);

	static int GetThreadsNumber(MiningPair* cpuPair);

	static bool GetNoPrefetch(MiningPair* cpuPair);
	static QList<int>* GetIntensityStak(MiningPair* pair);
};

#endif
