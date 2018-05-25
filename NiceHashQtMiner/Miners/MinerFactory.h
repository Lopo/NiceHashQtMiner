#ifndef MINERS_MINERFACTORY_H_
#define MINERS_MINERFACTORY_H_

#include "Miners/Miner.h"
#include "Enums.h"
class Algorithm;


class MinerFactory
{
public:
	static Miner* CreateMiner(Enums::DeviceType deviceType, Algorithm* algorithm);
	static Miner* CreateMiner(ComputeDevice* device, Algorithm* algorithm);

private:
	static Miner* CreateEthminer(Enums::DeviceType deviceType);
	static Miner* CreateClaymore(Algorithm* algorithm);
	static Miner* CreateExperimental(Enums::DeviceType deviceType, Enums::AlgorithmType algorithmType);
};

#endif
