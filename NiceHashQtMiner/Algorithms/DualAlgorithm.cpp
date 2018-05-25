#include "Algorithms/DualAlgorithm.h"
#include "DualAlgorithm.h"
#include "Utils/AlgorithmNiceHashNames.h"
#include "International.h"
#include "Switching/NHSmaData.h"
#include "Utils/Helpers.h"


Enums::AlgorithmType DualAlgorithm::DualNiceHashID() const
{
	return Helpers::DualAlgoFromAlgos(NiceHashID, SecondaryNiceHashID());
}

int DualAlgorithm::MostProfitableIntensity()
{
	if (!IntensityUpToDate) { // UpdateProfitableIntensity() can take some time, so we store most profitable and only update when needed
		UpdateProfitableIntensity();
		}
	return _mostProfitableIntensity;
}

std::set<int> DualAlgorithm::SelectedIntensities()
{
	std::set<int> list;
	for (int i=TuningStart; i<=TuningEnd; i+=TuningInterval) {
		list.insert(i);
		}
	return list;
}

std::set<int> DualAlgorithm::AllIntensities()
{
	std::set<int> ret;
	foreach (int i, IntensitySpeeds.keys()) {
		ret.insert(i);
		}
	foreach (int i, SecondaryIntensitySpeeds.keys()) {
		ret.insert(i);
		}
	foreach (int i, SelectedIntensities()) {
		ret.insert(i);
		}
	return ret;
}

double DualAlgorithm::BenchmarkSpeed()
{
	if (MostProfitableIntensity()>0) {
		try {
			return IntensitySpeeds[MostProfitableIntensity()];
			}
		catch (std::exception& e) {
			Helpers::ConsolePrint("CDTUNING", e.what());
			IntensityUpToDate=false;
			return 0;
			}
		}
	return Algorithm::BenchmarkSpeed();
}

double DualAlgorithm::SecondaryBenchmarkSpeed()
{
	if (MostProfitableIntensity()>0) {
		try {
			return SecondaryIntensitySpeeds[MostProfitableIntensity()];
			}
		catch (std::exception& e) {
			Helpers::ConsolePrint("CDTUNING", e.what());
			IntensityUpToDate=false;
			return 0;
			}
		}
	return _secondaryBenchmarkSpeed;
}

inline void DualAlgorithm::SecondaryBenchmarkSpeed(double value)
{
	_secondaryBenchmarkSpeed=value;
}

bool DualAlgorithm::BenchmarkNeeded()
{
	if (TuningEnabled) {
		if (SelectedIntensities().empty()) {
			return true;
			}
		foreach (int i, SelectedIntensities()) {
			if (IsIntensityEmpty(i)) {
				return true;
				}
			}
		}
	else {
		if (SecondaryBenchmarkSpeed()<=0 || BenchmarkSpeed()<=0) {
			return true;
			}
		}
	return false;
}

double DualAlgorithm::PowerUsage()
{
	if (UseIntensityPowers && MostProfitableIntensity()>0 && IntensityPowers.contains(MostProfitableIntensity())) {
		return IntensityPowers.value(MostProfitableIntensity());
		}
	return Algorithm::PowerUsage();
}

DualAlgorithm::DualAlgorithm(Enums::MinerBaseType minerBaseType, Enums::AlgorithmType niceHashID, Enums::AlgorithmType secondaryNiceHashID)
	: Algorithm(minerBaseType, niceHashID, "")
{
	SecondaryNiceHashID_=secondaryNiceHashID;

	AlgorithmName_=AlgorithmNiceHashNames::GetName(DualNiceHashID()); // needed to add secondary
	SecondaryAlgorithmName=AlgorithmNiceHashNames::GetName(secondaryNiceHashID);
	AlgorithmStringID_=MinerBaseTypeName+"_"+AlgorithmName();

	SecondaryBenchmarkSpeed(0.0d);

	IntensitySpeeds.clear();
	SecondaryIntensitySpeeds.clear();
	IntensityPowers.clear();
}

QString DualAlgorithm::CurPayingRate()
{
	QString rate=International::GetText("BenchmarkRatioRateN_A");
	double payingRate=0.0d;
	double paying;
	if (BenchmarkSpeed()>0 && NHSmaData::TryGetPaying(NiceHashID, paying)) {
		payingRate+=BenchmarkSpeed()*paying*Mult;
		rate=QString::number(payingRate, 'f', 8);
		}
	double secPaying;
	if (SecondaryBenchmarkSpeed() && NHSmaData::TryGetPaying(SecondaryNiceHashID(), secPaying)) {
		payingRate+=SecondaryBenchmarkSpeed()*secPaying*Mult;
		rate=QString::number(payingRate, 'f', 8);
		}
	return rate;
}

QString DualAlgorithm::SecondaryCurPayingRatio()
{
	double paying;
	if (NHSmaData::TryGetPaying(SecondaryNiceHashID(), paying)) {
		return QString::number(paying, 'f', 8);
		}
	return International::GetText("BenchmarkRatioRateN_A");
}

QString DualAlgorithm::SecondaryBenchmarkSpeedString()
{
	const QString dcriStatus=" (dcri:%1)";
	if (Enabled && IsBenchmarkPending() && TuningEnabled && !BenchmarkStatus.isEmpty()) {
		return CurrentIntensity>=0
			? dcriStatus.arg(CurrentIntensity)
			: BenchmarkSpeedString();	
		}

	if (SecondaryBenchmarkSpeed()>0) {
		return Helpers::FormatDualSpeedOutput(SecondaryBenchmarkSpeed())+(TuningEnabled? dcriStatus.arg(MostProfitableIntensity()) : "");
		}
	return International::GetText("BenchmarkSpeedStringNone");
}

