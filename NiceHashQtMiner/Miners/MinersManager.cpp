#include "Miners/MinersManager.h"
#include "Devices/ComputeDeviceManager.h"
#include "Miners/MiningSession.h"
#include "Miners/Ethlargement.h"
#include <QtConcurrent/QtConcurrentRun>


MiningSession* MinersManager::_curMiningSession=nullptr;


void MinersManager::StopAllMiners()
{
	if (_curMiningSession!=nullptr) {
		_curMiningSession->StopAllMiners();
		}
	Ethlargement.Stop();
	if (_curMiningSession!=nullptr) {
		delete _curMiningSession;
		_curMiningSession=nullptr;
		}
}

void MinersManager::StopAllMinersNonProfitable()
{
	if (_curMiningSession!=nullptr) {
		_curMiningSession->StopAllMinersNonProfitable();
		}
}

QString MinersManager::GetActiveMinersGroup()
{
	// if no session it is idle
	return _curMiningSession!=nullptr
		? _curMiningSession->GetActiveMinersGroup()
		: "IDLE";
}

QList<int>* MinersManager::GetActiveMinersIndexes()
{
	return _curMiningSession!=nullptr
		? _curMiningSession->ActiveDeviceIndexes()
		: new QList<int>;
}

double MinersManager::GetTotalRate()
{
	if (_curMiningSession!=nullptr) {
		return _curMiningSession->GetTotalRate();
		}
	return 0;
}

bool MinersManager::StartInitialize(IMainFormRatesComunication* mainFormRatesCommunication, QString miningLocation, QString worker, QString btcAddress)
{
	if (_curMiningSession!=nullptr) {delete _curMiningSession;}
	_curMiningSession=new MiningSession(ComputeDeviceManager.Available.Devices, mainFormRatesCommunication, miningLocation, worker, btcAddress);
	return _curMiningSession->IsMiningEnabled();
}

bool MinersManager::IsMiningEnabled()
{
	return _curMiningSession!=nullptr && _curMiningSession->IsMiningEnabled();
}

/**
 * SwichMostProfitable should check the best combination for most profit.
 * Calculate profit for each supported algorithm per device and group.
 * @param niceHashData
 * @deprecated Deprecated in favour of AlgorithmSwitchingManager timer
 *//*
void MinersManager::SwitchMostProfitableGroupUpMethod()
{
	if (_curMiningSession!=nullptr) {
		QtConcurrent::run(_curMiningSession, &MiningSession::SwitchMostProfitableGroupUpMethod).waitForFinished();
		}
}
*/

void MinersManager::MinerStatsCheck()
{
	if (_curMiningSession!=nullptr) {
		QtConcurrent::run(_curMiningSession, &MiningSession::MinerStatsCheck).waitForFinished();
		}
}
