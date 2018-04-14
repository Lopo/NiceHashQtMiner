#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include <QObject>
#include <QString>
#include "Enums.h"


class Algorithm
	: public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool IsBenchmarkPending READ IsBenchmarkPending)
	Q_PROPERTY(QString CurPayingRatio READ CurPayingRatio)
	Q_PROPERTY(QString CurPayingRate READ CurPayingRate)
public:
	Algorithm(Enums::MinerBaseType minerBaseType, Enums::AlgorithmType niceHashID, QString minerName, Enums::AlgorithmType secondaryNiceHashID=Enums::AlgorithmType::NONE);

	QString AlgorithmName="";
	QString MinerBaseTypeName="";
	Enums::AlgorithmType NiceHashID=Enums::AlgorithmType::NONE;
	Enums::AlgorithmType SecondaryNiceHashID=Enums::AlgorithmType::NONE;
	Enums::MinerBaseType MinerBaseType=Enums::MinerBaseType::NONE;
	QString AlgorithmStringID="";

	// Miner name is used for miner ALGO flag parameter
	QString MinerName="";

	double BenchmarkSpeed=0;
	double SecondaryBenchmarkSpeed=0;
	QStringList ExtraLaunchParameters;
	bool Enabled=false;

	// CPU miners only setting
	int LessThreads=0;

	// average speed of same devices to increase mining stability
	double AveragedSpeed=0;

	double SecondaryAveragedSpeed=0;

	// based on device and settings here we set the miner path
	QString MinerBinaryPath="";

	// these are changing (logging reasons)
	double CurrentProfit=0;

	double CurNhmSmaDataVal=0;
	double SecondaryCurNhmSmaDataVal=0;

	// benchmark info
	QString BenchmarkStatus="";
	inline bool IsBenchmarkPending() const { return _IsBenchmarkPending; };
	QString CurPayingRatio();
	QString CurPayingRate();
	void SetBenchmarkPending();
	void SetBenchmarkPendingNoMsg();
	void ClearBenchmarkPending();
	void ClearBenchmarkPendingFirst();
	QString BenchmarkSpeedString();
	Enums::AlgorithmType DualNiceHashID(); // return hybrid type if dual, else standard ID
	bool IsDual();

private:
	bool _IsBenchmarkPending=false;

	bool IsPendingString();
};

#endif
