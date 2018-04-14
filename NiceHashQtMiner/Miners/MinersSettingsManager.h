#ifndef MINERS_MINERSSETTINGSMANAGER_H_
#define MINERS_MINERSSETTINGSMANAGER_H_

#include "Configs/ConfigJsonFile/ConfigFile.h"
#include "Configs/ConfigJsonFile/FOLDERS.h"
#include "Enums.h"
#include <QMap>
#include <QString>
#include <QList>
#include <QMetaEnum>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
class MinerSystemVariablesConfig;


class MinersSettingsManager
{
private:
	class MinerReservedPortsFile
		: public ConfigFile<QMap<Enums::MinerBaseType, QMap<QString, QMap<Enums::AlgorithmType, QList<int>*>*>*>>
	{
	public:
		MinerReservedPortsFile(): ConfigFile<QMap<Enums::MinerBaseType, QMap<QString, QMap<Enums::AlgorithmType, QList<int>*>*>*>>(Folders::Config, "MinerReservedPorts.json", "MinerReservedPorts_old.json") {};
		void Commit(QMap<Enums::MinerBaseType, QMap<QString, QMap<Enums::AlgorithmType, QList<int>*>*>*>* m) {
			CheckAndCreateConfigsFolder();
			try {
				QFile f(FilePath);
				f.open(QIODevice::Truncate|QIODevice::WriteOnly);
				QJsonObject obj;
				foreach (Enums::MinerBaseType mbt, m->keys()) {
					QJsonObject o2;
					foreach (QString s, m->value(mbt)->keys()) {
						QJsonObject o3;
						foreach (Enums::AlgorithmType at, m->value(mbt)->value(s)->keys()) {
							QJsonArray l;
							foreach (auto i, *m->value(mbt)->value(s)->value(at)) {
								l.append(i);
								}
							o3.insert(QMetaEnum::fromType<Enums::AlgorithmType>().valueToKey((int)at), l);
							}
						o2.insert(s, o3);
						}
					obj.insert(QMetaEnum::fromType<Enums::MinerBaseType>().valueToKey((int)mbt), o2);
					}
				f.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
				f.close();
				}
			catch (...) { }
			};
		};

	static QMap<Enums::MinerBaseType,
		QMap<QString,
			QMap<Enums::AlgorithmType,
				QList<int>*>*>*>* _minerReservedPorts;
public:
	static QList<int>* AllReservedPorts;
	static MinerSystemVariablesConfig* MinerSystemVariables;

	void Init();
	static QList<int>* GetPortsListFor(Enums::MinerBaseType minerBaseType, QString path, Enums::AlgorithmType algorithmType);
	void InitMinerReservedPortsFile();
	void InitMinerSystemVariablesFile();
};

#ifdef __MinersSettingsManager_cpp__
	MinersSettingsManager MinersSettingsManager;
#else
	extern MinersSettingsManager MinersSettingsManager;
#endif

#endif
