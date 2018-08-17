#define __Ethlargement_cpp__
#include "Miners/Ethlargement.h"
#include "Utils/Helpers.h"
#include "Grouping/MinerPaths.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Algorithms/Algorithm.h"
#include "Devices/ComputeDevice/CudaComputeDevice.h"
#include <QThread>
#include <QProcess>
#include <QException>


//QProcess* Ethlargement::_process=nullptr;
//MiningSetup* Ethlargement::_cachedSetup=nullptr;


void Ethlargement::CheckAndStart(MiningSetup* setup)
{
//	Lock.lock();
	{std::lock_guard<std::mutex> lock(Lock);
		_cachedSetup=setup;
		if (!ShouldRun(setup)) {
			return;
			}
		_process=new NiceHashProcess;
		_process->setProgram(MinerPaths.Data.EthLargement);

//		if (ConfigManager.generalConfig.HideMiningWindows || ConfigManager.generalConfig.MinimizeMiningWindows) {
//			}

//		_process.EnableRisingEvents
		connect(_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(ExitEvent()));

		try {
			if (_process->Start()) {
				Helpers::ConsolePrint("ETHLARGEMENT", "Starting ethlargement...");
				}
			else {
				Helpers::ConsolePrint("ETHLARGEMENT", "Couldn't start ethlargement");
				}
			}
		catch (QException& ex) {
			Helpers::ConsolePrint("ETHLARGEMENT", ex.what());
			}
	}
//	Lock.unlock();
}

void Ethlargement::ExitEvent()
{
	QThread::msleep(1000);
	CheckAndStart(_cachedSetup);
}

void Ethlargement::Stop()
{
	_cachedSetup=nullptr;
	if (_process==nullptr) {
		return;
		}

	try {
		_process->close();
		if (!_process->waitForFinished(10*1000)) {
			_process->Kill();
			}
		_process->Close();
		}
	catch (std::exception& e)
	{
		Helpers::ConsolePrint("ETHLARGEMENT", e.what());
	}

	delete _process;
	_process=nullptr;
}

bool Ethlargement::ShouldRun(MiningSetup* setup)
{
	if (Running() || setup==nullptr || !ConfigManager.generalConfig->UseEthlargement) {
		return false;
		}

	Helpers::ConsolePrint("ETHLARGEMENT", "Checking if should run ethlargement...");

	if (ConfigManager.generalConfig->Use3rdPartyMiners!=Enums::Use3rdPartyMiners::YES) {
		Helpers::ConsolePrint("ETHLARGEMNT", "Ethlargement not started because 3rd party miners not enabled");
		return false;
		}

	if (!Helpers::IsElevated()) {
		Helpers::ConsolePrint("ETHLARGEMENT", "Ethlargement not started because NHQMis not running as root");
		return false;
		}

	foreach (MiningPair* p, *setup->MiningPairs) {
		if (p->algorithm->NiceHashID==Enums::AlgorithmType::DaggerHashimoto) {
			CudaComputeDevice* cuda=qobject_cast<CudaComputeDevice*>(p);
			if (cuda!=nullptr && cuda->ShouldRunEthlargement()) {
				return true;
				}
			}
		}

	Helpers::ConsolePrint("ETHLARGEMENT", "No ethlargement algorithm/device combos found. Not running");

	return false;
}
