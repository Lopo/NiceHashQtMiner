#include "Benchmarking/BenchHelpers/CpuBenchHelper.h"


CpuBenchHelper::CpuBenchHelper(int maxThreads)
{
	_maxThreads=maxThreads;
}

bool CpuBenchHelper::HasTest()
{
	return LessTreads_<_maxThreads;
}

void CpuBenchHelper::SetNextSpeed(double speed)
{
	if (HasTest()) {
		_benchmarks.append(CpuBenchmark(LessTreads_, speed));
		++LessTreads_;
		}
}

void CpuBenchHelper::FindFastest()
{
	std::sort(_benchmarks.begin(), _benchmarks.end(), [](CpuBenchmark& a, CpuBenchmark& b){return -a.Benchmark>b.Benchmark;});
}

double CpuBenchHelper::GetBestSpeed()
{
	return _benchmarks[0].Benchmark;
}

int CpuBenchHelper::GetLessThreads()
{
	return _benchmarks[0].LessThreads;
}

CpuBenchHelper::CpuBenchmark::CpuBenchmark(int lt, double bench)
{
	LessThreads=lt;
	Benchmark=bench;
}
