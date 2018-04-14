#include "Miners/Grouping/MiningDevice.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Miners/Grouping/GroupSetupUtils.h"
#include "Miners/Grouping/MinerPaths.h"
#include "Algorithm.h"
#include "Miners/Grouping/MiningPair.h"
#include "Switching/NiceHashSma.h"
#if (SWITCH_TESTING)
#	include "Utils/AlgorithmNiceHashNames.h"
#	include "Utils/Helpers.h"
#endif
#include <QMetaEnum>


#if (SWITCH_TESTING)
QList<Enums::AlgorithmType> MiningDevice::testingAlgos({
	Enums::AlgorithmType::CryptoNight,
	Enums::AlgorithmType::Equihash
	});
int MiningDevice::next=-1;
bool MiningDevice::ForceNone=false;
int MiningDevice::seconds=20;
int MiningDevice::SMAMinerCheckInterval=seconds*1000; // 30s
bool MiningDevice::ForcePerCardMiners=false;

void MiningDevice::SetNextTest()
{
	++next;
	if (next>=testingAlgos.count()) {
		next=0;
		}
	Helpers::ConsolePrint("SWITCH_TESTING", QString("Setting most MostProfitKey to %1").arg(AlgorithmNiceHashNames::GetName(testingAlgos[next])));
}
#endif


MiningDevice::MiningDevice(ComputeDevice* device)
{
	_Device=device;
	foreach (Algorithm* algo, *_Device->GetAlgorithmSettings()) {
		bool isAlgoMiningCapable=GroupSetupUtils::IsAlgoMiningCapable(algo);
		bool isValidMinerPath=MinerPaths::IsValidMinerPath(&algo->MinerBinaryPath);
		if (isAlgoMiningCapable && isValidMinerPath) {
			Algorithms->append(algo);
			}
		}
	_MostProfitableAlgorithmType=Enums::AlgorithmType::NONE;
	_MostProfitableMinerBaseType=Enums::MinerBaseType::NONE;
}

QString MiningDevice::GetMostProfitableString()
{
	return
		QString(QMetaEnum::fromType<Enums::MinerBaseType>().valueToKey((int)_MostProfitableMinerBaseType))
		+"_"
		+QMetaEnum::fromType<Enums::AlgorithmType>().valueToKey(_MostProfitableAlgorithmType);
}

int MiningDevice::GetMostProfitableIndex()
{
	for (int i=0; i<Algorithms->count(); i++) {
		if ((*Algorithms)[i]->DualNiceHashID()==_MostProfitableAlgorithmType && (*Algorithms)[i]->MinerBaseType==_MostProfitableMinerBaseType) {
			return i;
			}
		}
	return -1;
}

int MiningDevice::GetPrevProfitableIndex()
{
	for (int i=0; i<Algorithms->count(); i++) {
		if ((*Algorithms)[i]->DualNiceHashID()==_PrevProfitableAlgorithmType && (*Algorithms)[i]->MinerBaseType==_PrevProfitableMinerBaseType) {
			return i;
			}
		}
	return -1;
}

double MiningDevice::GetCurrentMostProfitValue()
{
	int mostProfitableIndex=GetMostProfitableIndex();
	if (mostProfitableIndex>-1) {
		return (*Algorithms)[mostProfitableIndex]->CurrentProfit;
		}
	return 0;
}

double MiningDevice::GetPrevMostProfitValue()
{
	int mostProfitableIndex=GetPrevProfitableIndex();
	if (mostProfitableIndex>-1) {
		return (*Algorithms)[mostProfitableIndex]->CurrentProfit;
		}
	return 0;
}

MiningPair* MiningDevice::GetMostProfitablePair()
{
	return new MiningPair(_Device, (*Algorithms)[GetMostProfitableIndex()]);
}

bool MiningDevice::HasProfitableAlgo()
{
	return GetMostProfitableIndex()>-1;
}

void MiningDevice::RestoreOldProfitsState()
{
	// restore last state
	_MostProfitableAlgorithmType=_PrevProfitableAlgorithmType;
	_MostProfitableMinerBaseType=_PrevProfitableMinerBaseType;
}

void MiningDevice::SetNotMining()
{
	// device isn't mining (e.g. below profit threshold) so set state to none
	_PrevProfitableAlgorithmType=Enums::AlgorithmType::NONE;
	_PrevProfitableMinerBaseType=Enums::MinerBaseType::NONE;
	_MostProfitableAlgorithmType=Enums::AlgorithmType::NONE;
	_MostProfitableMinerBaseType=Enums::MinerBaseType::NONE;
}

void MiningDevice::CalculateProfits(QMap<Enums::AlgorithmType, double> profits)
{
	// save last state
	_PrevProfitableAlgorithmType=_MostProfitableAlgorithmType;
	_PrevProfitableMinerBaseType=_MostProfitableMinerBaseType;
	// assume none is profitable
	_MostProfitableAlgorithmType=Enums::AlgorithmType::NONE;
	_MostProfitableMinerBaseType=Enums::MinerBaseType::NONE;
	// calculate new profits
	foreach (Algorithm* algo, *Algorithms) {
		Enums::AlgorithmType key=algo->NiceHashID;
		Enums::AlgorithmType secondaryKey=algo->SecondaryNiceHashID;
		if (profits.contains(key)) {
			algo->CurNhmSmaDataVal=profits.value(key);
			algo->CurrentProfit=algo->CurNhmSmaDataVal*algo->AveragedSpeed*0.000000001;
			if (profits.contains(secondaryKey)) {
				algo->SecondaryCurNhmSmaDataVal=profits.value(secondaryKey);
				algo->CurrentProfit+=algo->SecondaryCurNhmSmaDataVal*algo->SecondaryAveragedSpeed*0.000000001;
				}
			}
		else {
			algo->CurrentProfit=0;
			}
		}
	// find max paying value and save key
	double maxProfit=0;
	foreach (Algorithm* algo, *Algorithms) {
		if (maxProfit<algo->CurrentProfit) {
			maxProfit=algo->CurrentProfit;
			_MostProfitableAlgorithmType=algo->DualNiceHashID();
			_MostProfitableMinerBaseType=algo->MinerBaseType;
			}
		}
#if (SWITCH_TESTING)
	QString devname=_Device->GetFullName();
	// set new most profit
	auto a=std::find_if(Algorithms->begin(), Algorithms->end(), [](Algorithm* a){return a->NiceHashID==testingAlgos[next];});
	if (a!=Algorithms->end()) {
		_MostProfitableAlgorithmType=testingAlgos[next];
		_MostProfitableMinerBaseType=(*a)->MinerBaseType;
		}
	else if (ForceNone) {
		_MostProfitableAlgorithmType=Enums::AlgorithmType::NONE;
		_MostProfitableMinerBaseType=Enums::MinerBaseType::NONE;
		}
	Helpers::ConsolePrint("SWITCH_TESTING", QString("Setting device %1 to %2").arg(devname).arg(AlgorithmNiceHashNames::GetName(_MostProfitableAlgorithmType)));
#endif
}
