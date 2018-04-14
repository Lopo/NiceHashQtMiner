#ifndef STATS_NICEHASHSTATS_H_
#define STATS_NICEHASHSTATS_H_

#include "config.h"
#include "Algorithm.h"
#include "Utils/Helpers.h"
#include "Enums.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Utils/Links.h"
#include "Stats/NiceHashSocket.h"
#include <QObject>
#include <QString>
#include <QList>
#include <QMap>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTimer>

class NiceHashSma;


class NiceHashStats
	: public QObject
{
	Q_OBJECT
	Q_PROPERTY(double Balance READ Balance)
	Q_PROPERTY(QString Version READ Version)
public:
	class NicehashCredentials {
	public:
		QString method="credentials.set";
		QString btc;
		QString worker;
		QString asJson();
		};

	class NicehashDeviceStatus {
	public:
		QString method="devices.status";
		QList<QJsonArray>* devices=nullptr;
		QString asJson();
		};

	inline static double Balance() { return Balance_; };
	inline static QString Version() { return Version_; };

	static bool IsAlive();

	void StartConnection(QString address);
	static void SetCredentials(QString btc, QString worker);
	static QString GetNiceHashApiData(QString URL, QString worker);

private:
	const int DeviceUpdateLaunchDelay=20*1000;
	static const int DeviceUpdateInterval=60*1000;
	static double Balance_;
	static QString Version_;
	static NiceHashSocket* _socket;
	static QTimer* _deviceUpdateTimer;

	void SetAlgorithmRates(QJsonArray data);
	void SetStableAlgorithms(QJsonArray stable);
	void SetBalance(QString balance);
	void SetVersion(QString version);

Q_SIGNALS:
	void BalanceUpdate(double);
	void SmaUpdate();
	void VersionUpdate(QString);
	void ConnectionLost();
	void ConnectionEstablished();
	void VersionBurn(QString);

protected Q_SLOTS:
	void DeviceStatus_Tick();
	void SocketOnOnConnectionLost();
	void SocketOnOnDataReceived(QString e);
	void SocketOnOnConnectionEstablished();
};

#ifdef __NiceHashStats_cpp__
	NiceHashStats NiceHashStats;
#else
	extern NiceHashStats NiceHashStats;
#endif

#endif
