#include "Miners/Equihash/NhEqBase.h"
#include "Devices/ComputeDeviceManager.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Utils/Helpers.h"
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>


QString NhEqBase::IterPerSec="I/s";
QString NhEqBase::SolsPerSec="Sols/s";

NhEqBase::NhEqBase(QString minerDevicename)
	: Miner(minerDevicename, 60*1000*5)
{
	AmdOclPlatform=ComputeDeviceManager.Available.AmdOpenCLPlatformNum;
}

void NhEqBase::InitMiningSetup(MiningSetup* miningSetup)
{
	Miner::InitMiningSetup(miningSetup);
	QList<MiningPair*>* cpus=new QList<MiningPair*>;
#if WITH_NVIDIA
	QList<MiningPair*>* nvidias=new QList<MiningPair*>;
#endif
#if WITH_AMD
	QList<MiningPair*>* amds=new QList<MiningPair*>;
#endif
	foreach (MiningPair* pairs, *MiningSetup_->MiningPairs) {
		switch (pairs->Device->DeviceType) {
			case Enums::DeviceType::CPU:
				cpus->append(pairs);
				break;
#if WITH_NVIDIA
			case Enums::DeviceType::NVIDIA:
				nvidias->append(pairs);
				break;
#endif
#if WITH_AMD
			case Enums::DeviceType::AMD:
				amds->append(pairs);
				break;
#endif
			}
		}
	// reinit
	CpuSetup=new MiningSetup(cpus);
#if WITH_NVIDIA
	NvidiaSetup=new MiningSetup(nvidias);
#endif
#if WITH_AMD
	AmdSetup=new MiningSetup(amds);
#endif
}

QStringList NhEqBase::BenchmarkCreateCommandLine(Algorithm* algorithm, int time)
{
	// TODO nvidia extras
	return QStringList() << "-b" << GetDevicesCommandString();
}

ApiData* NhEqBase::GetSummaryAsync()
{
	CurrentMinerReadStatus=Enums::MinerApiReadStatus::NONE;
	ApiData* ad=new ApiData(MiningSetup_->CurrentAlgorithmType);

	JsonApiResponse* resp=nullptr;
	try {
		QTcpSocket client(this);
		client.connectToHost("127.0.0.1", ApiPort());
		client.waitForConnected();
		client.write("status\n");
		client.flush();
		client.waitForReadyRead();
		resp=JsonApiResponse::fromJson(client.readAll());
		client.close();
		}
	catch (QException& ex) {
		Helpers::ConsolePrint("ERROR", ex.what());
		}

	if (resp!=nullptr && resp->error==nullptr) {
		ad->Speed=resp->result.speed_sps;
		CurrentMinerReadStatus=Enums::MinerApiReadStatus::GOT_READ;
		if (!ad->Speed) {
			CurrentMinerReadStatus=Enums::MinerApiReadStatus::READ_SPEED_ZERO;
			}
		}

	return ad;
}

void NhEqBase::_Stop(Enums::MinerStopType willswitch)
{
	Stop_cpu_ccminer_sgminer_nheqminer(willswitch);
}

double NhEqBase::GetNumber(QString outdata, QString startF, QString remF)
{
	try {
		int speedStart=outdata.indexOf(startF);
		QString speed=outdata.mid(speedStart, outdata.length()-speedStart).remove(startF).remove(remF).trimmed();
		return speed.toDouble();
		}
	catch (...) {}
	return 0;
}

void NhEqBase::BenchmarkOutputErrorDataReceivedImpl(QString outdata)
{
	CheckOutdata(outdata);
}

NhEqBase::JsonApiResponse* NhEqBase::JsonApiResponse::fromJson(QByteArray json)
{
	QJsonDocument d=QJsonDocument::fromJson(json);
	if (d.isEmpty()) {
		return nullptr;
		}
	QJsonObject o=d.object();
	JsonApiResponse* ret=new JsonApiResponse;
	ret->method=o.value("method").toString();
	ret->result.loadJsonObject(o.value("result").toObject());
	if (o.contains("error")) {
		o.value("error").toObject();
		}
	return ret;
}

void NhEqBase::Result::loadJsonObject(QJsonObject o)
{
	interval_seconds=o.value("interval_seconds").toDouble();
	speed_ips=o.value("speed_ips").toDouble();
	speed_sps=o.value("speed_sps").toDouble();
	accepted_per_minute=o.value("accepted_per_minute").toDouble();
	rejected_per_minute=o.value("rejected_per_minute").toDouble();
}
