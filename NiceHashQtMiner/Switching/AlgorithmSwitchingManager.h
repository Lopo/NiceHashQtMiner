#ifndef SWITCHING_ALGORITHMSWITCHINGMANAGER_H_
#define SWITCHING_ALGORITHMSWITCHINGMANAGER_H_

#include "Switching/Interval.h"
#include "Enums.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include <QObject>
#include <QMutex>
#include <QTimer>
class AlgorithmHistory;


/**
 * Event args used for reporting fresh normalized profits
 */
class SmaUpdateEventArgs
{
public:
	SmaUpdateEventArgs(QMap<Enums::AlgorithmType, double>* profits);

	QMap<Enums::AlgorithmType, double> NormalizedProfits;
};


/**
 * Handles profit switching within a mining session
 */
class AlgorithmSwitchingManager
	: public QObject
{
	Q_OBJECT
public:
	AlgorithmSwitchingManager();

	inline static Interval StableRange() { return ConfigManager.generalConfig->SwitchSmaTicksStable; };
	inline static Interval UnstableRange() { return ConfigManager.generalConfig->SwitchSmaTicksUnstable; };
	inline static Interval SmaCheckRange() { return ConfigManager.generalConfig->SwitchSmaTimeChangeSeconds; };

	inline static int MaxHistory() { return std::max(StableRange().Upper, UnstableRange().Upper); };

	void Start();
	void Stop();

private:
	const QString Tag="SwitchingManager";

	QTimer* _smaCheckTimer=nullptr;

	int _ticksForStable;
	int _ticksForUnstable;
	double _smaCheckTime;

	QMap<Enums::AlgorithmType, AlgorithmHistory*>* _stableHistory;
	QMap<Enums::AlgorithmType, AlgorithmHistory*>* _unstableHistory;

	bool _hasStarted;

	QMap<Enums::AlgorithmType, double>* _lastLegitPaying; /// Currently used normalized profits

	bool UpdateProfits(QMap<Enums::AlgorithmType, AlgorithmHistory*>* history, int ticks, QStringList& sb);
	void Randomize();
	//QMutex _randomMtx=QMutex(QMutex::RecursionMode::Recursive);

	double LastPayingForAlgo(Enums::AlgorithmType algo);

Q_SIGNALS:
	void SmaCheck(SmaUpdateEventArgs); /// Emitted when the profits are checked

private Q_SLOTS:
	void SmaCheckTimerOnElapsed(); /// Checks profits and updates normalization based on ticks
};

#endif
