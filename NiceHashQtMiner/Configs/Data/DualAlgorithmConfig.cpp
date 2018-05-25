#include "Configs/Data/DualAlgorithmConfig.h"


void DualAlgorithmConfig::FixSettingsBounds()
{
	if (TuningStart<0 || TuningStart>TuningEnd) {
		TuningStart=25;
		}
	if (TuningInterval<1) {
		TuningInterval=25;
		}
	TuningEnd=std::max(TuningEnd, TuningStart);
}
