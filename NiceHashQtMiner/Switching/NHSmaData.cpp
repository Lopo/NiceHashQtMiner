#include "Switching/NHSmaData.h"
#include "Enums.h"
#include "Switching/NiceHashSma.h"
#include "Utils/Helpers.h"
#include <QSet>
#include <QMap>
#include <QMetaEnum>
#include <QVector>
#include <exception>


const QString NHSmaData::Tag="NHSMAData";
bool NHSmaData::Initialized_=false;
bool NHSmaData::HasData_=false;

QMap<Enums::AlgorithmType, NiceHashSma*>* NHSmaData::_currentSma=nullptr;
QSet<Enums::AlgorithmType>* NHSmaData::_stableAlgorithms=nullptr;

QMutex NHSmaData::_currentSmaMtx(QMutex::RecursionMode::Recursive);
QMutex NHSmaData::_stableAlgorithmsMtx(QMutex::RecursionMode::Recursive);


void NHSmaData::Initialize()
{
	_currentSma=new QMap<Enums::AlgorithmType, NiceHashSma*>;
	_stableAlgorithms=new QSet<Enums::AlgorithmType>;

	auto algoEnum=QMetaEnum::fromType<Enums::AlgorithmType>();
	for (int i=0; i<algoEnum.keyCount(); i++) {
		Enums::AlgorithmType algo=(Enums::AlgorithmType)algoEnum.value(i);
		if (algo>=0) {
			NiceHashSma* nhs=new NiceHashSma;
			nhs->Port=(int)algo+3333;
			nhs->Name=algoEnum.valueToKey(algo);
			nhs->Algo=(int)algo;
			nhs->Paying=0;
			_currentSma->insert(algo, nhs);
			}
		}
	Initialized_=true;
}

void NHSmaData::InitializeIfNeeded()
{
	if (!Initialized_) {
		Initialize();
		}
}

/**
 * Change SMA profits to new values
 *
 * @param newSma Algorithm/profit dictionary with new values
 */
void NHSmaData::UpdateSmaPaying(QMap<Enums::AlgorithmType, double> newSma)
{
	CheckInit();
	bool l=_currentSmaMtx.tryLock();
Q_ASSERT(l);
		foreach (Enums::AlgorithmType algo, newSma.keys()) {
			if (_currentSma->contains(algo)) {
				_currentSma->value(algo)->Paying=newSma.value(algo);
				}
			}
	_currentSmaMtx.unlock();

	HasData_=true;
}

/**
 * Change SMA profit for one algo
 *
 * @param algo
 * @param paying
 */
void NHSmaData::UpdatePayingForAlgo(Enums::AlgorithmType algo, double paying)
{
	CheckInit();
	bool l=_currentSmaMtx.tryLock();
Q_ASSERT(l);
		if (!_currentSma->contains(algo)) {
			throw std::invalid_argument("Algo not setup in SMA");
			}
		_currentSma->value(algo)->Paying=paying;
	_currentSmaMtx.unlock();

	HasData_=true;
}

/**
 * Update list of stable algorithms
 *
 * @param algorithms Algorithms that are stable
 */
void NHSmaData::UpdateStableAlgorithms(QList<Enums::AlgorithmType> algorithms)
{
	CheckInit();
	QStringList sb;
	sb << "Updating stable algorithms";
	bool hasChange=false;

	bool l=_stableAlgorithmsMtx.tryLock();
Q_ASSERT(l);
		QVector<Enums::AlgorithmType> algosEnumd=algorithms.toVector();
		foreach (Enums::AlgorithmType algo, algosEnumd) {
			if (!_stableAlgorithms->contains(algo)) {
				_stableAlgorithms->insert(algo);
				sb << QString("\tADDED %1").arg(algo);
				hasChange=true;
				}
			}

		foreach (Enums::AlgorithmType algo, *_stableAlgorithms) {
			if (algosEnumd.contains(algo)) {
				break;
				}

			_stableAlgorithms->remove(algo);
			sb << QString("\tREMOVED %1").arg(algo);
			hasChange=true;
			break;;
			}
	_stableAlgorithmsMtx.unlock();
	if (!hasChange) {
		sb << "\tNone changed";
		}
	Helpers::ConsolePrint(Tag, sb.join('\n'));
}

/**
 * Attempt to get SMA for an algorithm
 *
 * @param algo Algorithm
 * @param sma Variable to place SMA in
 * @return True iff we know about this algo
 */
bool NHSmaData::TryGetSma(Enums::AlgorithmType algo, NiceHashSma* sma)
{
	CheckInit();
	bool l=_currentSmaMtx.tryLock();
Q_ASSERT(l);
		if (_currentSma->contains(algo)) {
			sma=_currentSma->value(algo);
			return true;
			}
	_currentSmaMtx.unlock();

	sma=nullptr;
	return false;
}

/**
 * Attempt to get paying rate for an algorithm
 *
 * @param algo Algorithm
 * @param paying Variable to place paying in
 * @return True iff we know about this algo
 */
bool NHSmaData::TryGetPaying(Enums::AlgorithmType algo, double& paying)
{
	CheckInit();
	NiceHashSma* sma;
	if (TryGetSma(algo, sma)) {
		paying=sma->Paying;
		return true;
		}

	paying=0.0d;
	return false;
}

bool NHSmaData::IsAlgorithmStable(Enums::AlgorithmType algo)
{
	CheckInit();
	bool l=_stableAlgorithmsMtx.tryLock();
Q_ASSERT(l);
		return _stableAlgorithms->contains(algo);
	_stableAlgorithmsMtx.unlock();
}

/**
 * Filters SMA profits based on whether the algorithm is stable
 *
 * @param stable True to get stable, false to get unstable
 * @return Filtered Algorithm/double map
 */
QMap<Enums::AlgorithmType, double>* NHSmaData::FilteredCurrentProfits(bool stable)
{
	CheckInit();
	QMap<Enums::AlgorithmType, double>* dict=new QMap<Enums::AlgorithmType, double>;

	bool l=_currentSmaMtx.tryLock();
Q_ASSERT(l);
		foreach (Enums::AlgorithmType kvpKey, _currentSma->keys()) {
			if (_stableAlgorithms->contains(kvpKey)==stable) {
				dict->insert(kvpKey, _currentSma->value(kvpKey)->Paying);
				}
			}
	_currentSmaMtx.unlock();

	return dict;
}

/**
 * Helper to ensure initialization
 */
void NHSmaData::CheckInit()
{
	if (!Initialized_) {
		throw std::logic_error("NHSmaData cannot be used before initialization");
		}
}
