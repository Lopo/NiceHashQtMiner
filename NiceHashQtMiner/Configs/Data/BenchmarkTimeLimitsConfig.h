#ifndef CONFIGS_DATA_BENCHAMRKTIMELIMITSCONFIG_H_
#define CONFIGS_DATA_BENCHAMRKTIMELIMITSCONFIG_H_

#include "config.h"
#include "Enums.h"
#include <QVector>



class BenchmarkTimeLimitsConfig
{
public:
	static const int Size=3;

	inline QVector<int> CPU() const { return _benchmarkTimeLimitsCPU; };
	inline void setCPU(const QVector<int> value) { _benchmarkTimeLimitsCPU= IsValid(&value)? value : DefaultCpuNvidia; };
#if WITH_NVIDIA
	inline QVector<int> NVIDIA() const { return _benchmarkTimeLimitsNVIDIA; };
	inline void setNVIDIA(const QVector<int> value) { _benchmarkTimeLimitsNVIDIA= IsValid(&value)? value : DefaultCpuNvidia; };
#endif
#if WITH_AMD
	inline QVector<int> AMD() const { return _benchmarkTimeLimitsAMD; };
	inline void setAMD(const QVector<int> value) { _benchmarkTimeLimitsAMD= IsValid(&value)? value : DefaultAmd; };
#endif

	int GetBenchmarktime(Enums::BenchmarkPerformanceType benchmarkPerformanceType, Enums::DeviceGroupType deviceGroupType);

private:
//	const QVector<int> DefaultCpuNvidia={20, 60, 120};
	static const QVector<int> DefaultCpuNvidia;
#if WITH_AMD
//	const QVector<int> DefaultAmd;
	static const QVector<int> DefaultAmd;
#endif

	QVector<int> _benchmarkTimeLimitsCPU=DefaultCpuNvidia;
#if WITH_NVIDIA
	QVector<int> _benchmarkTimeLimitsNVIDIA=DefaultCpuNvidia;
#endif
#if WITH_AMD
	QVector<int> _benchmarkTimeLimitsAMD=DefaultAmd;
#endif

	bool IsValid(const QVector<int>* value) const;
};

#endif
