#ifndef MINERS_EQUIHASH_NHEQBASE_H_
#define MINERS_EQUIHASH_NHEQBASE_H_

#include "Miners/Miner.h"
#include "config.h"


class NhEqBase
	: public Miner
{
//	Q_OBJECT
public:
	void InitMiningSetup(MiningSetup* miningSetup) override;
	ApiData* GetSummaryAsync() override;

protected:
	MiningSetup* CpuSetup=new MiningSetup(nullptr);
#if WITH_NVIDIA
	MiningSetup* NvidiaSetup=new MiningSetup(nullptr);
#endif
#if WITH_AMD
	int AmdOclPlatform;
	MiningSetup* AmdSetup=new MiningSetup(nullptr);
#endif

	// extra benchmark stuff
	double CurSpeed=0;

	static QString IterPerSec;
	static QString SolsPerSec;
	const double SolMultFactor=1.9;

	NhEqBase(QString minerDeviceName);

	QStringList BenchmarkCreateCommandLine(Algorithm* algorithm, int time) override;

	void _Stop(Enums::MinerStopType willswitch) override;
	double GetNumber(QString outdata, QString startF, QString remF);
	void BenchmarkOutputErrorDataReceivedImpl(QString outdata) override;

private:
	class Result {
	public:
		double interval_seconds;
		double speed_ips;
		double speed_sps;
		double accepted_per_minute;
		double rejected_per_minute;

		void loadJsonObject(QJsonObject o);
		};

	class JsonApiResponse {
	public:
		QString method;
		Result result;
		QJsonObject* error=nullptr;

		static JsonApiResponse* fromJson(QByteArray json);
		};
};

#endif
