#ifndef MINERS_MINERSMANAGER_H_
#define MINERS_MINERSMANAGER_H_

#include "Enums.h"

class NiceHashSma;
class IMainFormRatesComunication;
class MiningSession;


class MinersManager
{
public:
	static void StopAllMiners();
	static void StopAllMinersNonProfitable();
	static QString GetActiveMinersGroup();
	static QList<int>* GetActiveMinersIndexes();
	static double GetTotalRate();
	static bool StartInitialize(IMainFormRatesComunication* mainFormRatesCommunication, QString miningLocation, QString worker, QString btcAddress);
	static bool IsMiningEnabled();
//	static void SwitchMostProfitableGroupUpMethod();
	static void MinerStatsCheck();

private:
	static MiningSession* _curMiningSession;
};

#endif
