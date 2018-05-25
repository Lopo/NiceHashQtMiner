#define __GroupSetupUtils_cpp__
#include "Miners/Grouping/GroupSetupUtils.h"
#include "Algorithms/Algorithm.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Miners/Grouping/MinerPaths.h"
#include "Miners/Grouping/MiningDevice.h"
#include "Utils/Helpers.h"
#include "Algorithms/DualAlgorithm.h"


const QString GroupSetupUtils::Tag="GroupSetupUtils";


bool GroupSetupUtils::IsAlgoMiningCapable(Algorithm* algo)
{
	return algo!=nullptr && algo->Enabled && algo->BenchmarkSpeed()>0;
}

std::tuple<ComputeDevice*, Enums::DeviceMiningStatus>* GroupSetupUtils::GetDeviceMiningStatus(ComputeDevice* device)
{
	Enums::DeviceMiningStatus status=Enums::DeviceMiningStatus::CanMine;
	if (device==nullptr) {
		status=Enums::DeviceMiningStatus::DeviceNull;
		}
	else if (!device->Enabled) {
		status=Enums::DeviceMiningStatus::Disabled;
		}
	else {
		bool hasEnabledAlgo=false;
		foreach (Algorithm* algo, *device->GetAlgorithmSettings()) {
			hasEnabledAlgo|=(IsAlgoMiningCapable(algo) && MinerPaths::IsValidMinerPath(&algo->MinerBinaryPath));
			}
		if (!hasEnabledAlgo) {
			status=Enums::DeviceMiningStatus::NoEnabledAlgorithms;
			}
		}
	return new std::tuple<ComputeDevice*, Enums::DeviceMiningStatus>(device, status);
}

std::tuple<QList<MiningDevice*>*, QList<std::tuple<ComputeDevice*, Enums::DeviceMiningStatus>*>*>* GroupSetupUtils::GetMiningAndNonMiningDevices(QList<ComputeDevice*>* devices)
{
	QList<std::tuple<ComputeDevice*, Enums::DeviceMiningStatus>*>* nonMiningDevStatuses=new QList<std::tuple<ComputeDevice*, Enums::DeviceMiningStatus>*>;
	QList<MiningDevice*>* miningDevices=new QList<MiningDevice*>;
	foreach (ComputeDevice* dev, *devices) {
		std::tuple<ComputeDevice*, Enums::DeviceMiningStatus>* devStatus=GetDeviceMiningStatus(dev);
		if (std::get<1>(*devStatus)==Enums::DeviceMiningStatus::CanMine) {
			miningDevices->append(new MiningDevice(dev));
			}
		else {
			nonMiningDevStatuses->append(devStatus);
			}
		}
	return new std::tuple<QList<MiningDevice*>*, QList<std::tuple<ComputeDevice*, Enums::DeviceMiningStatus>*>*>(miningDevices, nonMiningDevStatuses);
}

QString GroupSetupUtils::GetDisabledDeviceStatusString(std::tuple<ComputeDevice*, Enums::DeviceMiningStatus>* devStatusTuple)
{
	switch (std::get<1>(*devStatusTuple)) {
		case Enums::DeviceMiningStatus::DeviceNull:
			return "Passed Device is NULL";
		case Enums::DeviceMiningStatus::Disabled:
			return "DISABLED: "+std::get<0>(*devStatusTuple)->GetFullName();
		case Enums::DeviceMiningStatus::NoEnabledAlgorithms:
			return "No Enabled Algorithms: "+std::get<0>(*devStatusTuple)->GetFullName();
		default:
			return "Invalid status Passed";
		}
}

void GroupSetupUtils::LogMiningNonMiningStatuses(QList<MiningDevice*>* enabledDevices, QList<std::tuple<ComputeDevice*, Enums::DeviceMiningStatus>*>* disabledDevicesStatuses)
{
	// print statuses
	if (disabledDevicesStatuses->count()>0) {
		QStringList stringBuilder;
		stringBuilder << "";
		stringBuilder << "Disabled Devices:";
		foreach (auto deviceStatus, *disabledDevicesStatuses) {
			stringBuilder << "\t"+GetDisabledDeviceStatusString(deviceStatus);
			}
		Helpers::ConsolePrint(Tag, stringBuilder.join('\n'));
		}
	if (enabledDevices->count()>0) {
		// print enabled
		QStringList stringBuilder;
		stringBuilder << "";
		stringBuilder << "Enabled Devices for Mining session:";
		foreach (MiningDevice* miningDevice, *enabledDevices) {
			ComputeDevice* device=miningDevice->Device();
			stringBuilder << QString("\tENABLED (%1)").arg(device->GetFullName());
			foreach (Algorithm* algo, *device->GetAlgorithmSettings()) {
				bool isEnabled=IsAlgoMiningCapable(algo) && MinerPaths::IsValidMinerPath(&algo->MinerBinaryPath);
				stringBuilder << QString("\t\tALGORITHM %1 (%2)").arg(isEnabled? "ENABLED " : "DISABLED").arg(algo->AlgorithmStringID());
				}
			}
		Helpers::ConsolePrint(Tag, stringBuilder.join('\n'));
		}
}

