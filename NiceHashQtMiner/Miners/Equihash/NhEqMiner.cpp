#include "Miners/Equihash/NhEqMiner.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "PInvoke/CPUID.h"
#include "PInvoke/NiceHashProcess.h"
#include "Miners/Parsing/ExtraLaunchParametersParser.h"
#include "Algorithms/Algorithm.h"

using namespace NiceHashQtMiner;


NhEqMiner::NhEqMiner()
	: NhEqBase("NhEqMiner")
{
	_ConectionType=Enums::NhmConectionType::NONE;
}

NiceHashProcess* NhEqMiner::_Start()
{
	NiceHashProcess* P=NhEqBase::_Start();
	if (CpuSetup->IsInit && P!=nullptr) {
		ulong affinityMask=CpuSetup->MiningPairs->at(0)->Device->AffinityMask();
		if (affinityMask) {
			CPUID::AdjustAffinity(P->Id, affinityMask);
			}
		}

	return P;
}

void NhEqMiner::Start(QString url, QString btcAdress, QString worker)
{
	QString username=GetUsername(btcAdress, worker);
	LastCommandLine=GetDevicesCommandString() << "-a" << QString::number(ApiPort()) << "-1" << url << "-u" << username;
	ProcessHandle=_Start();
}

QStringList NhEqMiner::GetDevicesCommandString()
{
	QStringList deviceStringCommand;

	if (CpuSetup->IsInit) {
		deviceStringCommand << ExtraLaunchParametersParser::ParseForMiningSetup(CpuSetup, Enums::DeviceType::CPU);
		}
	else {
		// disable CPU
		deviceStringCommand << "-t" << "0";
		}

#if WITH_NVIDIA
	if (NvidiaSetup->IsInit) {
		deviceStringCommand << "-cd";
		foreach (MiningPair* nvidiaPair, *NvidiaSetup->MiningPairs) {
			deviceStringCommand << QString::number(nvidiaPair->Device->ID);
			}
		deviceStringCommand << ExtraLaunchParametersParser::ParseForMiningSetup(NvidiaSetup, Enums::DeviceType::NVIDIA);
		}
#endif
#if WITH_AMD
	if (AmdSetup->IsInit) {
		deviceStringCommand << "-op" << QString::number(AmdOclPlatform);
		deviceStringCommand << "-od";
		foreach (MiningPair* amdPair, *AmdSetup->MiningPairs) {
			deviceStringCommand << QString::number(amdPair->Device->ID);
			}
		deviceStringCommand << ExtraLaunchParametersParser::ParseForMiningSetup(AmdSetup, Enums::DeviceType::AMD);
		}
#endif

	return deviceStringCommand;
}

QProcess* NhEqMiner::BenchmarkStartProcess(QStringList commandLine)
{
	QProcess* benchmarkHandle=NhEqBase::BenchmarkStartProcess(commandLine);

	if (CpuSetup->IsInit && benchmarkHandle!=nullptr) {
		ulong affinityMask=(*CpuSetup->MiningPairs)[0]->Device->AffinityMask();
		if (affinityMask) {
			CPUID::AdjustAffinity(benchmarkHandle->processId(), affinityMask);
			}
		}

	return benchmarkHandle;
}

bool NhEqMiner::BenchmarkParseLine(QString outdata)
{
	if (outdata.contains(IterPerSec)) {
		CurSpeed=GetNumber(outdata, "Speed: ", IterPerSec)*SolMultFactor;
		}
	if (outdata.contains(SolsPerSec)) {
		double sols=GetNumber(outdata, "Speed: ", SolsPerSec);
		if (sols>0) {
			BenchmarkAlgorithm->BenchmarkSpeed(CurSpeed);
			return true;
			}
		}
	return false;
}
