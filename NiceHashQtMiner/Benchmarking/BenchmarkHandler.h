#ifndef BENCHMARKING_BENCHMARKHANDLER_H_
#define BENCHMARKING_BENCHMARKHANDLER_H_

#include "Interfaces/IBenchmarkComunicator.h"
#include "BenchHelpers/CpuBenchHelper.h"
#include "BenchHelpers/PowerHelper.h"
#include "Enums.h"
#include <QQueue>

class ComputeDevice;
class Algorithm;
class IBenchmarkForm;
class Miner;
class ClaymoreZcashBenchHelper;
class CpuBenchHelper;


class BenchmarkHandler
	: public QObject,
		public IBenchmarkComunicator
{
public:
	BenchmarkHandler(ComputeDevice* device, QQueue<Algorithm*> algorithms, IBenchmarkForm* form, Enums::BenchmarkPerformanceType performance);

	ComputeDevice* Device=nullptr;

	void Start();
	void OnBenchmarkComplete(bool success, QString status) override;
	void InvokeQuit();

private:
	QQueue<Algorithm*> _benchmarkAlgorithmQueue;
	int _benchmarkAlgorithmsCount=0;
	int _benchmarkCurrentIndex=-1;
	QStringList _benchmarkFailedAlgo;
	IBenchmarkForm* _benchmarkForm=nullptr;
	Algorithm* _currentAlgorithm=nullptr;
	Miner* _currentMiner=nullptr;
	Enums::BenchmarkPerformanceType _performanceType;
	ClaymoreZcashBenchHelper* _claymoreZcashStatus=nullptr;

	CpuBenchHelper* _cpuBenchmarkStatus;

	PowerHelper* _powerHelper;

	// CPU sweet spots
	QList<Enums::AlgorithmType> _cpuAlgos={Enums::AlgorithmType::CryptoNight};

	void NextBenchmark();
	void EndBenchmark();
};

#endif
