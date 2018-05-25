#include "Miners/ethminer/MinerEtherum.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Utils/Helpers.h"
#include "Miners/ethminer/Ethereum.h"
#include "Algorithms/Algorithm.h"
#include "PInvoke/NiceHashProcess.h"
#include <QUdpSocket>
#include <QDateTime>
#include <QNetworkDatagram>
#include <float.h>


MinerEtherum::MinerEtherum(QString minerDeviceName, QString blockString)
	: Miner(minerDeviceName, 90*1000)
{
	CurrentBlockString=blockString;
	_dagGenerationType=ConfigManager.generalConfig->EthminerDagGenerationType;
}

QStringList MinerEtherum::GetDevicesCommandString()
{
	QStringList deviceStringCommand;
	foreach (MiningPair* mPair, *MiningSetup_->MiningPairs) {
		deviceStringCommand << QString::number(mPair->Device->ID);
		}
	// set dag load mode
	deviceStringCommand << "--dag-load-mode" << GetDagGenerationString(_dagGenerationType);
	if (_dagGenerationType==Enums::DagGenerationType::Single || _dagGenerationType==Enums::DagGenerationType::SingleKeep) {
		// set dag generation device
		deviceStringCommand << QString::number(DaggerHashimotoGenerateDevice->ID);
		}
	return deviceStringCommand;
}

QString MinerEtherum::GetDagGenerationString(Enums::DagGenerationType type)
{
	switch (type) {
		case Enums::DagGenerationType::Parallel:
			return "parallel";
		case Enums::DagGenerationType::Sequential:
			return "sequential";
		case Enums::DagGenerationType::Single:
			return "single";
		case Enums::DagGenerationType::SingleKeep:
		default:
			return "singlekeep";
		}
	return "singlekeep";
}

void MinerEtherum::Start(QString url, QString btcAdress, QString worker, QList<MinerEtherum*> usedMiners)
{
	if (!IsInit()) {
		Helpers::ConsolePrint(MinerTag(), "MiningSetup is not initialized exiting Start()");
		return;
		}
	foreach (MinerEtherum* ethminer, usedMiners) {
		if (ethminer->MinerID!=MinerID && (ethminer->IsRunning() || ethminer->IsPaused)) {
			Helpers::ConsolePrint(MinerTag(), QString("Will end %1 %2").arg(ethminer->MinerTag()).arg(ethminer->ProcessTag()));
			ethminer->End();
			QThread::msleep(ConfigManager.generalConfig->MinerRestartDelayMS);
			}
		}

	IsPaused=false;
	if (ProcessHandle==nullptr) {
		QString username=GetUsername(btcAdress, worker);
		LastCommandLine=GetStartCommandStringPart(url, username) << GetDevicesCommandString();
		ProcessHandle=_Start();
		}
	else {
		Helpers::ConsolePrint(MinerTag(), ProcessTag()+" Resuming ethminer..");
		StartCoolDownTimerChecker();
		StartMining();
		}
}

QStringList MinerEtherum::BenchmarkCreateCommandLine(Algorithm* algorithm, int time)
{
	QStringList commandLine=GetBenchmarkCommandStringPart(algorithm) << GetDevicesCommandString();
	Ethereum::GetCurrentBlock(CurrentBlockString);
	commandLine << "--benchmark" << Ethereum::CurrentBlockNum;

	return commandLine;
}

void MinerEtherum::InitMiningSetup(MiningSetup* miningSetup)
{
	Miner::InitMiningSetup(miningSetup);
	// now find the fastest for DAG generation
	double fastestSpeed=-DBL_MAX;
	foreach (MiningPair* mPair, *MiningSetup_->MiningPairs) {
		double compareSpeed=mPair->algorithm->AvaragedSpeed;
		if (fastestSpeed<compareSpeed) {
			DaggerHashimotoGenerateDevice=mPair->Device;
			fastestSpeed=compareSpeed;
			}
		}
}

ApiData* MinerEtherum::GetSummaryAsync()
{
	ApiData* ad=new ApiData(MiningSetup_->CurrentAlgorithmType);

	bool ismining;
	GetSpeedStatus getSpeedStatus=GetSpeed(&ismining, &ad->Speed);
	if (getSpeedStatus==GetSpeedStatus::GOT) {
		// fix MH/s
		ad->Speed*=1000*1000;
		CurrentMinerReadStatus=Enums::MinerApiReadStatus::GOT_READ;
		// check if speed zero
		if (!ad->Speed) {
			CurrentMinerReadStatus=Enums::MinerApiReadStatus::READ_SPEED_ZERO;
			}
		return ad;
		}
	if (getSpeedStatus==GetSpeedStatus::NONE) {
		ad->Speed=0;
		CurrentMinerReadStatus=Enums::MinerApiReadStatus::NONE;
		return ad;
		}
	CurrentMinerReadStatus=Enums::MinerApiReadStatus::NONE;
	return nullptr;
}

