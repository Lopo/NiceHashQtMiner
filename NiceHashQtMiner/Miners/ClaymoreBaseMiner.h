#ifndef MINERS_CLAYMOREBASEMINER_H_
#define MINERS_CLAYMOREBASEMINER_H_

#include "Miners/Miner.h"


class ClaymoreBaseMiner
	: public Miner
{
//	Q_OBJECT
public:
	bool IsDual();

	ApiData* GetSummaryAsync() override;

protected:
	int BenchmarkTimeWait=2*45; // Ok... this was all wrong
	QString LookForStart;

	// only dagger change
	bool IgnoreZero=false;

	double ApiReadMult=1;
	Enums::AlgorithmType SecondaryAlgorithmType=Enums::AlgorithmType::NONE;

	ClaymoreBaseMiner(QString minerDeviceName, QString lookForStart, int maxCDTime);

	virtual double DevFee()=0;
	virtual QString SecondaryLookForStart();

	void _Stop(Enums::MinerStopType willswitch) override;
	virtual QStringList DeviceCommand(int amdCount=1);
	QStringList GetDevicesCommandString() override;
	void BenchmarkThreadRoutine(QStringList commandLine) override;
	void ProcessBenchLinesAlternate(QStringList lines) override;
	void BenchmarkOutputErrorDataReceivedImpl(QString outdata) override;
	bool BenchmarkParseLine(QString outdata) override;

	double GetNumber(QString outdata);
	double GetNumber(QString outdata, QString LOOK_FOR_START, QString LOOK_FOR_END);

private:
	int _benchmarkReadCount=0;
	double _benchmarkSum=0.0d;
	int _secondaryBenchmarkReadCount=0;
	double _secondaryBenchmarkSum=0.0d;
	const QString LookForEnd="h/s";

	class JsonApiResponse {
	public:
		QStringList* result;
		int id;
		QJsonObject* error=nullptr;

		static JsonApiResponse* fromJson(QByteArray json);
		};
};

#endif
