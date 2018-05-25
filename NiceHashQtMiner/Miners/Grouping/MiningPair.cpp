#include "Miners/Grouping/MiningPair.h"
#include "Algorithms/Algorithm.h"


MiningPair::MiningPair(const MiningPair& other)
	: Device(other.Device), algorithm(other.algorithm), CurrentExtraLaunchParameters(other.CurrentExtraLaunchParameters)
{ }

MiningPair::MiningPair(ComputeDevice* d, Algorithm* a)
{
	Device=d;
	algorithm=a;
	CurrentExtraLaunchParameters=algorithm->ExtraLaunchParameters;
}
