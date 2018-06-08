#include "Benchmarking/BenchmarkHandler.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Algorithms/Algorithm.h"
#include "Benchmarking/BenchHelpers/ClaymoreZcashBenchHelper.h"
#include "Miners/MinerFactory.h"
#include "Utils/Helpers.h"
#include "Algorithms/DualAlgorithm.h"
#include "Interfaces/IBenchmarkForm.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include <QThread>
#include <QMetaEnum>
#include <QtConcurrent/QtConcurrentRun>


BenchmarkHandler::BenchmarkHandler(ComputeDevice* device, QQueue<Algorithm*> algorithms, IBenchmarkForm* form, Enums::BenchmarkPerformanceType performance)
{
	Device=device;
	_benchmarkAlgorithmQueue=algorithms;
	_benchmarkForm=form;
	_performanceType=performance;

	_benchmarkAlgorithmsCount=_benchmarkAlgorithmQueue.count();
	_powerHelper=new PowerHelper(device);
}

void BenchmarkHandler::Start()
{
//	QFuture<void> f=QtConcurrent::run(this, &BenchmarkHandler::NextBenchmark);
	QtConcurrent::run(this, &BenchmarkHandler::NextBenchmark);
/*	QThread* thread=new QThread(NextBenchmark);
	thread->setObjectName(QString("dev_%1-%2_benchmark").arg(QMetaEnum::fromType<Enums::DeviceType>().valueToKey((int)Device->DeviceType)).arg(Device->ID));
	thread->start();*/
}

void BenchmarkHandler::OnBenchmarkComplete(bool success, QString status)
{
	if (!_benchmarkForm->InBenchmark()) {
		return;
		}

	bool rebenchSame=false;
	if (success && _cpuBenchmarkStatus!=nullptr && _cpuAlgos.contains(_currentAlgorithm->NiceHashID) && _currentAlgorithm->MinerBaseType==Enums::MinerBaseType::XmrStak) {
		_cpuBenchmarkStatus->SetNextSpeed(_currentAlgorithm->BenchmarkSpeed());
		rebenchSame=_cpuBenchmarkStatus->HasTest();
		_currentAlgorithm->LessThreads=_cpuBenchmarkStatus->LessTreads();
		if (!rebenchSame) {
			_cpuBenchmarkStatus->FindFastest();
			_currentAlgorithm->BenchmarkSpeed(_cpuBenchmarkStatus->GetBestSpeed());
			_currentAlgorithm->LessThreads=_cpuBenchmarkStatus->GetLessThreads();
			}
		}

	if (_claymoreZcashStatus!=nullptr && _currentAlgorithm->MinerBaseType==Enums::MinerBaseType::Claymore && _currentAlgorithm->NiceHashID==Enums::AlgorithmType::Equihash) {
		if (_claymoreZcashStatus->HasTest()) {
			_currentMiner=MinerFactory::CreateMiner(Device, _currentAlgorithm);
			rebenchSame=true;
			_claymoreZcashStatus->SetSpeed(_currentAlgorithm->BenchmarkSpeed());
			_claymoreZcashStatus->SetNext();
			_currentAlgorithm->ExtraLaunchParameters=_claymoreZcashStatus->GetTestExtraParams();
			Helpers::ConsolePrint("ClaymoreAMD_Equihash", _currentAlgorithm->ExtraLaunchParameters.join(' '));
			_currentMiner->InitBenchmarkSetup(new MiningPair(Device, _currentAlgorithm));
			}

		if (!_claymoreZcashStatus->HasTest()) {
			rebenchSame=false;
			// set fastest mode
			_currentAlgorithm->BenchmarkSpeed(_claymoreZcashStatus->GetFastestTime());
			_currentAlgorithm->ExtraLaunchParameters=_claymoreZcashStatus->GetFastestExtraParams();
			}
		}

	double power=_powerHelper->Stop();

	DualAlgorithm* dualAlgo=qobject_cast<DualAlgorithm*>(_currentAlgorithm);
	if (dualAlgo!=nullptr && dualAlgo->TuningEnabled) {
		dualAlgo->SetPowerForCurrent(power);

		if (dualAlgo->IncrementToNextEmptyIntensity()) {
			rebenchSame=true;
			}
		}
	else {
		_currentAlgorithm->PowerUsage(power);
		}

	if (!rebenchSame) {
		_benchmarkForm->RemoveFromStatusCheck(Device, _currentAlgorithm);
		}

	if (!success && !rebenchSame) {
		// set status to empty string it will return speed
		_currentAlgorithm->ClearBenchmarkPending();
		_benchmarkForm->SetCurrentStatus(Device, _currentAlgorithm, "");
		}

	if (rebenchSame) {
		_powerHelper->Start();

		if (_cpuBenchmarkStatus!=nullptr) {
			_currentMiner->BenchmarkStart(_cpuBenchmarkStatus->Time, this);
			}
		else if (_claymoreZcashStatus!=nullptr) {
			_currentMiner->BenchmarkStart(_claymoreZcashStatus->Time, this);
			}
		else if (dualAlgo!=nullptr && dualAlgo->TuningEnabled) {
			_currentMiner->BenchmarkStart(ConfigManager.generalConfig->BenchmarkTimeLimits->GetBenchmarktime(_performanceType, Device->DeviceGroupType), this);
			}
		}
	else {
		NextBenchmark();
		}
}

