#ifndef MINERS_MININGSESSION_H_
#define MINERS_MININGSESSION_H_


#include "config.h"
#include "Enums.h"
#include "Switching/AlgorithmSwitchingManager.h"
#include <QTimer>
#include <QMap>
class IMainFormRatesComunication;
class ComputeDevice;
class MiningDevice;
class MiningPair;
class GroupMiner;
class NiceHashSma;


typedef QSet<QString> GroupedDevices;


class MiningSession
	: public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool IsMiningEnabled READ IsMiningEnabled)
	Q_PROPERTY(bool IsCurrentlyIdle READ IsCurrentlyIdle)
	Q_PROPERTY(QList<int>* ActiveDeviceIndexes READ ActiveDeviceIndexes)

	const QString Tag="MiningSession";

public:
	MiningSession(QList<ComputeDevice*>* devices, IMainFormRatesComunication* mainFormRatesCommunication, QString miningLocation, QString worker, QString btcAdress);
	bool IsMiningEnabled() const { return _miningDevices->count()>0; };
	QList<int>* ActiveDeviceIndexes();
	void StopAllMiners();
	void StopAllMinersNonProfitable();
	QString GetActiveMinersGroup();
	double GetTotalRate();
	void MinerStatsCheck();

private:
	// session variables fixed
	QString _miningLocation="";
	QString _btcAdress="";
	QString _worker="";
	QList<MiningDevice*>* _miningDevices=new QList<MiningDevice*>;
	IMainFormRatesComunication* _mainFormRatesCommunication=nullptr;

	AlgorithmSwitchingManager* _switchingManager=nullptr;

	// session variables changing
	// GroupDevices hash code doesn't work correctly use string instead
	QMap<QString, GroupMiner*>* _runningGroupMiners=new QMap<QString, GroupMiner*>;
#if WITH_NVIDIA
	GroupMiner* _ethminerNvidiaPaused=nullptr;
#endif
#if WITH_AMD
	GroupMiner* _ethminerAmdPaused=nullptr;
#endif

	bool _isProfitable=false;

	bool _isConnectedToInternet=false;
	bool _isMiningRegardlesOfProfit=false;

	// timers
	QTimer* _preventSleepTimer=nullptr;
	// check internet connection
	QTimer* _internetCheckTimer=nullptr;

	bool IsCurrentlyIdle() const { return !IsMiningEnabled() || !_isConnectedToInternet || !_isProfitable; };

	static QString CalcGroupedDevicesKey(const GroupedDevices group);
	bool CheckIfProfitable(double CurrentProfit, bool log=true);
	bool CheckIfShouldMine(double CurrentProfit, bool log=true);
	Enums::AlgorithmType GetMinerPairAlgorithmType(QList<MiningPair*>* miningPairs);

private Q_SLOTS:
	void InternetCheckTimer_Tick();
	void PreventSleepTimer_Tick();
	void SwichMostProfitableGroupUpMethod(SmaUpdateEventArgs);
};

#endif
