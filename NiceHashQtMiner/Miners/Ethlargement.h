#ifndef MINERS_ETHLARGEMENT_H_
#define MINERS_ETHLARGEMENT_H_

#include "Grouping/MiningSetup.h"
#include "PInvoke/NiceHashProcess.h"
#include <QObject>
#include <QProcess>
//#include <QMutex>
#include <mutex>


class Ethlargement
	: public QObject
{
	Q_OBJECT
public:
	/*static */void CheckAndStart(MiningSetup* setup);
	/*static */void Stop();

private:
	/*static */bool ShouldRun(MiningSetup* setup);

	/*static */NiceHashProcess* _process=nullptr;
	/*static */MiningSetup* _cachedSetup=nullptr;
//	/*static */QMutex Lock;
	std::mutex Lock;
	/*static */bool Running() {return _process!=nullptr && _process->state()!=QProcess::NotRunning;};

private Q_SLOTS:
	/*static */void ExitEvent();
};

#ifdef __Ethlargement_cpp__
	Ethlargement Ethlargement;
#else
	extern Ethlargement Ethlargement;
#endif

#endif
