#ifndef MINERS_XMRSTAK_CONFIGS_XMRSTAKCONFIGPOOL_H_
#define MINERS_XMRSTAK_CONFIGS_XMRSTAKCONFIGPOOL_H_

#include "Miners/XmrStak/Configs/IasJsonObject.h"
#include <QString>
#include <QList>


class XmrStakConfigPool
	: public IasJsonObject
{
public:
	class XmrStakPoolItem {
		public:
			XmrStakPoolItem(QString pool, QString worker, int weight);

			QString pool_address;
			QString wallet_address;
			QString pool_password="x";
			bool use_nicehash=true;
			bool use_tls=false;
			QString tls_fingerprint="";
			int pool_weight=1;
			QString rig_id="";

			QJsonObject asJsonObject();
		};

	void SetupPools(QString poolAddr, QString wallet);
	void SetupPools(QStringList poolAddrs, QString wallet);

	QList<XmrStakPoolItem> pool_list;
	QString currency="monero7";

	QJsonObject asJsonObject() override;
};

#endif
