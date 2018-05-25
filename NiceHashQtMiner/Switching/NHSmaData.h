#ifndef SWITCHING_NHSMADATA_H_
#define SWITCHING_NHSMADATA_H_

#include "Enums.h"
#include <QObject>
#include <QMutex>
class NiceHashSma;


class NHSmaData
	: public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool Initialized READ Initialized);
	Q_PROPERTY(bool HasData READ HasData) // True if there has been at least one SMA update
public:
	inline static bool Initialized() { return Initialized_; };
	inline static bool HasData() { return HasData_; };

	static void Initialize();
	static void InitializeIfNeeded();

	static void UpdateSmaPaying(QMap<Enums::AlgorithmType, double> newSma);
	static void UpdatePayingForAlgo(Enums::AlgorithmType algo, double paying);
	static void UpdateStableAlgorithms(QList<Enums::AlgorithmType> algorithms);

	static bool TryGetSma(Enums::AlgorithmType algo, NiceHashSma& sma);
	static bool TryGetPaying(Enums::AlgorithmType algo, double& paying);

	static bool IsAlgorithmStable(Enums::AlgorithmType algo);
	static QMap<Enums::AlgorithmType, double>* FilteredCurrentProfits(bool stable);

	static void CheckInit();

private:
	static const QString Tag;
	static const QString CachedFile;

	static bool Initialized_;
	static bool HasData_;

	static QMap<Enums::AlgorithmType, NiceHashSma*>* _currentSma; // Global list of SMA data, should be accessed with a lock since callbacks/timers update it
	static QSet<Enums::AlgorithmType>* _stableAlgorithms; // Global list of stable algorithms, should be accessed with a lock

	static QMutex _currentSmaMtx;
	static QMutex _stableAlgorithmsMtx;
};

#endif
