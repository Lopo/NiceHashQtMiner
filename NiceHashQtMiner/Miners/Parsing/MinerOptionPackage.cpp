#include "Miners/Parsing/MinerOptionPackage.h"
#include "Miners/Parsing/MinerOption.h"
#include <QMetaEnum>
#include <QJsonObject>
#include <QJsonArray>


MinerOptionPackage::MinerOptionPackage(Enums::MinerType iType, QList<MinerOption*>* iGeneralOptions, QList<MinerOption*>* iTemperatureOptions)
{
	Type=iType;
	GeneralOptions=iGeneralOptions;
	TemperatureOptions=iTemperatureOptions;
	Name=QMetaEnum::fromType<Enums::MinerType>().valueToKey((int)iType);
}

MinerOptionPackage* MinerOptionPackage::fromJson(QString json)
{
	QJsonDocument doc=QJsonDocument::fromJson(json.toLatin1());
	QJsonObject o=doc.object();
	if (o.value("Type")!=QJsonValue::Undefined
		&& o.value("GeneralOptions")!=QJsonValue::Undefined
		&& o.value("TemperatureOptions")!=QJsonValue::Undefined
		&& o.value("Name")!=QJsonValue::Undefined
		) {
		MinerOptionPackage* p=new MinerOptionPackage(
			(Enums::MinerType)o.value("Type").toInt(),
				optionsFromJsonArray(o.value("GeneralOptions").toArray()),
				optionsFromJsonArray(o.value("TemperatureOptions").toArray())
			);
		p->Name=o.value("Name").toString();
		return p;
		}
	return nullptr;
}

QString MinerOptionPackage::asJson(QJsonDocument::JsonFormat format)
{
	return QString();
}


QList<MinerOption*>* MinerOptionPackage::optionsFromJsonArray(QJsonArray a)
{
	QList<MinerOption*>* l=new QList<MinerOption*>();
	foreach (QJsonValue opt, a) {
		l->append(MinerOption::fromJsonObject(opt.toObject()));
		}
	return l;
}
