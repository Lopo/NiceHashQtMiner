#ifndef CONFIGS_CONFIGJSONFILE_CONFIGFILE_H_
#define CONFIGS_CONFIGJSONFILE_CONFIGFILE_H_

#include <QString>


template<typename T>
class ConfigFile
{
private:
	const QString TagFormat="ConfigFile<%1>";
	QString _confFolder; // ="configs/"
	QString _tag;

protected:
	void CheckAndCreateConfigsFolder();

	QString FilePath;
	QString FilePathOld;

	ConfigFile(QString iConfFolder, QString fileName, QString fileNameOld);

public:
	bool IsFileExists();
	T* ReadFile();
	void Commit(T* file);
	void CreateBackup();
};

#endif
