#include "Switching/AlgorithmSwitchingManager.h"
#include "Switching/NHSmaData.h"
#include "Utils/Helpers.h"
#include <Switching/AlgorithmHistory.h>
#include <QTimer>
#include <QMetaEnum>


AlgorithmSwitchingManager::AlgorithmSwitchingManager()
{
	_stableHistory=new QMap<Enums::AlgorithmType, AlgorithmHistory*>;
	_unstableHistory=new QMap<Enums::AlgorithmType, AlgorithmHistory*>;
	_lastLegitPaying=new QMap<Enums::AlgorithmType, double>;

	QMap<Enums::AlgorithmType, double>* fcp=NHSmaData::FilteredCurrentProfits(true);
	foreach (Enums::AlgorithmType kvpKey, fcp->keys()) {
		_stableHistory->insert(kvpKey, new AlgorithmHistory(MaxHistory()));
		_lastLegitPaying->insert(kvpKey, fcp->value(kvpKey));
		}
	fcp=NHSmaData::FilteredCurrentProfits(false);
	foreach (Enums::AlgorithmType kvpKey, fcp->keys()) {
		_unstableHistory->insert(kvpKey, new AlgorithmHistory(MaxHistory()));
		_lastLegitPaying->insert(kvpKey, fcp->value(kvpKey));
		}
}

void AlgorithmSwitchingManager::Start()
{
	_smaCheckTimer=new QTimer;
	_smaCheckTimer->setInterval(100);
	connect(_smaCheckTimer, SIGNAL(timeout()), this, SLOT(SmaCheckTimerOnElapsed()));

	_smaCheckTimer->start();
}

void AlgorithmSwitchingManager::Stop()
{
	_smaCheckTimer->stop();
	delete _smaCheckTimer;
	_smaCheckTimer=nullptr;
}

void AlgorithmSwitchingManager::SmaCheckTimerOnElapsed()
{
	Randomize();

	// Will be null if manually called (in tests)
	if (_smaCheckTimer!=nullptr) {
		_smaCheckTimer->setInterval(_smaCheckTime*1000);
		}

	QStringList sb;

	if (_hasStarted) {
		sb << "Normalizing profits";
		}

	bool stableUpdated=UpdateProfits(_stableHistory, _ticksForStable, sb);
	bool unstableUpdated=UpdateProfits(_unstableHistory, _ticksForUnstable, sb);

	if (!stableUpdated && !unstableUpdated && _hasStarted) {
		sb << "No algos affected (either no SMA update or no algos higher";
		}

	if (_hasStarted) {
		Helpers::ConsolePrint(Tag, sb.join('\n'));
		}
	else {
		_hasStarted=true;
		}

	Q_EMIT SmaCheck(SmaUpdateEventArgs(_lastLegitPaying));
}

/**
 * Check profits for a history dict and update if profit has been higher for required ticks or if it is lower
 *
 * @param history
 * @param ticks
 * @param sb
 * @return True iff any profits were postponed or updated
 */
bool AlgorithmSwitchingManager::UpdateProfits(QMap<Enums::AlgorithmType, AlgorithmHistory*>* history, int ticks, QStringList& sb)
{
	bool updated=false;

	foreach (Enums::AlgorithmType algo, history->keys()) {
		double paying;
		if (NHSmaData::TryGetPaying(algo, paying)) {
			history->value(algo)->Add(paying);
			if (paying>_lastLegitPaying->value(algo)) {
				updated=true;
				int i=history->value(algo)->CountOverProfit(_lastLegitPaying->value(algo));
				if (i>=ticks) {
					_lastLegitPaying->insert(algo, paying);
					sb << QString("\tTAKEN: new profit %1 after %2 ticks for %3").arg(QString::number(paying, 'e', 5)).arg(i).arg(QMetaEnum::fromType<Enums::AlgorithmType>().valueToKey((int)algo));
					}
				else {
					sb << QString("\tPOSTPONED: new profit %1 (previously %2), higher for %3/%4 ticks for %5").arg(QString::number(paying, 'e', 5)).arg(QString::number(_lastLegitPaying->value(algo), 'e', 5)).arg(i).arg(ticks).arg(QMetaEnum::fromType<Enums::AlgorithmType>().valueToKey((int)algo));
					}
				}
			else {
				// Profit has gone down
				_lastLegitPaying->insert(algo, paying);
				}
			}
		}

	return updated;
}

void AlgorithmSwitchingManager::Randomize()
{
	// Lock in case this gets called simultaneously
//	_randomMtx.lock();
		_ticksForStable=StableRange().RandomInt();
		_ticksForUnstable=UnstableRange().RandomInt();
		_smaCheckTime=SmaCheckRange().RandomInt();
//	_randomMtx.unlock();
}

double AlgorithmSwitchingManager::LastPayingForAlgo(Enums::AlgorithmType algo)
{
	return _lastLegitPaying->value(algo);
}


SmaUpdateEventArgs::SmaUpdateEventArgs(QMap<Enums::AlgorithmType, double>* profits)
{
	NormalizedProfits=*profits;
}
