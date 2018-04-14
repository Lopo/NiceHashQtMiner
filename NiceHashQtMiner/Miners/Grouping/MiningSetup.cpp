#include "Miners/Grouping/MiningSetup.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Algorithm.h"
#include "Miners/Grouping/MinerPaths.h"


MiningSetup::MiningSetup(QList<MiningPair*>* miningPairs)
{
	IsInit=false;
	CurrentAlgorithmType=Enums::AlgorithmType::NONE;
	CurrentSecondaryAlgorithmType=Enums::AlgorithmType::NONE;
	if (miningPairs==nullptr || miningPairs->count()<=0) {
		return;
		}
	MiningPairs=miningPairs;
	std::sort(miningPairs->begin(), miningPairs->end(), [](MiningPair* a, MiningPair* b) { return a->Device->ID<b->Device->ID; });
	MinerName=miningPairs->at(0)->algorithm->MinerName;
	CurrentAlgorithmType=miningPairs->at(0)->algorithm->NiceHashID;
	CurrentSecondaryAlgorithmType=miningPairs->at(0)->algorithm->SecondaryNiceHashID;
	MinerPath=miningPairs->at(0)->algorithm->MinerBinaryPath;
	IsInit=MinerPaths::IsValidMinerPath(&MinerPath);
}
