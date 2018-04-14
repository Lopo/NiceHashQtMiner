#include "Configs/Data/AlgorithmConfig.h"
#include "Enums.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>


AlgorithmConfig* AlgorithmConfig::fromJsonObject(QJsonObject o)
{
	AlgorithmConfig* ac=new AlgorithmConfig;
	if (o.value("Name")!=QJsonValue::Undefined) {
		ac->Name=o.value("Name").toString();
		}
	if (o.value("NiceHashID")!=QJsonValue::Undefined) {
		ac->NiceHashID=(Enums::AlgorithmType)o.value("NiceHashID").toInt();
		}
	if (o.value("SecondaryNiceHashID")!=QJsonValue::Undefined) {
		ac->SecondaryNiceHashID=(Enums::AlgorithmType)o.value("SecondaryNiceHashID").toInt();
		}
	if (o.value("MinerBaseType")!=QJsonValue::Undefined) {
		ac->MinerBaseType=(Enums::MinerBaseType)o.value("MinerBaseType").toInt();
		}
	if (o.value("MinerName")!=QJsonValue::Undefined) {
		ac->MinerName=o.value("MinerName").toString();
		}
	if (o.value("BenchmarkSpeed")!=QJsonValue::Undefined) {
		ac->BenchmarkSpeed=o.value("BenchmarkSpeed").toDouble();
		}
	if (o.value("SecondaryBenchmarkSpeed")!=QJsonValue::Undefined) {
		ac->SecondaryBenchmarkSpeed=o.value("SecondaryBenchmarkSpeed").toDouble();
		}
	if (o.value("ExtraLaunchParameters")!=QJsonValue::Undefined) {
		ac->ExtraLaunchParameters.clear();
		foreach (QJsonValue s, o.value("ExtraLaunchParameters").toArray()) {
			ac->ExtraLaunchParameters << s.toString();
			}
		}
	if (o.value("Enabled")!=QJsonValue::Undefined) {
		ac->Enabled=o.value("Enabled").toBool();
		}
	if (o.value("LessThreads")!=QJsonValue::Undefined) {
		ac->LessThreads=o.value("LessThreads").toInt();
		}
	return ac;
}

QJsonObject AlgorithmConfig::asJsonObject()
{
	QJsonObject o;
	o.insert("Name", Name);
	o.insert("NiceHashID", (int)NiceHashID);
	o.insert("SecondaryNiceHashID", (int)SecondaryNiceHashID);
	o.insert("MinerBaseType", (int)MinerBaseType);
	o.insert("MinerName", MinerName);
	o.insert("BenchmarkSpeed", BenchmarkSpeed);
	o.insert("SecondaryBenchmarkSpeed", SecondaryBenchmarkSpeed);
	o.insert("ExtraLaunchParameters", QJsonArray::fromStringList(ExtraLaunchParameters));
	o.insert("Enabled", Enabled);
	o.insert("LessThreads", LessThreads);
	return o;
}
