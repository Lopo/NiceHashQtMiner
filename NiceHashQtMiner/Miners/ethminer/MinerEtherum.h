#ifndef MINERS_ETHMINER_MINERETHERUM_H_
#define MINERS_ETHMINER_MINERETHERUM_H_

#include "Miners/Miner.h"
class QUdpSocket;

class MinerEtherum
	: public Miner
{
//	Q_OBJECT
public:
	static QString GetDagGenerationString(Enums::DagGenerationType type);
	void Start(QString url, QString btcAdress, QString worker, QList<MinerEtherum*> usedMiners);
	void InitMiningSetup(MiningSetup* miningSetup) override;
	ApiData* GetSummaryAsync() override;

protected:
	MinerEtherum(QString minerDeviceName, QString blockString);

	virtual QStringList GetStartCommandStringPart(QString url, QString username)=0;
	virtual QStringList GetBenchmarkCommandStringPart(Algorithm* algorithm)=0;
	QStringList GetDevicesCommandString() override;
	QStringList BenchmarkCreateCommandLine(Algorithm* algorithm, int time) override;
	NiceHashProcess* _Start() override;
	void _Stop(Enums::MinerStopType willswitch) override;
	bool BenchmarkParseLine(QString outdata) override;
	void BenchmarkOutputErrorDataReceivedImpl(QString outdata) override;

	ComputeDevice* DaggerHashimotoGenerateDevice=nullptr;
	QString CurrentBlockString;
	bool IsPaused=false;

private:
	void SetEthminerAPI(int port);
	void StartMining();
	void StopMining();

	Enums::DagGenerationType _dagGenerationType;
	enum class GetSpeedStatus {
		NONE,
		GOT,
		EXCEPTION
		};
	GetSpeedStatus GetSpeed(bool* ismining, double* speed);

	int _mPort;
	QUdpSocket* _mClient=nullptr;
	void SendUdp(int code);
};

#endif
