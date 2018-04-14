#include "Miners/Parsing/MinerOption.h"
#include <QJsonObject>


MinerOption::MinerOption(QString iType, QString iShortName,  QString iLongName, QString iDefault, Enums::MinerOptionFlagType iFlagType, QString iSeparator/*=""*/)
{
	Type=iType;
	ShortName=iShortName;
	LongName=iLongName;
	Default=iDefault;
	FlagType=iFlagType;
	Separator=iSeparator;
}

MinerOption::MinerOption(QString iType, QString iLongName, QString iDefault, Enums::MinerOptionFlagType iFlagType, QString iSeparator/*=""*/)
{
	Type=iType;
	ShortName=iLongName;
	LongName=iLongName;
	Default=iDefault;
	FlagType=iFlagType;
	Separator=iSeparator;
}

MinerOption* MinerOption::fromJsonObject(QJsonObject o)
{
	return new MinerOption(
		o.value("Type").toString(),
		o.value("ShortName").toString(),
		o.value("LongName").toString(),
		o.value("Default").toString(),
		(Enums::MinerOptionFlagType)o.value("Type").toInt(),
		o.value("Separator").toString()
		);
}
