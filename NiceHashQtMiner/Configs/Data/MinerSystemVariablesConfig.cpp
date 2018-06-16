#include "config.h"
#include "Configs/Data/MinerSystemVariablesConfig.h"
#include "Miners/Grouping/MinerPaths.h"
#include <QJsonObject>


void MinerSystemVariablesConfig::Init()
{
	{ // general AMD defaults scope
		QStringList minerPaths={
#if WITH_AMD
			MinerPaths::Data.Sgminer560General,
			MinerPaths::Data.SgminerGm,
			MinerPaths::Data.ClaymoreCryptoNightMiner,
			MinerPaths::Data.ClaymoreZcashMiner,
#endif
			MinerPaths::Data.OptiminerZcashMiner
			};
		foreach (QString minerPath, minerPaths) {
			insert(minerPath, new QMap<QString, QString>({
				{"GPU_MAX_ALLOC_PERCENT", "100"},
				{"GPU_USE_SYNC_OBJECTS", "1"},
				{"GPU_SINGLE_ALLOC_PERCENT", "100"},
				{"GPU_MAX_HEAP_SIZE", "100"},
				{"GPU_FORCE_64BIT_PTR", "1"}
				}));
			}
	}
	{ // ClaymoreDual scope
		insert(MinerPaths::Data.ClaymoreDual, new QMap<QString, QString>({
			{"GPU_MAX_ALLOC_PERCENT", "100"},
			{"GPU_USE_SYNC_OBJECTS", "1"},
			{"GPU_SINGLE_ALLOC_PERCENT", "100"},
			{"GPU_MAX_HEAP_SIZE", "100"},
			{"GPU_FORCE_64BIT_PTR", "0"}
			}));
	}
}

MinerSystemVariablesConfig* MinerSystemVariablesConfig::fromJson(QString json)
{
	QJsonParseError e;
	QJsonDocument d=QJsonDocument::fromJson(json.toLatin1(), &e);
	if (e.error) {
		return nullptr;
		}
	MinerSystemVariablesConfig* c=new MinerSystemVariablesConfig;
	QJsonObject o=d.object();
	foreach (QString pKey, o.keys()) {
		QJsonObject so=o.value(pKey).toObject();
		QMap<QString, QString>* iMap=new QMap<QString, QString>;
		foreach (QString sKey, so.keys()) {
			iMap->insert(sKey, so.value(sKey).toString());
			}
		c->insert(pKey, iMap);
		}
	return c;
}

QString MinerSystemVariablesConfig::asJson(QJsonDocument::JsonFormat format)
{
	QJsonObject o;
	foreach (QString pKey, keys()) {
		QJsonObject t;
		foreach (QString iKey, value(pKey)->keys()) {
			t.insert(iKey, value(pKey)->value(iKey));
			}
		o.insert(pKey, t);
		}

	return QString(QJsonDocument(o).toJson(format));
}
