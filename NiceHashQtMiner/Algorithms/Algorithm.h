#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include "Enums.h"
#include <QObject>
#include <QString>


class Algorithm
	: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString AlgorithmName READ AlgorithmName) // Friendly display name for this algorithm
	Q_PROPERTY(QString AlgorithmStringID READ AlgorithmStringID) // Friendly name for this algorithm/miner combo
	Q_PROPERTY(double CurrentProfit READ CurrentProfit) // Current profit for this algorithm in BTC/Day
	Q_PROPERTY(double CurNhmSmaDataVal READ CurNhmSmaDataVal) // Current SMA profitability for this algorithm type in BTC/GH/Day
	Q_PROPERTY(double PowerUsage READ PowerUsage) // Power consumption of this algorithm, in Watts
	Q_PROPERTY(bool IsBenchmarkPending READ IsBenchmarkPending)
	Q_PROPERTY(QString CurPayingRatio READ CurPayingRatio)
	Q_PROPERTY(QString CurPayingRate READ CurPayingRate)
public:
	Algorithm(Enums::MinerBaseType minerBaseType, Enums::AlgorithmType niceHashID, QString minerName);

	QString AlgorithmName() const;
	QString MinerBaseTypeName=""; // Friendly name for miner type
	inline QString AlgorithmStringID() const { return AlgorithmStringID_;};
	Enums::AlgorithmType NiceHashID=Enums::AlgorithmType::NONE; // AlgorithmType used by this Algorithm
	Enums::MinerBaseType MinerBaseType=Enums::MinerBaseType::NONE; // MinerBaseType used by this algorithm
	QString MinerName=""; // Used for miner ALGO flag parameter

	virtual double BenchmarkSpeed() { return BenchmarkSpeed_;}; // Hashrate in H/s set by benchmark or user
	virtual void BenchmarkSpeed(double value) { BenchmarkSpeed_=value;};
	/** Gets the averaged speed for this algorithm in H/s;
	 * When multiple devices of the same model are used, this will be set to their averaged hashrate
	 */
	double AvaragedSpeed=0;
	QStringList ExtraLaunchParameters; // String containing raw extralaunchparams entered by user
	bool Enabled=false; // Get or set whether this algorithm is enabled for mining

	int LessThreads=0; // TODO not needed with new xmr-stak?

	/**
	 * Path to the miner executable
	 * Path may differ for the same miner/algo combos depending on devices and user settings
	 */
	QString MinerBinaryPath="";

	virtual bool BenchmarkNeeded(); // Indicates whether this algorithm requires a benchmark

	inline double CurrentProfit() const { return CurrentProfit_;};
	inline double CurNhmSmaDataVal() const { return CurNhmSmaDataVal_;};
	inline virtual double PowerUsage() { return PowerUsage_;};
	inline virtual void PowerUsage(double value) { PowerUsage_=value;};

	virtual Enums::AlgorithmType SecondaryNiceHashID() const {return Enums::AlgorithmType::NONE;};
	virtual Enums::AlgorithmType DualNiceHashID() const {return NiceHashID;};
	
	QString BenchmarkStatus="";
	inline bool IsBenchmarkPending() const { return IsBenchmarkPending_;};
	QString CurPayingRatio();
	virtual QString CurPayingRate();
	void SetBenchmarkPending();
	void SetBenchmarkPendingNoMsg();
	void ClearBenchmarkPending();
	virtual void ClearBenchmarkPendingFirst();
	QString BenchmarkSpeedString();

	virtual void UpdateCurProfit(QMap<Enums::AlgorithmType, double> profits);

protected:
	const double Mult=0.000000001; // Used for converting SMA values to BTC/H/Day
	QString AlgorithmName_;
	QString AlgorithmStringID_;
	double CurrentProfit_=0;

	void SubtractPowerFromProfit();

private:
	double BenchmarkSpeed_=0;
	double CurNhmSmaDataVal_=0;
	double PowerUsage_=0;

	bool IsBenchmarkPending_=false;

	bool IsPendingString();
};

#endif
