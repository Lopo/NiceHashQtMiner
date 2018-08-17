#include "Configs/Data/BenchmarkTimeLimitsConfig.h"


const QVector<int> BenchmarkTimeLimitsConfig::DefaultCpuNvidia={20, 60, 120};
#if WITH_AMD
const QVector<int> BenchmarkTimeLimitsConfig::DefaultAmd={120, 180, 240};
#endif


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
	return 0;
}
