#ifndef BENCHMARKING_BENCHHELPERS_CPUBENCHHELPER_H_
#define BENCHMARKING_BENCHHELPERS_CPUBENCHHELPER_H_

#include <QList>

class CpuBenchHelper
{
private:
	class CpuBenchmark;

public:
	CpuBenchHelper(int maxThreads);
	int Time;
	inline int LessTreads() const { return LessTreads_;};
	bool HasTest();
	void SetNextSpeed(double speed);
	void FindFastest();
	double GetBestSpeed();
	int GetLessThreads();

private:
	QList<CpuBenchmark> _benchmarks;
	int _maxThreads;

	class CpuBenchmark {
		public:
			CpuBenchmark(int lt, double bench);
			double Benchmark;
			int LessThreads;
		};

	int LessTreads_;
};

#endif
