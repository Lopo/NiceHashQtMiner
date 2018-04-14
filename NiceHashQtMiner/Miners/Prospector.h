#ifndef MINERS_PROSPECTOR_H_
#define MINERS_PROSPECTOR_H_

#include "Miners/Miner.h"
#include <QtSql/QSqlDatabase>

class ProspectorPlatforms
{
public:
	inline static bool IsInit() {return NVPlatform>=0 || AmdPlatform>=0;};
	static int NVPlatform/*=-1*/;
	static int AmdPlatform/*=-1*/;

	static int PlatformForDeviceType(Enums::DeviceType type);
};


class Prospector
	: public Miner
{
public:
	Prospector();
	ApiData* GetSummaryAsync() override;
	void Start(QString url, QString btcAdress, QString worker) override;

protected:
	void CleanAllOldLogs();
	void _Stop(Enums::MinerStopType willswitch) override;

	QStringList BenchmarkCreateCommandLine(Algorithm* algorithm, int time) override;
	void BenchmarkThreadRoutine(QStringList commandLine) override;
	void BenchmarkOutputErrorDataReceivedImpl(QString outdata) override;
	bool BenchmarkParseLine(QString outdata) override;

private:
	class hashrates {
	public:
		int id;

		int session_id;
		QString coin;
		QString device;
		int time;
		double rate;
		};
	class sessions {
	public:
		int id;

		QString start;
		};
	class HashrateApiResponse {
	public:
		QString coin;
		QString device;
		double rate;
		QString time;
		};
	class ProspectorDatabase {
	public:
		ProspectorDatabase(QString path);
		double QueryLastSpeed(QString device);
		QVector<hashrates> QuerySpeedsForSession(int id);
		sessions LastSession();
	private:
		QSqlDatabase _db;
		};

	ProspectorDatabase* _database=nullptr;

	int _benchmarkTimeWait=0;
	const double DevFee=0.01; // 1% devfee

	const QString PlatformStart="platform ";
	const QString PlatformEnd=" - ";

	const int apiPort=42000;

	QString DeviceIDString(int id, Enums::DeviceType type);
	QString GetConfigFileName();
	void PrepareConfigFile(QString pool, QString wallet);
	bool InitPlatforms();

	QStringList GetStartupCommand(QString url, QString btcAddress, QString worker);
};

#endif
