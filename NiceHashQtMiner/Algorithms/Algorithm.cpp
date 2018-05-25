#include "Algorithms/Algorithm.h"
#include "Utils/AlgorithmNiceHashNames.h"
#include "Globals.h"
#include "Switching/NiceHashSma.h"
#include "Utils/Helpers.h"
#include "International.h"
#include "Switching/NHSmaData.h"
#include "Stats/ExchangeRateAPI.h"
#include <QMetaEnum>


QString Algorithm::AlgorithmName() const
{
	return AlgorithmName_;
}

Algorithm::Algorithm(Enums::MinerBaseType minerBaseType, Enums::AlgorithmType niceHashID, QString minerName)
{
	NiceHashID=niceHashID;

	AlgorithmName_=AlgorithmNiceHashNames::GetName(NiceHashID);
	MinerBaseTypeName=QMetaEnum::fromType<Enums::MinerBaseType>().valueToKey((int)minerBaseType);
	AlgorithmStringID_=MinerBaseTypeName+"_"+AlgorithmName_;

	MinerBaseType=minerBaseType;
	MinerName=minerName;

	ExtraLaunchParameters.clear();
	LessThreads=0;
	Enabled= !(
			NiceHashID==Enums::AlgorithmType::Nist5
			|| (NiceHashID==Enums::AlgorithmType::NeoScrypt && minerBaseType==Enums::MinerBaseType::sgminer)
			);
	BenchmarkStatus="";
}

bool Algorithm::BenchmarkNeeded()
{
	return BenchmarkSpeed()<=0;
}

QString Algorithm::CurPayingRatio()
{
	QString ratio=International::GetText("BenchmarkRatioRateN_A");
	double paying=0;
	if (NHSmaData::TryGetPaying(NiceHashID, paying)) {
		ratio=QString::number(paying, 'f', 8);
		}
	return ratio;
}

QString Algorithm::CurPayingRate()
{
	QString rate=International::GetText("BenchmarkRatioRateN_A");
	double payingRate=0,
		paying;
	if (BenchmarkSpeed()>0 && NHSmaData::TryGetPaying(NiceHashID, paying)) {
		payingRate=BenchmarkSpeed()*paying*0.000000001;
		rate=QString::number(payingRate, 'f', 8);
		}
	return rate;
}

void Algorithm::SetBenchmarkPending()
{
	IsBenchmarkPending_=true;
	BenchmarkStatus=International::GetText("Algorithm_Waiting_Benchmark");
}

void Algorithm::SetBenchmarkPendingNoMsg()
{
	IsBenchmarkPending_=true;
}

bool Algorithm::IsPendingString()
{
	return BenchmarkStatus==International::GetText("Algorithm_Waiting_Benchmark")
		|| BenchmarkStatus=="."
		|| BenchmarkStatus==".."
		|| BenchmarkStatus=="...";
}

void Algorithm::ClearBenchmarkPending()
{
	IsBenchmarkPending_=false;
	if (IsPendingString()) {
		BenchmarkStatus="";
		}
}

void Algorithm::ClearBenchmarkPendingFirst()
{
	IsBenchmarkPending_=false;
	BenchmarkStatus="";
}

QString Algorithm::BenchmarkSpeedString()
{
	if (Enabled && IsBenchmarkPending_ && !BenchmarkStatus.isEmpty()) {
		return BenchmarkStatus;
		}
	if (BenchmarkSpeed()>0) {
		return Helpers::FormatDualSpeedOutput(BenchmarkSpeed(), 0, NiceHashID);
		}
	if (!IsPendingString() && !BenchmarkStatus.isEmpty()) {
		return BenchmarkStatus;
		}
	return International::GetText("BenchmarkSpeedStringNone");
}

void Algorithm::UpdateCurProfit(QMap<Enums::AlgorithmType, double> profits)
{
	CurNhmSmaDataVal_=profits.value(NiceHashID);
	CurrentProfit_=CurNhmSmaDataVal_*AvaragedSpeed*Mult;
	SubtractPowerFromProfit();
}

void Algorithm::SubtractPowerFromProfit()
{
	// This is power usage in BTC/hr
	double power=PowerUsage()/1000*ExchangeRateApi::GetKwhPriceInBtc();
	// Now it is power usage in BTC/day
	power*=24;
	// Now we subtract from profit, which may make profit negative
	CurrentProfit_-=power;
}
