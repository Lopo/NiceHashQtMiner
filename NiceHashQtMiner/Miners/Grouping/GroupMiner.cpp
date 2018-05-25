#include "Miners/Grouping/GroupMiner.h"
#include "Miners/Grouping/MiningPair.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Miners/MinerFactory.h"
#include "Algorithms/Algorithm.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Globals.h"


GroupMiner::GroupMiner(QList<MiningPair*>* miningPairs, QString key)
{
	_AlgorithmType=Enums::AlgorithmType::NONE;
	_DualAlgorithmType=Enums::AlgorithmType::NONE;
	_DevicesInfoString="N/A";
	CurrentRate=0;
	_Key=key;
	if (miningPairs->count()>0) {
		// sort pairs by device id
		std::sort(miningPairs->begin(), miningPairs->end(), [](const MiningPair* a, const MiningPair* b){
			return a->Device->ID<b->Device->ID;
			});
		// init name scope and IDs
		{
			QStringList* deviceNames=new QStringList;
			if (_DevIndexes!=nullptr) {delete _DevIndexes;}
			_DevIndexes=new QList<int>;
			foreach (MiningPair* pair, *miningPairs) {
				deviceNames->append(pair->Device->NameCount);
				_DevIndexes->append(pair->Device->Index());
				TotalPower_+=pair->algorithm->PowerUsage();
				}
			_DevicesInfoString="{ "+deviceNames->join(", ")+" }";
		}
		// init miner
		{
			MiningPair* mPair=(*miningPairs)[0];
			_DeviceType=mPair->Device->DeviceType;
//			if (_Miner!=nullptr) {delete _Miner;}
			_Miner=MinerFactory::CreateMiner(mPair->Device, mPair->algorithm);
			if (_Miner!=nullptr) {
				_Miner->InitMiningSetup(new MiningSetup(miningPairs));
				_AlgorithmType=mPair->algorithm->NiceHashID;
				_DualAlgorithmType=mPair->algorithm->DualNiceHashID();
				}
		}
		}
}

void GroupMiner::Stop()
{
	if (_Miner!=nullptr && _Miner->IsRunning()) {
		_Miner->Stop();
		// wait before going on
		QThread::msleep(ConfigManager.generalConfig->MinerRestartDelayMS);
		}
	CurrentRate=0;
}

void GroupMiner::End()
{
	if (_Miner!=nullptr) {
		_Miner->End();
		}
	CurrentRate=0;
}

void GroupMiner::Start(QString miningLocation, QString btcAdress, QString worker)
{
	if (_Miner->IsRunning()) {
		return;
		}
	// Wait before new start
	QThread::msleep(ConfigManager.generalConfig->MinerRestartDelayMS);
	_Miner->Start(Globals::GetLocationURL(_AlgorithmType, miningLocation, _Miner->ConectionType()), btcAdress, worker);
}
