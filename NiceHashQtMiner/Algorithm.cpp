#include "Algorithm.h"
#include "Utils/AlgorithmNiceHashNames.h"
#include "Globals.h"
#include "Switching/NiceHashSma.h"
#include "Utils/Helpers.h"
#include "International.h"
#include "Switching/NHSmaData.h"
#include <QMetaEnum>


Algorithm::Algorithm(Enums::MinerBaseType minerBaseType, Enums::AlgorithmType niceHashID, QString minerName, Enums::AlgorithmType secondaryNiceHashID/*=Enums::AlgorithmType::NONE*/)
{
	NiceHashID=niceHashID;
	SecondaryNiceHashID=secondaryNiceHashID;

	AlgorithmName=AlgorithmNiceHashNames::GetName(DualNiceHashID());
	MinerBaseTypeName=QMetaEnum::fromType<Enums::MinerBaseType>().valueToKey((int)minerBaseType);
	AlgorithmStringID=MinerBaseTypeName+"_"+AlgorithmName;

	MinerBaseType=minerBaseType;
	MinerName=minerName;

	BenchmarkSpeed=0.0d;
	SecondaryBenchmarkSpeed=0.0d;
	ExtraLaunchParameters.clear();
	LessThreads=0;
	Enabled= !(
			NiceHashID==Enums::AlgorithmType::Nist5
			|| (NiceHashID==Enums::AlgorithmType::NeoScrypt && minerBaseType==Enums::MinerBaseType::sgminer)
			);
	BenchmarkStatus="";
}

QString Algorithm::CurPayingRatio()
{
	QString ratio=International::GetText("BenchmarkRatioRateN_A");
	double paying;
	if (NHSmaData::TryGetPaying(NiceHashID, paying)) {
		ratio=QString::number(paying, 'f', 8);
		double secondaryPaying;
		if (IsDual() && NHSmaData::TryGetPaying(SecondaryNiceHashID, secondaryPaying)) {
			ratio+="/"+QString::number(secondaryPaying, 'f', 8);
			}
		}
	return ratio;
}

QString Algorithm::CurPayingRate()
{
	QString rate=International::GetText("BenchmarkRatioRateN_A");
	double payingRate=0.0d,
		paying;
	if (BenchmarkSpeed>0 && NHSmaData::TryGetPaying(NiceHashID, paying)) {
		payingRate+=BenchmarkSpeed*paying*0.000000001;
		}
	double secondaryPaying;
	if (SecondaryBenchmarkSpeed>0 && IsDual() && NHSmaData::TryGetPaying(NiceHashID, secondaryPaying)) {
		payingRate+=SecondaryBenchmarkSpeed*secondaryPaying*0.000000001;
		}
	rate=QString::number(payingRate, 'f', 8);
	return rate;
}

void Algorithm::SetBenchmarkPending()
{
	_IsBenchmarkPending=true;
	BenchmarkStatus=International::GetText("Algorithm_Waiting_Benchmark");
}

void Algorithm::SetBenchmarkPendingNoMsg()
{
	_IsBenchmarkPending=true;
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
	_IsBenchmarkPending=false;
	if (IsPendingString()) {
		BenchmarkStatus="";
		}
}

void Algorithm::ClearBenchmarkPendingFirst()
{
	_IsBenchmarkPending=false;
	BenchmarkStatus="";
}

QString Algorithm::BenchmarkSpeedString()
{
	if (Enabled && IsBenchmarkPending() && !BenchmarkStatus.isEmpty()) {
		return BenchmarkStatus;
		}
	if (BenchmarkSpeed>0) {
		return Helpers::FormatDualSpeedOutput(NiceHashID, BenchmarkSpeed, SecondaryBenchmarkSpeed);
		}
	if (!IsPendingString() && !BenchmarkStatus.isEmpty()) {
		return BenchmarkStatus;
		}
	return International::GetText("BenchmarkSpeedStringNone");
}

// return hybrid type if dual, else standard ID
Enums::AlgorithmType Algorithm::DualNiceHashID()
{
	if (NiceHashID==Enums::AlgorithmType::DaggerHashimoto) {
		switch (SecondaryNiceHashID) {
			case Enums::AlgorithmType::Decred:
				return Enums::AlgorithmType::DaggerDecred;
			case Enums::AlgorithmType::Lbry:
				return Enums::AlgorithmType::DaggerLbry;
			case Enums::AlgorithmType::Pascal:
				return Enums::AlgorithmType::DaggerPascal;
			case Enums::AlgorithmType::Sia:
				return Enums::AlgorithmType::DaggerSia;
			case Enums::AlgorithmType::Keccak:
				return Enums::AlgorithmType::DaggerKeccak;
			case Enums::AlgorithmType::Blake2s:
				return Enums::AlgorithmType::DaggerBlake2s;
			default: // prevent warning: enumeration value not handled in switch [-Wswitch]
				return NiceHashID;
			}
		}
	return NiceHashID;
}

bool Algorithm::IsDual()
{
	return (Enums::AlgorithmType::DaggerBlake2s<=DualNiceHashID() && DualNiceHashID()<=Enums::AlgorithmType::DaggerPascal);
}
