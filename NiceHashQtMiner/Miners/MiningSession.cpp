#include "Miners/MiningSession.h"
#include "Miners/Grouping/GroupSetupUtils.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Utils/Helpers.h"
#include "Devices/GroupNames.h"
#include "Globals.h"
#include "Miners/Grouping/GroupingLogic.h"
#include "Switching/Models.h"
#include "International.h"
#include "Interfaces/IMainFormRatesComunication.h"
#include "Miners/Grouping/GroupMiner.h"
#include "Miners/Miner.h"
#include "Miners/Grouping/MiningDevice.h"
#include "Algorithms/Algorithm.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Switching/NHSmaData.h"
#include "Stats/ExchangeRateAPI.h"
#include "Algorithms/DualAlgorithm.h"
#include <QtConcurrent/QtConcurrentRun>


QList<int>* MiningSession::ActiveDeviceIndexes()
{
	QList<int>* minerIDs=new QList<int>;
	if (!IsCurrentlyIdle()) {
		foreach (GroupMiner* miner, _runningGroupMiners->values()) {
			minerIDs->append(*(miner->DevIndexes()));
			}
		}
	return minerIDs;
}

MiningSession::MiningSession(QList<ComputeDevice*>* devices, IMainFormRatesComunication* mainFormRatesCommunication, QString miningLocation, QString worker, QString btcAdress)
{
	// init fixed
	_mainFormRatesCommunication=mainFormRatesCommunication;
	_miningLocation=miningLocation;

	_switchingManager=new AlgorithmSwitchingManager;
	connect(_switchingManager, SIGNAL(SmaCheck(SmaUpdateEventArgs)), this, SLOT(SwichMostProfitableGroupUpMethod(SmaUpdateEventArgs)));

	_btcAdress=btcAdress;
	_worker=worker;

	// initial settup
	_miningDevices=GroupSetupUtils::GetMiningDevices(devices, true);
	if (_miningDevices->count()>0) {
		GroupSetupUtils::AvarageSpeeds(_miningDevices);
		}

	// init timer stuff
	_preventSleepTimer=new QTimer(this);
	connect(_preventSleepTimer, SIGNAL(timeout()), this, SLOT(PreventSleepTimer_Tick()));
	// sleep time is minimal 1 minute
	_preventSleepTimer->setInterval(20*1000); // leave this interval, it works

	// set internet checking
	_internetCheckTimer=new QTimer(this);
	connect(_internetCheckTimer, SIGNAL(timeout()), this, SLOT(InternetCheckTimer_Tick()));
	_internetCheckTimer->setInterval(1*1000*60); // every minute

	// assume profitable
	_isProfitable=true;
	// assume we have internet
	_isConnectedToInternet=true;

	if (IsMiningEnabled()) {
		_preventSleepTimer->start();
		_internetCheckTimer->start();
		}

	_switchingManager->Start();

	_isMiningRegardlesOfProfit= !ConfigManager.generalConfig->MinimumProfit;
}

void MiningSession::InternetCheckTimer_Tick()
{
	if (ConfigManager.generalConfig->IdleWhenNoInternetAccess) {
		_isConnectedToInternet=Helpers::IsConnectedToInternet();
		}
}

void MiningSession::PreventSleepTimer_Tick()
{
	// when mining keep system awake, prevent sleep
	Helpers::PreventSleep();
}

void MiningSession::StopAllMiners()
{
	if (_runningGroupMiners!=nullptr) {
		foreach (GroupMiner* groupMiner, _runningGroupMiners->values()) {
			groupMiner->End();
			}
		delete _runningGroupMiners;
		_runningGroupMiners=new QMap<QString, GroupMiner*>;
		}
#if WITH_NVIDIA
	if (_ethminerNvidiaPaused!=nullptr) {
		_ethminerNvidiaPaused->End();
		delete _ethminerNvidiaPaused;
		_ethminerNvidiaPaused=nullptr;
		}
#endif
#if WITH_AMD
	if (_ethminerAmdPaused!=nullptr) {
		_ethminerAmdPaused->End();
		delete _ethminerAmdPaused;
		_ethminerAmdPaused=nullptr;
		}
#endif

	_switchingManager->Stop();

	if (_mainFormRatesCommunication!=nullptr) {
		_mainFormRatesCommunication->ClearRatesALL();
		}

	// restore/enable sleep
	_preventSleepTimer->stop();
	_internetCheckTimer->stop();
	Helpers::AllowMonitorPowerdownAndSleep();
}