void BenchmarkHandler::NextBenchmark()
{
	++_benchmarkCurrentIndex;
	if (_benchmarkCurrentIndex>0) {
		_benchmarkForm->StepUpBenchmarkStepProgress();
		}
	if (_benchmarkCurrentIndex>=_benchmarkAlgorithmsCount) {
		EndBenchmark();
		return;
		}

	if (_benchmarkAlgorithmQueue.count()>0) {
		_currentAlgorithm=_benchmarkAlgorithmQueue.dequeue();
		}

	if (Device!=nullptr && _currentAlgorithm!=nullptr) {
		_currentMiner=MinerFactory::CreateMiner(Device, _currentAlgorithm);
		if (_cpuBenchmarkStatus!=nullptr) {delete _cpuBenchmarkStatus;};
		_cpuBenchmarkStatus=nullptr;

		if (_currentAlgorithm->MinerBaseType==Enums::MinerBaseType::Claymore
			&& _currentAlgorithm->NiceHashID==Enums::AlgorithmType::Equihash
			&& !_currentAlgorithm->ExtraLaunchParameters.empty()
			&& !_currentAlgorithm->ExtraLaunchParameters.contains("-asm")
			) {
			if (_claymoreZcashStatus!=nullptr) { delete _claymoreZcashStatus;};
			_claymoreZcashStatus=new ClaymoreZcashBenchHelper(_currentAlgorithm->ExtraLaunchParameters);
			_currentAlgorithm->ExtraLaunchParameters=_claymoreZcashStatus->GetFastestExtraParams();
			}
		else {
			if (_claymoreZcashStatus!=nullptr) { delete _claymoreZcashStatus;};
			_claymoreZcashStatus=nullptr;
			}

		DualAlgorithm* dualAlgo=qobject_cast<DualAlgorithm*>(_currentAlgorithm);
		if (dualAlgo!=nullptr && dualAlgo->TuningEnabled) {
			dualAlgo->StartTuning();
			}
		}

	if (_currentMiner!=nullptr && _currentAlgorithm!=nullptr && Device!=nullptr) {
		_currentMiner->InitBenchmarkSetup(new MiningPair(Device, _currentAlgorithm));
	
		int time=ConfigManager.generalConfig->BenchmarkTimeLimits->GetBenchmarktime(_performanceType, Device->DeviceGroupType);
		if (_cpuBenchmarkStatus!=nullptr) {
			_cpuBenchmarkStatus->Time=time;
			}
		if (_claymoreZcashStatus!=nullptr) {
			_claymoreZcashStatus->Time=time;
			}

		_benchmarkForm->AddToStatusCheck(Device, _currentAlgorithm);

		_currentMiner->BenchmarkStart(time, this);
		_powerHelper->Start();
		}
	else {
		NextBenchmark();
		}
}

void BenchmarkHandler::EndBenchmark()
{
	if (_currentAlgorithm!=nullptr) {
		_currentAlgorithm->ClearBenchmarkPending();
		}
	_benchmarkForm->EndBenchmarkForDevice(Device, _benchmarkFailedAlgo.count()>0);
}

void BenchmarkHandler::InvokeQuit()
{
	// clear benchmark pending status
	if (_currentAlgorithm!=nullptr) {
		_currentAlgorithm->ClearBenchmarkPending();
		}
	if (_currentMiner!=nullptr) {
		_currentMiner->BenchmarkSignalQuit=true;
		_currentMiner->InvokeBenchmarkSignalQuit();
		}

//	delete _currentMiner;
	_currentMiner=nullptr;
}
