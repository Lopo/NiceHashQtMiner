#include "DeviceBenchmarkConfigFile.h"
#include "FOLDERS.h"

#include <QVector>
#include <QChar>


const QString DeviceBenchmarkConfigFile::BenchmarkPrefix="benchmark_";

QString DeviceBenchmarkConfigFile::GetName(QString deviceUuid, QString old)
{
	// make device name
	QList<QChar> invalid={'<', '>', ':', '"', '/', '\\', '|', '?', '*'};
	QString fileName=BenchmarkPrefix+deviceUuid.replace(' ', '_');
	foreach (QChar c, invalid) {
		fileName=fileName.replace(c, "");
		}
	return fileName+old+".json";
}

DeviceBenchmarkConfigFile::DeviceBenchmarkConfigFile(QString deviceUuid)
	: ConfigFile(Folders::Config, GetName(deviceUuid), GetName(deviceUuid, "_OLD"))
{
}
