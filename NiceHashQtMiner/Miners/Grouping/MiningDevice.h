#ifndef MINERS_GROUPING_MININGDEVICE_H_
#define MINERS_GROUPING_MININGDEVICE_H_

#include "config.h"
#include "Enums.h"
#include <QObject>

class NiceHashSma;
class ComputeDevice;
class Algorithm;
class MiningPair;


class MiningDevice
	: public QObject
{
	Q_OBJECT
	// switch testing quick and dirty, runtime versions
	Q_PROPERTY(ComputeDevice* Device READ Device())
	Q_PROPERTY(Enums::AlgorithmType MostProfitableAlgorithmType READ MostProfitableAlgorithmType)
	Q_PROPERTY(Enums::MinerBaseType MostProfitableMinerBaseType READ MostProfitableMinerBaseType)
	Q_PROPERTY(Enums::AlgorithmType PrevProfitableAlgorithmType READ PrevProfitableAlgorithmType)
	Q_PROPERTY(Enums::MinerBaseType PrevProfitableMinerBaseType READ PrevProfitableMinerBaseType)
	Q_PROPERTY(double GetCurrentMostProfitValue READ GetCurrentMostProfitValue)
	Q_PROPERTY(double GetPrevMostProfitValue READ GetPrevMostProfitValue)
public:
	MiningDevice(ComputeDevice* device);
	inline ComputeDevice* Device() const { return _Device; };
	QList<Algorithm*>* Algorithms=new QList<Algorithm*>;
	QString GetMostProfitableString();
	inline Enums::AlgorithmType MostProfitableAlgorithmType() const { return _MostProfitableAlgorithmType; };
	inline Enums::MinerBaseType MostProfitableMinerBaseType() const { return _MostProfitableMinerBaseType; };
	inline Enums::AlgorithmType PrevProfitableAlgorithmType() const { return _PrevProfitableAlgorithmType; };
	inline Enums::MinerBaseType PrevProfitableMinerBaseType() const { return _PrevProfitableMinerBaseType; };
	double GetCurrentMostProfitValue();
	double GetPrevMostProfitValue();
	MiningPair* GetMostProfitablePair();
	bool HasProfitableAlgo();
	void RestoreOldProfitsState();
	void SetNotMining();
	void CalculateProfits(QMap<Enums::AlgorithmType, double> profits);
#if (SWITCH_TESTING)
	static int next;
	static void SetNextTest();
	static bool ForceNone;
	static int seconds;
	static int SMAMinerCheckInterval;
	static bool ForcePerCardMiners;
#endif

private:
	ComputeDevice* _Device=nullptr;
	Enums::AlgorithmType _MostProfitableAlgorithmType;
	Enums::MinerBaseType _MostProfitableMinerBaseType;
	Enums::AlgorithmType _PrevProfitableAlgorithmType;
	Enums::MinerBaseType _PrevProfitableMinerBaseType;

	int GetMostProfitableIndex();
	int GetPrevProfitableIndex();
#if (SWITCH_TESTING)
	static QList<Enums::AlgorithmType> testingAlgos;
#endif

};

#endif
