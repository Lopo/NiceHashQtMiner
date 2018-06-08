#include "Miners/XmrStak/Configs/XmrStakConfigPool.h"
#include <QJsonArray>


XmrStakConfigPool::XmrStakPoolItem::XmrStakPoolItem(QString pool, QString worker, int weight)
{
	pool_address=pool;
	wallet_address=worker;
	pool_weight=weight;
}

void XmrStakConfigPool::SetupPools(QString poolAddr, QString wallet, bool isHeavy)
{
	SetupPools(QStringList({poolAddr}), wallet, isHeavy);
}

void XmrStakConfigPool::SetupPools(QStringList poolAddrs, QString wallet, bool isHeavy)
{
	pool_list.clear();
	int i=1;
	foreach (QString poolAddr, poolAddrs) {
		pool_list.append(XmrStakPoolItem(poolAddr, wallet, i));
		i++;
		}

	if (isHeavy) {
		currency="cryptonight_heavy";
		}
}

QJsonObject XmrStakConfigPool::XmrStakPoolItem::asJsonObject()
{
	QJsonObject o;
	o.insert("pool_address", pool_address);
	o.insert("wallet_address", wallet_address);
	o.insert("pool_password", pool_password);
	o.insert("use_nicehash", use_nicehash);
	o.insert("use_tls", use_tls);
	o.insert("tls_fingerprint", tls_fingerprint);
	o.insert("pool_weight", pool_weight);
	o.insert("rig_id", rig_id);
	return o;
}

QJsonObject XmrStakConfigPool::asJsonObject()
{
	QJsonObject o;
	QJsonArray plArr;
	foreach (XmrStakPoolItem pi, pool_list) {
		plArr.append(pi.asJsonObject());
		}
	o.insert("pool_list", plArr);
	o.insert("currency", currency);
	return o;
}