void MiningSession::StopAllMinersNonProfitable()
{
	if (_runningGroupMiners!=nullptr) {
		foreach (GroupMiner* groupMiner, _runningGroupMiners->values()) {
			groupMiner->End();
			}
		delete _runningGroupMiners;
		_runningGroupMiners=new QMap<QString, GroupMiner*>;
		}
#if WITH_NVIDIA
	if (_ethminerNvidiaPaused!=nullptr) {
		_ethminerNvidiaPaused->End();
		delete _ethminerNvidiaPaused;
		_ethminerNvidiaPaused=nullptr;
		}
#endif
#if WITH_AMD
	if (_ethminerAmdPaused!=nullptr) {
		_ethminerAmdPaused->End();
		delete _ethminerAmdPaused;
		_ethminerAmdPaused=nullptr;
		}
#endif
	if (_mainFormRatesCommunication!=nullptr) {
		_mainFormRatesCommunication->ClearRates(-1);
		}
}

QString MiningSession::CalcGroupedDevicesKey(const GroupedDevices group)
{
	return QStringList::fromSet(group).join(", ");
}

QString MiningSession::GetActiveMinersGroup()
{
	if (IsCurrentlyIdle()) {
		return "IDLE";
		}

	// get unique miner groups like CPU, NVIDIA, AMD,...
	QSet<QString>* uniqueMinerGroups=new QSet<QString>;
	foreach (MiningDevice* miningDevice, *_miningDevices) {
		uniqueMinerGroups->insert(GroupNames::GetNameGeneral(miningDevice->Device()->DeviceType));
		}
	if (uniqueMinerGroups->count()>0 && _isProfitable) {
		return QStringList::fromSet(*uniqueMinerGroups).join('/');
		}
	return "";
}

double MiningSession::GetTotalRate()
{
	double totalRate=0;

	if (_runningGroupMiners!=nullptr) {
		foreach (GroupMiner* groupMiner, _runningGroupMiners->values()) {
			totalRate+=groupMiner->CurrentRate;
			}
		}

	return totalRate;
}

bool MiningSession::CheckIfProfitable(double currentProfit, bool log/*=true*/)
{
	// TODO FOR NOW USD ONLY
	double currentProfitUsd=(currentProfit*ExchangeRateApi::GetUsdExchangeRate());
#pragma GCC diagnostic ignored "-Wparentheses"
	_isProfitable= _isMiningRegardlesOfProfit || !_isMiningRegardlesOfProfit && currentProfitUsd>=ConfigManager.generalConfig->MinimumProfit;
#pragma GCC diagnostic pop
	if (log) {
		Helpers::ConsolePrint(Tag, "Current Global profit: "+QString::number(currentProfitUsd, 'f', 8)+" USD/Day");
		if (!_isProfitable) {
			Helpers::ConsolePrint(Tag, "Current Global profit: NOT PROFITABLE MinProfit "+QString::number(ConfigManager.generalConfig->MinimumProfit, 'f', 8)+" USD/Day");
			}
		else {
			QString profitabilityInfo= _isMiningRegardlesOfProfit
				? "mine always regardless of profit"
				: QString::number(ConfigManager.generalConfig->MinimumProfit, 'f', 8)+" USD/Day";
			Helpers::ConsolePrint(Tag, "Current Global profit: IS PROFITABLE MinProfit "+profitabilityInfo);
			}
		}
	return _isProfitable;
}

bool MiningSession::CheckIfShouldMine(double currentProfit, bool log/*=true*/)
{
	// if profitable and connected to internet mine
	bool shouldMine=CheckIfProfitable(currentProfit, log) && _isConnectedToInternet;
	if (shouldMine) {
		_mainFormRatesCommunication->HideNotProfitable();
		}
	else {
		if (!_isConnectedToInternet) {
			// change msg
			if (log) {
				Helpers::ConsolePrint(Tag, "NO INTERNET!!! Stopping mining.");
				}
			_mainFormRatesCommunication->ShowNotProfitable(International::GetText("Form_Main_MINING_NO_INTERNET_CONNECTION"));
			}
		else {
			_mainFormRatesCommunication->ShowNotProfitable(International::GetText("Form_Main_MINING_NOT_PROFITABLE"));
			}
		// return don't group
		StopAllMinersNonProfitable();
		}
	return shouldMine;
}

