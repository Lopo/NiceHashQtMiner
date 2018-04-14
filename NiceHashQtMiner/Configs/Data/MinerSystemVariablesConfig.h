#ifndef CONFIGS_DATA_MINERSYSTEMVARIABLESCONFIG_H_
#define CONFIGS_DATA_MINERSYSTEMVARIABLESCONFIG_H_

#include <QMap>
#include <QJsonDocument>


class MinerSystemVariablesConfig
	: public QMap<QString, QMap<QString, QString>*>
{
public:
	void Init();
	static MinerSystemVariablesConfig* fromJson(QString json);
	QString asJson(QJsonDocument::JsonFormat format=QJsonDocument::JsonFormat::Indented);
};

#endif
