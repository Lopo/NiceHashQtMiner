#ifndef MINERS_GROUPING_GROUPINGLOGIC_H_
#define MINERS_GROUPING_GROUPINGLOGIC_H_

//#include "Miners/Grouping/MiningPair.h"
class MiningPair;


class GroupingLogic
{
public:
	static bool ShouldGroup(MiningPair* a, MiningPair* b);

private:
	static bool IsNotCpuGroups(MiningPair* a, MiningPair* b);
	static bool IsSameBinPath(MiningPair* a, MiningPair* b);
	static bool IsSameAlgorithmType(MiningPair* a, MiningPair* b);
	static bool IsSameDeviceType(MiningPair* a, MiningPair* b);
	static bool IsGroupableMinerBaseType(MiningPair* a);
};

#endif
