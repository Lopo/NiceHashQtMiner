#include "Configs/ConfigJsonFile/ConfigFile.h"

#include <QDir>
#include <QException>
#include <QJsonDocument>

#include "Utils/Helpers.h"

#ifdef __GNUG__
#	include <cxxabi.h>
#	include <memory>
#endif

template<typename T>
void ConfigFile<T>::CheckAndCreateConfigsFolder()
{
	try {
		if (!QDir(_confFolder).exists()) {
			QDir().mkdir(_confFolder);
			}
		}
	catch (...) { }
}

template<typename T>
ConfigFile<T>::ConfigFile(QString iConfFolder, QString fileName, QString fileNameOld)
{
	_confFolder=iConfFolder;
	FilePath= fileName.contains(_confFolder)
		? fileName
		: _confFolder+fileName;
	FilePathOld= fileNameOld.contains(_confFolder)
		? fileNameOld
		: _confFolder+fileNameOld;
	std::string Tname(typeid(T).name());
#ifdef __GNUG__
	std::size_t len=0;
	int status=0;
	std::unique_ptr<char, decltype(&std::free)> ptr(__cxxabiv1::__cxa_demangle(Tname.c_str(), nullptr, &len, &status), &std::free);
	Tname=ptr.get();
#endif
	_tag=QString(TagFormat).arg(Tname.c_str());
}

template<typename T>
bool ConfigFile<T>::IsFileExists()
{
	return QFile::exists(FilePath);
}

template<typename T>
T* ConfigFile<T>::ReadFile()
{
	CheckAndCreateConfigsFolder();
	T* file=nullptr;
	try {
		if (QFile::exists(FilePath)) {
			QFile fr(FilePath);
			fr.open(QIODevice::ReadOnly);
			file=T::fromJson(fr.readAll());
			fr.close();
			}
		}
	catch (QException& ex) {
		Helpers::ConsolePrint(_tag, QString("ReadFile %1: exception %2").arg(FilePath).arg(ex.what()));
		return nullptr;
		}
	return file;
}

template<typename T>
void ConfigFile<T>::Commit(T* file)
{
	CheckAndCreateConfigsFolder();
	if (file==nullptr) {
		Helpers::ConsolePrint(_tag, QString("Commit for FILE %1 IGNORED. Passed null object").arg(FilePath));
		return;
		}
	try {
		QFile f(FilePath);
		f.open(QIODevice::Truncate|QIODevice::WriteOnly);
		f.write(file->asJson(QJsonDocument::Indented).toLatin1());
		f.close();
		}
	catch (QException& ex) {
		Helpers::ConsolePrint(_tag, QString("Commit %1: exception %2").arg(FilePath).arg(ex.what()));
		}
}

template<typename T>
void ConfigFile<T>::CreateBackup()
{
	Helpers::ConsolePrint(_tag, QString("Backing up %1 to %2..").arg(FilePath).arg(FilePathOld));
	try {
		if (QFile::exists(FilePathOld)) {
			QFile::remove(FilePathOld);
			}
		QFile::copy(FilePath, FilePathOld);
		}
	catch (...) {
	}
}

#include "Configs/Data/GeneralConfig.h"
template class ConfigFile<GeneralConfig>;
#include "Configs/Data/DeviceBenchmarkConfig.h"
template class ConfigFile<DeviceBenchmarkConfig>;
#include "Miners/Parsing/MinerOptionPackage.h"
template class ConfigFile<MinerOptionPackage>;
#include "Configs/Data/MinerSystemVariablesConfig.h"
template class ConfigFile<MinerSystemVariablesConfig>;
#include "Miners/Grouping/MinerPaths.h"
template class ConfigFile<MinerPathPackage>;
