#ifndef MINERS_PARSING_MINEROPTIONPACKAGE_H_
#define MINERS_PARSING_MINEROPTIONPACKAGE_H_

#include "Enums.h"
#include <QJsonDocument>
class MinerOption;


class MinerOptionPackage
	: public QObject
{
	Q_OBJECT
public:
	MinerOptionPackage(Enums::MinerType iType, QList<MinerOption*>* iGeneralOptions, QList<MinerOption*>* iTemperatureOptions);

	QString Name;
	Enums::MinerType Type;
	QList<MinerOption*>* GeneralOptions=nullptr;
	QList<MinerOption*>* TemperatureOptions=nullptr;

	static MinerOptionPackage* fromJson(QString json);
	QString asJson(QJsonDocument::JsonFormat format=QJsonDocument::JsonFormat::Indented);

private:
	static QList<MinerOption*>* optionsFromJsonArray(QJsonArray a);
};

#endif