void MiningSession::SwichMostProfitableGroupUpMethod(SmaUpdateEventArgs e)
{
#if (SWITCH_TESTING)
	MiningDevice::SetNextTest();
#endif
	QList<MiningPair*>* profitableDevices=new QList<MiningPair*>;
	double currentProfit=0.0d;
	double prevStateProfit=0.0d;
	foreach (MiningDevice* device, *_miningDevices) {
		// calculate profits
		device->CalculateProfits(e.NormalizedProfits);
		// check if device has profitable algo
		if (device->HasProfitableAlgo()) {
			profitableDevices->append(device->GetMostProfitablePair());
			currentProfit+=device->GetCurrentMostProfitValue();
			prevStateProfit+=device->GetPrevMostProfitValue();
			}
		}

	QStringList stringBuilderFull;
	stringBuilderFull << "Current device profits:";
	foreach (MiningDevice* device, *_miningDevices) {
		QStringList stringBuilderDevice;
		stringBuilderDevice << QString("\tProfits for %1 (%2):").arg(device->Device()->Uuid()).arg(device->Device()->GetFullName());
		foreach (Algorithm* algo, *device->Algorithms) {
			stringBuilderDevice << QString("\t\tPROFIT = %1\t(SPEED = %2)\t\t| NHSMA = %3)\t[%4]")
					.arg(QString::number(algo->CurrentProfit(), 'f', 12))
					.arg(QString::number(algo->AvaragedSpeed, 'e', 5))
					.arg(QString::number(algo->CurNhmSmaDataVal(), 'e', 5))
					.arg(algo->AlgorithmStringID());
			DualAlgorithm* dualAlg=qobject_cast<DualAlgorithm*>(algo);
			if (dualAlg!=nullptr) {
				stringBuilderDevice << QString("\t\t\t\t\t  Secondary:\t\t %1\t\t\t\t  %2")
						.arg(QString::number(dualAlg->SecondaryAveragedSpeed, 'e', 5))
						.arg(QString::number(dualAlg->SecondaryCurNhmSmaDataVal, 'e', 5));
				}
			}
		// most profitable
		stringBuilderDevice << QString("\t\tMOST PROFITABLE ALGO: %1, PROFIT: %2").arg(device->GetMostProfitableString()).arg(QString::number(device->GetCurrentMostProfitValue(), 'f', 12));
		stringBuilderFull << stringBuilderDevice.join('\n');
		}
	Helpers::ConsolePrint(Tag, stringBuilderFull.join('\n'));

	// check if should mine
	// Only check if profitable inside this method when getting SMA data, checking during mining is not reliable
	if (!CheckIfShouldMine(currentProfit)) {
		foreach (MiningDevice* device, *_miningDevices) {
			device->SetNotMining();
			}
		return;
		}

	// check profit threshold
	Helpers::ConsolePrint(Tag, QString("PrevStateProfit %1, Currentprofit %2").arg(prevStateProfit).arg(currentProfit));
	if (prevStateProfit>0 && currentProfit>0) {
		double a=std::max(prevStateProfit, currentProfit);
		double b=std::min(prevStateProfit, currentProfit);
		double percDiff=(a-b)/b;
		if (percDiff<ConfigManager.generalConfig->SwitchProfitabilityThreshold) {
			// don't switch
			Helpers::ConsolePrint(Tag, QString("Will NOT switch profit diff is %1, current threshold %2").arg(percDiff).arg(ConfigManager.generalConfig->SwitchProfitabilityThreshold));
			// RESTORE OLD PROFITS STATE
			foreach (MiningDevice* device, *_miningDevices) {
				device->RestoreOldProfitsState();
				}
			return;
			}
		Helpers::ConsolePrint(Tag, QString("Will SWITCH profit diff is %1, current threshold %2").arg(percDiff).arg(ConfigManager.generalConfig->SwitchProfitabilityThreshold));
		}

	// group new miners
	QMap<QString, QList<MiningPair*>*>* newGroupedMiningPairs=new QMap<QString, QList<MiningPair*>*>;
	{ // group devices with same supported algorithms
		QList<GroupedDevices*>* currentGroupedDevices=new QList<GroupedDevices*>;
		for (int first=0; first<profitableDevices->count(); ++first) {
			ComputeDevice* firstDev=(*profitableDevices)[first]->Device;
			// check if is in group
			bool isInGroup=false;
			foreach (GroupedDevices* groupedDevices, *currentGroupedDevices) {
				if (groupedDevices->contains(firstDev->Uuid())) {
					isInGroup=true;
					break;
					}
				}
			// if device is not in any group create new group and check if other device should group
			if (!isInGroup) {
				GroupedDevices* newGroup=new GroupedDevices;
				QList<MiningPair*>* miningPairs=new QList<MiningPair*>({profitableDevices->at(first)});
				newGroup->insert(firstDev->Uuid());
				for (int second=first+1; second<profitableDevices->count(); ++second) {
					// check if we should group
					MiningPair* firstPair=(*profitableDevices)[first];
					MiningPair* secondPair=(*profitableDevices)[second];
					if (GroupingLogic::ShouldGroup(firstPair, secondPair)) {
						ComputeDevice* secondDev=(*profitableDevices)[second]->Device;
						newGroup->insert(secondDev->Uuid());
						miningPairs->append((*profitableDevices)[second]);
						}
					}
				currentGroupedDevices->append(newGroup);
				(*newGroupedMiningPairs)[CalcGroupedDevicesKey(*newGroup)]=miningPairs;
				}
			}
	}
	{
		// check which groupMiners should be stopped and which ones should be started and which to keep running
		QMap<QString, GroupMiner*>* toStopGroupMiners=new QMap<QString, GroupMiner*>;
		QMap<QString, GroupMiner*>* toRunNewGroupMiners=new QMap<QString, GroupMiner*>;
		QMap<QString, GroupMiner*>* noChangeGroupMiners=new QMap<QString, GroupMiner*>;
		// check what to stop/update
		foreach (QString runningGroupKey, _runningGroupMiners->keys()) {
			if (!newGroupedMiningPairs->contains(runningGroupKey)) {
				// runningGroupKey not in new group definately needs to be stopped and removed from curently running
				(*toStopGroupMiners)[runningGroupKey]=(*_runningGroupMiners)[runningGroupKey];
				}
			else {
				// runningGroupKey is contained but needs to check if mining algorithm is changed
				QList<MiningPair*>* miningPairs=(*newGroupedMiningPairs)[runningGroupKey];
				Enums::AlgorithmType newAlgoType=GetMinerPairAlgorithmType(miningPairs);
				if (newAlgoType!=Enums::AlgorithmType::NONE && newAlgoType!=Enums::AlgorithmType::INVALID) {
					// Check if dcri optimal value has changed
					bool dcriChanged=false;
					foreach (MiningPair* mPair, *_runningGroupMiners->value(runningGroupKey)->miner()->miningSetup()->MiningPairs) {
						DualAlgorithm* algo=qobject_cast<DualAlgorithm*>(mPair->algorithm);
						if (algo!=nullptr && algo->TuningEnabled && algo->MostProfitableIntensity()!=algo->CurrentIntensity) {
							dcriChanged=true;
							break;
							}
						}

					// if algoType valid and different from currently running update
					if (newAlgoType!=(*_runningGroupMiners)[runningGroupKey]->DualAlgorithmType() || dcriChanged) {
						// remove current one and schedule to stop mining
						(*toStopGroupMiners)[runningGroupKey]=(*_runningGroupMiners)[runningGroupKey];
						// create new one TODO check if DaggerHashimoto
						GroupMiner* newGroupMiner=nullptr;
						if (newAlgoType==Enums::AlgorithmType::DaggerHashimoto) {
#if WITH_NVIDIA
							if (_ethminerNvidiaPaused!=nullptr && _ethminerNvidiaPaused->Key()==runningGroupKey) {
								newGroupMiner=_ethminerNvidiaPaused;
								}
#endif
#if WITH_AMD
							if (_ethminerAmdPaused!=nullptr && _ethminerAmdPaused->Key()==runningGroupKey) {
								newGroupMiner=_ethminerAmdPaused;
								}
#endif
							}
						if (newGroupMiner==nullptr) {
							newGroupMiner=new GroupMiner(miningPairs, runningGroupKey);
							}
						(*toRunNewGroupMiners)[runningGroupKey]=newGroupMiner;
						}
					else {
						(*noChangeGroupMiners)[runningGroupKey]=(*_runningGroupMiners)[runningGroupKey];
						}
					}
				}
			}
		// check brand new
		foreach (QString key, newGroupedMiningPairs->keys()) {
			QList<MiningPair*>* miningPairs=newGroupedMiningPairs->value(key);
			if (!_runningGroupMiners->contains(key)) {
				GroupMiner* newGroupMiner=new GroupMiner(miningPairs, key);
				(*toRunNewGroupMiners)[key]=newGroupMiner;
				}
			}

		if (toStopGroupMiners->values().count()>0 || toRunNewGroupMiners->values().count()>0) {
			QStringList stringBuilderPreviousAlgo;
			QStringList stringBuilderCurrentAlgo;
			QStringList stringBuilderNoChangeAlgo;

			// stop old miners
			foreach (GroupMiner* toStop, toStopGroupMiners->values()) {
				stringBuilderPreviousAlgo << QString("%1: %2, ").arg(toStop->DevicesInfoString()).arg(toStop->AlgorithmType());

				toStop->Stop();
				_runningGroupMiners->remove(toStop->Key());
				// TODO check if DaggerHashimoto and save
				if (toStop->AlgorithmType()==Enums::AlgorithmType::DaggerHashimoto) {
#if WITH_NVIDIA
					if (toStop->DeviceType()==Enums::DeviceType::NVIDIA) {
						_ethminerNvidiaPaused=toStop;
						}
#	if WITH_AMD
					else
#	endif
#endif
#if WITH_AMD
					if (toStop->DeviceType()==Enums::DeviceType::AMD) {
						_ethminerAmdPaused=toStop;
						}
#endif
					}
				}
			//start new miners
			foreach (GroupMiner* toStart, toRunNewGroupMiners->values()) {
				stringBuilderCurrentAlgo << QString("%1: %2, ").arg(toStart->DevicesInfoString()).arg(toStart->AlgorithmType());

				toStart->Start(_miningLocation, _btcAdress, _worker);
				(*_runningGroupMiners)[toStart->Key()]=toStart;
				}

			foreach (GroupMiner* noChange, noChangeGroupMiners->values()) {
				stringBuilderNoChangeAlgo << QString("%1: %2, ").arg(noChange->DevicesInfoString()).arg(noChange->AlgorithmType());
				}

			if (stringBuilderPreviousAlgo.length()>0) {
				Helpers::ConsolePrint(Tag, "Stop Mining: "+stringBuilderPreviousAlgo.join('\n'));
				}

			if (stringBuilderCurrentAlgo.length()>0) {
				Helpers::ConsolePrint(Tag, "Now Mining : "+stringBuilderCurrentAlgo.join('\n'));
				}

			if (stringBuilderNoChangeAlgo.length()>0) {
				Helpers::ConsolePrint(Tag, "No change  :"+stringBuilderNoChangeAlgo.join('\n'));
				}
			}
		}

	// stats quick fix code
//	QtConcurrent::run(this, &MiningSession::MinerStatsCheck, niceHashData).waitForFinished();

	if (_mainFormRatesCommunication!=nullptr) {
		_mainFormRatesCommunication->ForceMinerStatsUpdate();
		}
}

