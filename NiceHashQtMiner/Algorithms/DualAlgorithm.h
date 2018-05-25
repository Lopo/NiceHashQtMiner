#ifndef ALGORITHMS_DUALALGORITHM_H_
#define ALGORITHMS_DUALALGORITHM_H_

#include "Algorithms/Algorithm.h"
#include <QMap>
#include <set>


class DualAlgorithm
	: public Algorithm
{
	Q_OBJECT
public:
	DualAlgorithm(Enums::MinerBaseType minerBaseType, Enums::AlgorithmType niceHashID, Enums::AlgorithmType secondaryNiceHashID);

	Enums::AlgorithmType SecondaryNiceHashID() const override {return SecondaryNiceHashID_;};
	QString SecondaryAlgorithmName; // Friendly name for secondary algorithm
	double SecondaryCurNhmSmaDataVal=0; // Current SMA profitability for the secondary algorithm type in BTC/GH/Day
	Enums::AlgorithmType DualNiceHashID() const override;

	int CurrentIntensity=-1; // Current intensity while mining or benchmarking
	int TuningStart=25; // Lower bound for intensity tuning
	int TuningEnd=200; // Upper bound for intensity tuning
	int TuningInterval=25; // Interval for intensity tuning

	QMap<int, double> IntensitySpeeds; // Dictionary of intensity values to speeds in hashrates
	QMap<int, double> SecondaryIntensitySpeeds; // Dictionary of intensity values to secondary speeds in hashrates
	bool TuningEnabled=false; // Get or set whether intensity tuning is enabled

	/**
	 * Get or set whether intensity profitability is up to date
	 * This should generally be set to false if tuning speeds or SMA profits are changed
	 */
	bool IntensityUpToDate=false;

	/**
	 * Get the most profitable intensity value for this algorithm
	 * If IntensityUpToDate = false, intensity profit will be updated first
	 */
	int MostProfitableIntensity();

	std::set<int> AllIntensities();

	double BenchmarkSpeed() override;

	/**
	 * Secondary hashrate in H/s set by benchmark or user
	 * If tuning is enabled, returns the hashrate from the most profitable intensity
	 */
	double SecondaryBenchmarkSpeed();
	void SecondaryBenchmarkSpeed(double value);

	double SecondaryAveragedSpeed;
	bool BenchmarkNeeded() override;

	QMap<int, double> IntensityPowers; // Dictionary of intensity values and power usage for each
	bool UseIntensityPowers=false; // Get or set whether we should use different powers for intensities

	double PowerUsage() override;
	inline void PowerUsage(double value) override { Algorithm::PowerUsage(value);};

	QString CurPayingRate() override;

	QString SecondaryCurPayingRatio();
	QString SecondaryBenchmarkSpeedString();

	void UpdateCurProfit(QMap<Enums::AlgorithmType, double> profits) override;

	void SetIntensitySpeedsForCurrent(double speed, double secondarySpeed);
	void SetPowerForCurrent(double power);

	bool IncrementToNextEmptyIntensity();
	bool StartTuning();
	void ClearBenchmarkPendingFirst() override;
	double ProfitForIntensity(int intensity);
	double SpeedForIntensity(int intensity);
	double SecondarySpeedForIntensity(int intensity);
	QString SpeedStringForIntensity(int intensity);
	QString SecondarySpeedStringForIntensity(int intensity);
	void MakeIntensityBackup();
	void RestoreIntensityBackup();

private:
	Enums::AlgorithmType SecondaryNiceHashID_;
	QMap<int, double> _intensitySpeedsBack;
	QMap<int, double> _secondaryIntensitySpeedsBack;
	bool _tuningEnabledBack=false;
	int _tuningStartBack=0;
	int _tuningEndBack=0;
	int _tuningIntervalBack=0;

	int _mostProfitableIntensity=-1;

	std::set<int> SelectedIntensities(); // Sorted list of intensities that are selected for tuning

	double _secondaryBenchmarkSpeed;

	QMap<int, double> _intensityPowersBack;
	bool _useIntensityPowersBack=false;

	void UpdateProfitableIntensity();

	bool IsIntensityEmpty(int i);
};

#endif
