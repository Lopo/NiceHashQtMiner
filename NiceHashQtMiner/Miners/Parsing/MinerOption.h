#ifndef MINERS_PARSING_MINEROPTION_H_
#define MINERS_PARSING_MINEROPTION_H_

#include "Enums.h"


class MinerOption
	: public QObject
{
	Q_OBJECT
public:
	MinerOption(QString iType, QString iShortName,  QString iLongName, QString iDefault, Enums::MinerOptionFlagType iFlagType, QString iSeparator="");
	MinerOption(QString iType, QString iLongName, QString iDefault, Enums::MinerOptionFlagType iFlagType, QString iSeparator=""); // Constructor if no short name

	static MinerOption* fromJsonObject(QJsonObject o);

	QString Type;
	QString ShortName;
	QString LongName;
	QString Default;
	Enums::MinerOptionFlagType FlagType;
	QString Separator;
};

#endif
