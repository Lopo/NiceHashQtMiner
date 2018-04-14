#ifndef MINERS_GROUPING_GROUPMINER_H_
#define MINERS_GROUPING_GROUPMINER_H_

#include "Enums.h"
#include <QObject>
class MiningPair;
class Miner;


class GroupMiner
	: public QObject
{
	Q_OBJECT
	Q_PROPERTY(Miner* Miner READ miner)
	Q_PROPERTY(QString DevicesInfoString READ DevicesInfoString)
	Q_PROPERTY(Enums::AlgorithmType AlgorithmType READ AlgorithmType)
	Q_PROPERTY(Enums::AlgorithmType DualAlgorithmType READ DualAlgorithmType)
	Q_PROPERTY(Enums::DeviceType DeviceType READ DeviceType) // for now used only for dagger identification AMD or NVIDIA
	Q_PROPERTY(QString Key READ Key)
	Q_PROPERTY(QList<int> DevIndexes READ DevIndexes)
public:
	GroupMiner(QList<MiningPair*>* miningPairs, QString key);

	inline Miner* miner() const { return _Miner; };
	inline QString DevicesInfoString() const { return _DevicesInfoString; };
	inline Enums::AlgorithmType AlgorithmType() const { return _AlgorithmType; };
	inline Enums::AlgorithmType DualAlgorithmType() const { return _DualAlgorithmType; };
	inline Enums::DeviceType DeviceType() const { return _DeviceType; };
	double CurrentRate=0;
	inline QString Key() const { return _Key; };
	inline QList<int>* DevIndexes() const { return _DevIndexes; };

	void Stop();
	void End();
	void Start(QString miningLocation, QString btcAdress, QString worker);

protected:
	Miner* _Miner=nullptr;

private:
	QString _DevicesInfoString;
	Enums::AlgorithmType _AlgorithmType=Enums::AlgorithmType::NONE;
	Enums::AlgorithmType _DualAlgorithmType=Enums::AlgorithmType::NONE;
	Enums::DeviceType _DeviceType;
	QString _Key;
	QList<int>* _DevIndexes=nullptr;
};

#endif
