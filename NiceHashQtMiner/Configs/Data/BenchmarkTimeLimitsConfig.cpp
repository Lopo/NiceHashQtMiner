#include "Configs/Data/BenchmarkTimeLimitsConfig.h"


//const QVector<int> BenchmarkTimeLimitsConfig::DefaultCpuNvidia={10, 20, 60};
//const QVector<int> BenchmarkTimeLimitsConfig::DefaultAmd={120, 180, 240};


bool BenchmarkTimeLimitsConfig::IsValid(const QVector<int>* value) const
{
	return value!=nullptr && value->length()==Size;
}

int BenchmarkTimeLimitsConfig::GetBenchmarktime(Enums::BenchmarkPerformanceType benchmarkPerformanceType, Enums::DeviceGroupType deviceGroupType)
{
	if (deviceGroupType==Enums::DeviceGroupType::CPU) {
		return CPU()[(int)benchmarkPerformanceType];
		}
#if WITH_AMD
	if (deviceGroupType==Enums::DeviceGroupType::AMD_OpenCL) {
		return AMD()[(int)benchmarkPerformanceType];
		}
#endif
#if WITH_NVIDIA
	return NVIDIA()[(int)benchmarkPerformanceType];
#endif
}