NiceHashProcess* MinerEtherum::_Start()
{
	SetEthminerAPI(ApiPort());
	return Miner::_Start();
}

void MinerEtherum::_Stop(Enums::MinerStopType willswitch)
{
	// prevent logging non runing miner
	if (IsRunning() && !IsPaused && willswitch==Enums::MinerStopType::SWITCH) {
		// daggerhashimoto - we only "pause" mining
		IsPaused=true;
		Helpers::ConsolePrint(MinerTag(), ProcessTag()+" Pausing ethminer..");
		StopMining();
		return;
		}
	if ((IsRunning() || IsPaused) && willswitch!=Enums::MinerStopType::SWITCH) {
		Helpers::ConsolePrint(MinerTag(), ProcessTag()+" Shutting down miner");
		}
	if ((willswitch==Enums::MinerStopType::FORCE_END || willswitch==Enums::MinerStopType::END) && ProcessHandle!=nullptr) {
		IsPaused=false; // shutting down means it is not paused
		try {
			ProcessHandle->Kill();
			}
		catch (...) { }
//		finally {
		if (ProcessHandle!=nullptr) {delete ProcessHandle;};
			ProcessHandle=nullptr;
//			}
		}
}

bool MinerEtherum::BenchmarkParseLine(QString outdata)
{
	if (!outdata.contains("min/mean/max:")) {
		return false;
		}
	QStringList splt=outdata.split('/', QString::SplitBehavior::SkipEmptyParts);
	int index=splt.indexOf("mean");
	double avg_spd=splt.at(index+2).toDouble();
	Helpers::ConsolePrint("BENCHMARK", QString("Final Speed: %1 H/s").arg(avg_spd));

	BenchmarkAlgorithm->BenchmarkSpeed(avg_spd);
	return true;
}

void MinerEtherum::BenchmarkOutputErrorDataReceivedImpl(QString outdata)
{
	CheckOutdata(outdata);
}

void MinerEtherum::SetEthminerAPI(int port)
{
	_mPort=port;
	_mClient=new QUdpSocket(this);
//	_mClient->connectToHost("127.0.0.1", port);
	_mClient->connectToHost(QHostAddress::LocalHost, port);
}

void MinerEtherum::StartMining()
{
	Helpers::ConsolePrint(MinerTag(), ProcessTag()+" Sending START UDP");
	SendUdp(2);
	_IsRunning=true;
}

void MinerEtherum::StopMining()
{
	Helpers::ConsolePrint(MinerTag(), ProcessTag()+" Sending STOP UDP");
	SendUdp(1);
	_IsRunning=false;
}

MinerEtherum::GetSpeedStatus MinerEtherum::GetSpeed(bool* ismining, double* speed)
{
	*speed=0;
	*ismining=false;

	SendUdp(3);

	QDateTime start=QDateTime::currentDateTime();

	while (start.msecsTo(QDateTime::currentDateTime())<2000) {
		if (_mClient->hasPendingDatagrams()) {
			// read
			try {
				QNetworkDatagram dgram=_mClient->receiveDatagram();
				if (!dgram.isValid()) {
					return GetSpeedStatus::NONE;
					}
				QByteArray data=dgram.data();
				if (data.length()!=8) {
					return GetSpeedStatus::NONE;
					}
				double speed=QVariant(data).toDouble();
				if (speed>=0) {
					*ismining=true;
					}
				else {
					speed=0;
					}
				return GetSpeedStatus::GOT;
				}
			catch (...) {
				Helpers::ConsolePrint(MinerTag(), ProcessTag()+" Could not read data from API bind port");
				return GetSpeedStatus::EXCEPTION;
				}
			}
		QThread::msleep(2);
		}

	return GetSpeedStatus::NONE;
}

void MinerEtherum::SendUdp(int code)
{
	int data[1]={code};
	_mClient->writeDatagram(QByteArray::fromRawData(reinterpret_cast<char*>(data), 1));
}
