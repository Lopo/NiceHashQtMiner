#ifndef MINERS_GROUPING_MININGSETUP_H_
#define MINERS_GROUPING_MININGSETUP_H_

#include "Miners/Grouping/MiningPair.h"
#include "Enums.h"


class MiningSetup
{
public:
	MiningSetup(QList<MiningPair*>* miningPairs);

	QList<MiningPair*>* MiningPairs=nullptr;
	QString MinerPath;
	QString MinerName;
	Enums::AlgorithmType CurrentAlgorithmType;
	Enums::AlgorithmType CurrentSecondaryAlgorithmType;
	bool IsInit=false;
};

#endif