Enums::AlgorithmType MiningSession::GetMinerPairAlgorithmType(QList<MiningPair*>* miningPairs)
{
	if (miningPairs->count()>0) {
		return (*miningPairs)[0]->algorithm->DualNiceHashID();
		}
	return Enums::AlgorithmType::NONE;
}

void MiningSession::MinerStatsCheck()
{
	double currentProfit=0.0d;
	_mainFormRatesCommunication->ClearRates(_runningGroupMiners->count());
	try {
		foreach (GroupMiner* groupMiners, _runningGroupMiners->values()) {
			Miner* m=groupMiners->miner();

			// skip if not running
			if (!m->IsRunning() || m->IsUpdatingApi) {
				continue;
				}

			m->IsUpdatingApi=true;
			ApiData* ad=QtConcurrent::run<ApiData*>(m, &Miner::GetSummaryAsync).result();
			m->IsUpdatingApi=false;
			if (ad==nullptr) {
				Helpers::ConsolePrint(m->MinerTag(), "GetSummary returned null..");
				}
			// set rates
			double paying;
			if (ad!=nullptr && NHSmaData::TryGetPaying(ad->AlgorithmID, paying)) {
				groupMiners->CurrentRate=paying*ad->Speed*0.000000001;
				double secPaying;
				if (NHSmaData::TryGetPaying(ad->SecondaryAlgorithmID, secPaying)) {
					groupMiners->CurrentRate+=secPaying*ad->SecondarySpeed*0.000000001;
					}
				// Deduct power costs
				double powerUsage= ad->PowerUsage>0? ad->PowerUsage : groupMiners->TotalPower();
				groupMiners->CurrentRate-=ExchangeRateApi::GetKwhPriceInBtc()*powerUsage*24/1000;
				}
			else {
				groupMiners->CurrentRate=0;
				// set empty
				ad=new ApiData(groupMiners->AlgorithmType());
				}
			currentProfit+=groupMiners->CurrentRate;
			// update GUI
			_mainFormRatesCommunication->AddRateInfo(m->MinerTag(), groupMiners->DevicesInfoString(), ad, groupMiners->CurrentRate, m->IsApiReadException());
			}
		}
	catch (QException& e) {
		Helpers::ConsolePrint(Tag, e.what());
		}
}