QList<MiningDevice*>* GroupSetupUtils::GetMiningDevices(QList<ComputeDevice*>* devices, bool log)
{
	std::tuple<QList<MiningDevice*>*, QList<std::tuple<ComputeDevice*, Enums::DeviceMiningStatus>*>*>* miningNonMiningDevs=GetMiningAndNonMiningDevices(devices);
	if (log) {
		LogMiningNonMiningStatuses(std::get<0>(*miningNonMiningDevs), std::get<1>(*miningNonMiningDevs));
		}
	return std::get<0>(*miningNonMiningDevs);
}

// avarage passed in benchmark values
void GroupSetupUtils::AvarageSpeeds(QList<MiningDevice*>* miningDevs)
{
	// calculate avarage speeds, to ensure mining stability
	// device name, algo key, algos refs list
	QMap<QString, AvaragerGroup*>* allAvaragers=new QMap<QString, AvaragerGroup*>;

	// init empty avarager
	foreach (MiningDevice* device, *miningDevs) {
		QString devName=device->Device()->Name;
		(*allAvaragers)[devName]=new AvaragerGroup;
		}
	// fill avarager
	foreach (MiningDevice* device, *miningDevs) {
		QString devName=device->Device()->Name;
		// add UUID
		(*allAvaragers)[devName]->UuidList->append(device->Device()->Uuid());
		(*allAvaragers)[devName]->AddAlgorithms(device->Algorithms);
		}
	// calculate and set new AvarageSpeeds for miningDevicesReferences
	foreach (QString key, allAvaragers->keys()) {
		AvaragerGroup* curAvarager=allAvaragers->value(key);
		QMap<QString, QList<double>*>* calculatedAvaragers=curAvarager->CalculateAverages();
		foreach (QString uuid, *curAvarager->UuidList) {
			int minerDevIndex=-1;
			for (int i=0; i<miningDevs->count(); i++) {
				if ((*miningDevs)[i]->Device()->Uuid()==uuid) {
					minerDevIndex=i;
					break;
					}
				}
			if (minerDevIndex>=-1) {
				foreach (QString algoID, calculatedAvaragers->keys()) {
					double avaragedSpeed=(*calculatedAvaragers->value(algoID))[0];
					double secondaryAvaragedSpeed=(*calculatedAvaragers->value(algoID))[1];
					int index=-1;
					for (int i=0; i<(*miningDevs)[minerDevIndex]->Algorithms->count(); i++) {
						if ((*miningDevs)[minerDevIndex]->Algorithms->at(i)->AlgorithmStringID()==algoID) {
							index=i;
							break;
							}
						}
					if (index>-1) {
						(*(*miningDevs)[minerDevIndex]->Algorithms)[index]->AvaragedSpeed=avaragedSpeed;
						DualAlgorithm* dualAlgo=qobject_cast<DualAlgorithm*>(miningDevs->value(minerDevIndex)->Algorithms->value(index));
						if (dualAlgo!=nullptr) {
							dualAlgo->SecondaryAveragedSpeed=secondaryAvaragedSpeed;
							}
						}
					}
				}
			}
		}
}

double SpeedSumCount::GetAvarage()
{
	if (Count>0) {
		return Speed/(double)Count;
		}
	return 0;
}

double SpeedSumCount::GetSecondaryAvarage()
{
	if (Count>0) {
		return SecondarySpeed/(double)Count;
		}
	return 0;
}

QMap<QString, QList<double>*>* AvaragerGroup::CalculateAverages()
{
	QMap<QString, QList<double>*>* ret=new QMap<QString, QList<double>*>;
	foreach (QString algoID, BenchmarkSums->keys()) {
		SpeedSumCount* ssc=BenchmarkSums->value(algoID);
		(*ret)[algoID]=new QList<double> {ssc->GetAvarage(), ssc->GetSecondaryAvarage()};
		}
	return ret;
}

void AvaragerGroup::AddAlgorithms(QList<Algorithm*>* algos)
{
	foreach (Algorithm* algo, *algos) {
		QString algoID=algo->AlgorithmStringID();
		double secondarySpeed=0;
		DualAlgorithm* dualAlgo=qobject_cast<DualAlgorithm*>(algo);
		if (dualAlgo!=nullptr) {
			secondarySpeed=dualAlgo->SecondaryBenchmarkSpeed();
			}
		if (!BenchmarkSums->contains(algoID)) {
			SpeedSumCount* ssc=new SpeedSumCount;
			ssc->Count=1;
			ssc->Speed=algo->BenchmarkSpeed();
			ssc->SecondarySpeed=secondarySpeed;
			(*BenchmarkSums)[algoID]=ssc;
			}
		else {
			(*BenchmarkSums)[algoID]->Count++;
			(*BenchmarkSums)[algoID]->Speed+=algo->BenchmarkSpeed();
			(*BenchmarkSums)[algoID]->SecondarySpeed+=secondarySpeed;
			}
		}
}
