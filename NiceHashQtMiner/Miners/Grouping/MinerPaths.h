#ifndef MINERS_GROUPING_MINERPATHS_H_
#define MINERS_GROUPING_MINERPATHS_H_

#include "config.h"
#include "Enums.h"
#include "Configs/ConfigJsonFile/ConfigFile.h"
#include "Configs/ConfigJsonFile/FOLDERS.h"
#include <QJsonDocument>
class Algorithm;
class ComputeDevice;


class MinerPath
{
public:
	MinerPath(Enums::AlgorithmType algo, QString path);
	MinerPath(QJsonObject obj);

	QString Name;
	Enums::AlgorithmType Algorithm;
	QString Path;

	QJsonObject asJsonObject();
};


class MinerTypePath
{
public:
	MinerTypePath(Enums::MinerBaseType type, QList<MinerPath> paths);
	MinerTypePath(QJsonObject obj);

	QString Name;
	Enums::MinerBaseType Type;
	QList<MinerPath> Algorithms;

	QJsonObject asJsonObject();
};


class MinerPathPackage
{
public:
	MinerPathPackage(Enums::DeviceGroupType type, QList<MinerTypePath*> paths);

	QString Name;
	Enums::DeviceGroupType DeviceType;
	QList<MinerTypePath*> MinerTypes;

	static MinerPathPackage* fromJson(QString json);
	QString asJson(QJsonDocument::JsonFormat=QJsonDocument::JsonFormat::Indented);
};


class MinerPathPackageFile
	: public ConfigFile<MinerPathPackage>
{
public:
	MinerPathPackageFile(QString name) : ConfigFile<MinerPathPackage>(Folders::Internals, name+".json", name+"_old.json") {};
};


class MinerPaths //done
{
public:
	static class Data {
	private:
		static const QString Bin; // root binary folder

	public:
		// ccminers
		static const QString CcminerDecred;

		static const QString CcminerNanashi;
		static const QString CcminerNeoscrypt;
		static const QString CcminerSp;
		static const QString CcminerTPruvot;
		static const QString CcminerCryptonight;
		static const QString CcminerX11Gost;
		static const QString CcminerKlausT;

		// ethminers
		static const QString Ethminer;

		// sgminers
		static const QString Sgminer560General;

		static const QString SgminerGm;

		static const QString NhEqMiner;
		static const QString Excavator;

		static const QString XmrStackCpuMiner;
		static const QString XmrStakAmd;
		static const QString XmrStak;
		static const QString Xmrig;
		static const QString XmrStakHeavy;

		static const QString CpuMiner;

		static const QString None;

		static const QString Bin3rdParty; // root binary folder

		static const QString ClaymoreZcashMiner;
		static const QString ClaymoreCryptoNightMiner;

		static const QString OptiminerZcashMiner;
		static const QString ClaymoreDual;
		static const QString Ewbf;
		static const QString Prospector;
		static const QString Dtsm;
		} Data;

	static QString GetPathFor(Enums::MinerBaseType minerBaseType, Enums::AlgorithmType algoType, Enums::DeviceGroupType devGroupType, bool def=false);
	static QString GetPathFor(ComputeDevice* computeDevice, Algorithm* algorithm);
	static bool IsValidMinerPath(QString* minerPath);
	static QList<Algorithm*>* GetAndInitAlgorithmsMinerPaths(QList<Algorithm*>* algos, ComputeDevice* computeDevice);
	static void InitializePackages();

private:
#if WITH_NVIDIA
	static class NvidiaGroups {
	private:
		static QString CcminerSM21(Enums::AlgorithmType algorithmType);
		static QString CcminerSM3X(Enums::AlgorithmType algorithmType);
		static QString CcminerSM5XOrSM6X(Enums::AlgorithmType algorithmType);
	public:
		static QString Ccminer_path(Enums::AlgorithmType algorithmType, Enums::DeviceGroupType nvidiaGroup);
		static QString CcminerUnstablePath(Enums::AlgorithmType algorithmType, Enums::DeviceGroupType nvidiaGroup);
		} NvidiaGroups;
#endif
#if WITH_AMD
	static class AmdGroup {
	public:
		static QString SgminerPath(Enums::AlgorithmType type);
		static QString ClaymorePath(Enums::AlgorithmType type);
		} AmdGroup;
#endif
	static class Experimental { // unstable miners, NVIDIA for now
	public:
		static QString GetPath(Enums::AlgorithmType algoType, Enums::DeviceGroupType devGroupType);
		} Experimental;

	static QList<MinerPathPackage*>* MinerPathPackages;
	static QList<Enums::MinerBaseType>* ConfigurableMiners;
};

#ifdef __MinerPaths_cpp__
	MinerPaths MinerPaths;
#else
	extern MinerPaths MinerPaths;
#endif

#endif