void DualAlgorithm::UpdateCurProfit(QMap<Enums::AlgorithmType, double> profits)
{
	Algorithm::UpdateCurProfit(profits);
	double secPaying=profits.value(SecondaryNiceHashID());

	SecondaryCurNhmSmaDataVal=secPaying;

	IntensityUpToDate=false;

	CurrentProfit_=(CurNhmSmaDataVal()*BenchmarkSpeed()+SecondaryCurNhmSmaDataVal*SecondaryBenchmarkSpeed())*Mult;

	SubtractPowerFromProfit();
}

void DualAlgorithm::SetIntensitySpeedsForCurrent(double speed, double secondarySpeed)
{
	IntensitySpeeds.insert(CurrentIntensity, speed);
	SecondaryIntensitySpeeds.insert(CurrentIntensity, secondarySpeed);
	Helpers::ConsolePrint("CDTUNING", QString("Speeds set for intensity %1: %2 / %3").arg(CurrentIntensity).arg(speed).arg(secondarySpeed));
	IntensityUpToDate=false;
}

void DualAlgorithm::SetPowerForCurrent(double power)
{
	IntensityPowers.insert(CurrentIntensity, power);
	IntensityUpToDate=false;
}

void DualAlgorithm::UpdateProfitableIntensity()
{
	if (!NHSmaData::HasData()) {
		_mostProfitableIntensity=-1;
		IntensityUpToDate=true;
		return;
		}

	double maxProfit=0.0d;
	int intensity=-1;
	// Max sure to use single | here so second expression evaluates
	double paying, secPaying;
	if (NHSmaData::TryGetPaying(NiceHashID, paying) | NHSmaData::TryGetPaying(SecondaryNiceHashID(), secPaying)) {
		foreach (int key, IntensitySpeeds.keys()) {
			double profit=IntensitySpeeds.value(key)*paying;
			if (SecondaryIntensitySpeeds.contains(key)) {
				profit+=SecondaryIntensitySpeeds.value(key)*secPaying;
				}
			if (profit>maxProfit) {
				maxProfit=profit;
				intensity=key;
				}
			}
		}

	_mostProfitableIntensity=intensity;
	IntensityUpToDate=true;
}

bool DualAlgorithm::IsIntensityEmpty(int i)
{
	if (!IntensitySpeeds.contains(i) || !SecondaryIntensitySpeeds.contains(i)) {
		return true;
		}
	return IntensitySpeeds.value(i, -1)<=0 || SecondaryIntensitySpeeds.value(i, -1)<=0;
}

bool DualAlgorithm::IncrementToNextEmptyIntensity()
{
	// Return false if no more needed increment
	if (!TuningEnabled) {
		return false;
		}
	foreach (int i, SelectedIntensities()) {
		if (IsIntensityEmpty(i)) {
			CurrentIntensity=i;
			}
		}
	return CurrentIntensity>0;
}

bool DualAlgorithm::StartTuning()
{
	// Return false if no benchmark needed
	CurrentIntensity=TuningStart;
	return IncrementToNextEmptyIntensity();
}

void DualAlgorithm::ClearBenchmarkPendingFirst()
{
	Algorithm::ClearBenchmarkPendingFirst();
	CurrentIntensity=-1;
}

double DualAlgorithm::ProfitForIntensity(int intensity)
{
	double profit=0.0d, paying, secPaying;
	if (NHSmaData::TryGetPaying(NiceHashID, paying) && IntensitySpeeds.contains(intensity)) {
		profit+=IntensitySpeeds.value(intensity)*paying*Mult;
		}
	if (NHSmaData::TryGetPaying(SecondaryNiceHashID(), secPaying) && SecondaryIntensitySpeeds.contains(intensity)) {
		profit+=SecondaryIntensitySpeeds.value(intensity)*secPaying*Mult;
		}
	return profit;
}

double DualAlgorithm::SpeedForIntensity(int intensity)
{
	return IntensitySpeeds.value(intensity);
}

double DualAlgorithm::SecondarySpeedForIntensity(int intensity)
{
	return SecondaryIntensitySpeeds.value(intensity);
}

QString DualAlgorithm::SpeedStringForIntensity(int intensity)
{
	double speed=SpeedForIntensity(intensity);
	if (speed>0) {
		return Helpers::FormatSpeedOutput(speed)+"H/s";
		}
	return International::GetText("BenchmarkSpeedStringNone");
}

QString DualAlgorithm::SecondarySpeedStringForIntensity(int intensity)
{
	double speed=SecondarySpeedForIntensity(intensity);
	if (speed>0) {
		return Helpers::FormatSpeedOutput(speed)+"H/s";
		}
	return International::GetText("BenchmarkSpeedStringNone");
}

void DualAlgorithm::MakeIntensityBackup()
{
	_intensitySpeedsBack=IntensitySpeeds;
	_secondaryIntensitySpeedsBack=SecondaryIntensitySpeeds;
	_tuningEnabledBack=TuningEnabled;
	_tuningStartBack=TuningStart;
	_tuningEndBack=TuningEnd;
	_tuningIntervalBack=TuningInterval;
	_intensityPowersBack=IntensityPowers;
	_useIntensityPowersBack=UseIntensityPowers;
}

void DualAlgorithm::RestoreIntensityBackup()
{
	IntensitySpeeds=_intensitySpeedsBack;
	SecondaryIntensitySpeeds=_secondaryIntensitySpeedsBack;
	TuningEnabled=_tuningEnabledBack;
	TuningStart=_tuningStartBack;
	TuningEnd=_tuningEndBack;
	TuningInterval=_tuningIntervalBack;
	IntensityPowers=_intensityPowersBack;
	UseIntensityPowers=_useIntensityPowersBack;
}
