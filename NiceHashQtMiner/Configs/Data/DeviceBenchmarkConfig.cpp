#include "Configs/Data/DeviceBenchmarkConfig.h"

#include <QJsonObject>
#include <QJsonArray>


DeviceBenchmarkConfig* DeviceBenchmarkConfig::fromJson(QString json)
{
	DeviceBenchmarkConfig* gc=new DeviceBenchmarkConfig;
	QJsonObject o=QJsonDocument::fromJson(json.toLatin1()).object();

	if (o.value("DeviceUUID")!=QJsonValue::Undefined) {
		gc->DeviceUUID=o.value("DeviceUUID").toString();
		}
	if (o.value("DeviceName")!=QJsonValue::Undefined) {
		gc->DeviceName=o.value("DeviceName").toString();
		}
	if (o.value("AlgorithmSettings")!=QJsonValue::Undefined) {
		foreach (QJsonValue v, o.value("AlgorithmSettings").toArray()) {
			gc->AlgorithmSettings->append(AlgorithmConfig::fromJsonObject(v.toObject()));
			}
		}
	
	return gc;
}

QString DeviceBenchmarkConfig::asJson(QJsonDocument::JsonFormat format)
{
	QJsonObject o;
	o.insert("DeviceUUID", DeviceUUID);
	o.insert("DeviceName", DeviceName);
	QJsonArray as;
	foreach (AlgorithmConfig* ac, *AlgorithmSettings) {
		as.append(ac->asJsonObject());
		}
	o.insert("AlgorithmSettings", as);

	return QString(QJsonDocument(o).toJson(format));
}
