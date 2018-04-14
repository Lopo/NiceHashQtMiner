#define __MinersSettingsManager_cpp__
#include "Miners/MinersSettingsManager.h"
#include "Devices/GroupAlgorithms.h"
#include "Miners/Grouping/MinerPaths.h"
#include "Miners/Parsing/ExtraLaunchParameters.h"
#include "Configs/ConfigJsonFile/MinerSystemVariablesConfigFile.h"


QMap<Enums::MinerBaseType, QMap<QString, QMap<Enums::AlgorithmType, QList<int>*>*>*>* MinersSettingsManager::_minerReservedPorts=new QMap<Enums::MinerBaseType, QMap<QString, QMap<Enums::AlgorithmType, QList<int>*>*>*>;
QList<int>* MinersSettingsManager::AllReservedPorts=new QList<int>;
MinerSystemVariablesConfig* MinersSettingsManager::MinerSystemVariables=new MinerSystemVariablesConfig;


void MinersSettingsManager::Init()
{
	ExtraLaunchParameters.InitializePackages();
	MinerPaths.InitializePackages();
	InitMinerReservedPortsFile();
	InitMinerSystemVariablesFile();
}

QList<int>* MinersSettingsManager::GetPortsListFor(Enums::MinerBaseType minerBaseType, QString path, Enums::AlgorithmType algorithmType)
{
	if (_minerReservedPorts!=nullptr && _minerReservedPorts->contains(minerBaseType)) {
		if (_minerReservedPorts->value(minerBaseType)!=nullptr && _minerReservedPorts->value(minerBaseType)->contains(path)) {
			if (_minerReservedPorts->value(minerBaseType)->value(path)!=nullptr && _minerReservedPorts->value(minerBaseType)->value(path)->contains(algorithmType)) {
				if (_minerReservedPorts->value(minerBaseType)->value(path)->value(algorithmType)!=nullptr) {
					return _minerReservedPorts->value(minerBaseType)->value(path)->value(algorithmType);
					}
				}
			}
		}
	return new QList<int>;
}

void MinersSettingsManager::InitMinerReservedPortsFile()
{/*
	MinerReservedPortsFile* file=new MinerReservedPortsFile();
	MinerReservedPorts=new QMap<Enums::MinerBaseType, QMap<QString, QMap<Enums::AlgorithmType, QList<int>*>*>*>;
	if (file->IsFileExists()) {
		QJsonDocument fRead=QJsonDocument::fromJson(file->toString()->toUtf8());
//		QMap<Enums::MinerBaseType, QMap<QString, QMap<Enums::AlgorithmType, QList<int>>>*>* read=file->ReadFile();
		QMap<Enums::MinerBaseType, QMap<QString, QMap<Enums::AlgorithmType, QList<int>*>*>*>* read=new QMap<Enums::MinerBaseType, QMap<QString, QMap<Enums::AlgorithmType, QList<int>*>*>*>();
		foreach (QString mbts, fRead.object().keys()) {
			Enums::MinerBaseType mbte=(Enums::MinerBaseType)QMetaEnum::fromType<Enums::MinerBaseType>().keyToValue(mbts.toUtf8().data());
			QJsonObject mbto=fRead.object().value(mbts).toObject();
			QMap<QString, QMap<Enums::AlgorithmType, QList<int>*>*>* iMap2=new QMap<QString, QMap<Enums::AlgorithmType, QList<int>*>*>();
			foreach (QString mbtok, mbto.keys()) {
				QJsonObject mbtoo=mbto.value(mbtok).toObject();
				QMap<Enums::AlgorithmType, QList<int>*>* iMap3=new QMap<Enums::AlgorithmType, QList<int>*>;
				foreach (QString mbtooes, mbtoo.keys()) {
					Enums::AlgorithmType mbtooe=(Enums::AlgorithmType)QMetaEnum::fromType<Enums::AlgorithmType>().keyToValue(mbtooes.toUtf8().data());
					QJsonArray mbtooa=mbtoo.value(mbtooes).toArray();
					QList<int>* iList=new QList<int>;
					foreach (QJsonValue port, mbtooa) {
						iList->append(port.toInt());
						}
					iMap3->insert(mbtooe, iList);
					}
				iMap2->insert(mbtok, iMap3);
				}
			read->insert(mbte, iMap2);
			}
		if (read!=nullptr) {
			delete MinerReservedPorts;
			MinerReservedPorts=read;
			}
		}
	try {
		for (int type=(int)Enums::MinerBaseType::NONE+1; type<(int)Enums::MinerBaseType::END; ++type) {
			if (!MinerReservedPorts->contains((Enums::MinerBaseType)type)) {
				MinerReservedPorts->insert((Enums::MinerBaseType)type, new QMap<QString, QMap<Enums::AlgorithmType, QList<int>*>*>());
				}
			}
		for (int devGroupType=(int)Enums::DeviceGroupType::NONE+1; devGroupType<(int)Enums::DeviceGroupType::LAST; ++devGroupType) {
			QMap<Enums::MinerBaseType, QList<Algorithm*>*>* minerAlgosForGroup=GroupAlgorithms::CreateDefaultsForGroup((Enums::DeviceGroupType)devGroupType);
			if (minerAlgosForGroup!=nullptr) {
				foreach (Enums::MinerBaseType minerBaseType, minerAlgosForGroup->keys()) {
					QList<Algorithm*>* mbaseKvp=minerAlgosForGroup->value(minerBaseType);
					if (MinerReservedPorts->contains(minerBaseType)) {
						QList<Algorithm*>* algos=mbaseKvp;
						foreach (Algorithm* algo, *algos) {
							Enums::AlgorithmType algoType=algo->NiceHashID;
							QString path=MinerPaths::GetPathFor(minerBaseType, algoType, (Enums::DeviceGroupType)devGroupType);
							bool isPathValid= path!=MinerPaths::Data.NONE;
							if (isPathValid && !MinerReservedPorts->value(minerBaseType)->contains(path)) {
								MinerReservedPorts->value(minerBaseType)->insert(path, new QMap<Enums::AlgorithmType, QList<int>*>);
								}
							if (isPathValid && MinerReservedPorts->value(minerBaseType)->contains(path) && !MinerReservedPorts->value(minerBaseType)->value(path)->contains(algoType)) {
								MinerReservedPorts->value(minerBaseType)->value(path)->insert(algoType, new QList<int>);
								}
							}
						}
					}
				}
			}
		file->Commit(MinerReservedPorts);
		foreach (auto paths, *MinerReservedPorts) {
			foreach (auto algos, *paths) {
				foreach (auto ports, *algos) {
					foreach (int port, *ports) {
						AllReservedPorts->append(port);
						}
					}
				}
			}
		} catch (...) {}*/
}

void MinersSettingsManager::InitMinerSystemVariablesFile()
{
	MinerSystemVariablesConfigFile file;
	bool isFileInit=false;
	if (file.IsFileExists()) {
		MinerSystemVariablesConfig* read=file.ReadFile();
		if (read!=nullptr) {
			isFileInit=true;
			delete MinerSystemVariables;
			MinerSystemVariables=read;
			}
		}
	if (!isFileInit) {
		// save defaults
		MinerSystemVariables->Init();
		file.Commit(MinerSystemVariables);
		}
}
