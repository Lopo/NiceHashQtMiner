#ifndef CONFIGS_CONFIGMANAGER_H_
#define CONFIGS_CONFIGMANAGER_H_

#include <QObject>
#include <QMap>
#include <QString>


class GeneralConfig;
class GeneralConfigFile;
class DeviceBenchmarkConfig;
class DeviceBenchmarkConfigFile;

class ConfigManager
	: public QObject
{
	Q_OBJECT
public:
	static GeneralConfig* generalConfig;

	static void InitializeConfig();
	static bool GeneralConfigIsFileExist();
	static void CreateBackup();
	static void RestoreBackup();
	static bool IsRestartNeeded();
	static void GeneralConfigFileCommit();
	static void CommitBenchmarks();
	static void AfterDeviceQueryInitialization();

private:
	static const QString Tag;

	//helper variables
	static bool _isGeneralConfigFileInit;
	static bool _isNewVersion;

	// for loading and saving
	static GeneralConfigFile* generalConfigFile;
	static QMap<QString, DeviceBenchmarkConfigFile*>* BenchmarkConfigFiles;

	// backups
	static GeneralConfig* _generalConfigBackup;
	static QMap<QString, DeviceBenchmarkConfig*>* _benchmarkConfigsBackup;
};

#ifdef __ConfigManager_cpp__
	ConfigManager ConfigManager;
#else
	extern ConfigManager ConfigManager;
#endif

#endif
