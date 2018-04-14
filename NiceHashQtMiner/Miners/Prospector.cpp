#include "Miners/Prospector.h"
#include "Utils/Helpers.h"
#include "Devices/ComputeDeviceManager.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Globals.h"
#include "Algorithm.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Qt/LException.h"
#include <QtSql>
#include <QtWebSockets/QWebSocket>
#include <QtNetwork/QNetworkReply>

extern QNetworkAccessManager* qnam;


int ProspectorPlatforms::NVPlatform=-1;
int ProspectorPlatforms::AmdPlatform=-1;


int ProspectorPlatforms::PlatformForDeviceType(Enums::DeviceType type)
{
	if (IsInit()) {
		if (type==Enums::DeviceType::NVIDIA) {
			return NVPlatform;
			}
		if (type==Enums::DeviceType::AMD) {
			return AmdPlatform;
			}
		}
	return -1;
}

Prospector::ProspectorDatabase::ProspectorDatabase(QString path)
{
	_db=QSqlDatabase::addDatabase("QSQLITE");
	_db.setDatabaseName(path);
	if (!_db.open()) {
		Helpers::ConsolePrint("PROSPECTORSQL", QString("Unable to open: %1").arg(_db.lastError().text()));
		}
}

double Prospector::ProspectorDatabase::QueryLastSpeed(QString device)
{
	try {
		QSqlTableModel m(nullptr, _db);
		m.setTable("hashrates");
		m.select();
		m.setFilter(QString("device='%1'").arg(device));
		m.setSort(m.fieldIndex("time"), Qt::SortOrder::DescendingOrder);
		m.select();
		return m.record(0).field("rate").value().toDouble();
		}
	catch (QException e) {
		Helpers::ConsolePrint("PROSPECTORSQL", e.what());
		return 0;
		}
}

QVector<Prospector::hashrates> Prospector::ProspectorDatabase::QuerySpeedsForSession(int id)
{
	try {
		QSqlTableModel m(nullptr, _db);
		m.setTable("hashrates");
		m.setFilter(QString("session_id=%1").arg(id));
		m.select();
		QVector<Prospector::hashrates> ret;
		for (int i=0; i<m.rowCount(); i++) {
			QSqlRecord r=m.record(i);
			Prospector::hashrates h;
			h.id=r.value("id").toInt();
			h.session_id=r.value("session_id").toInt();
			h.coin=r.value("coin").toString();
			h.device=r.value("device").toString();
			h.time=r.value("time").toInt();
			h.rate=r.value("rate").toDouble();
			ret.insert(h.id, h);
			}
		return ret;
		}
	catch (QException e) {
		Helpers::ConsolePrint("PROSPECTORSQL", e.what());
		return QVector<Prospector::hashrates>();
		}
}
Prospector::sessions Prospector::ProspectorDatabase::LastSession()
{
	try {
		QSqlTableModel m(nullptr, _db);
		m.setTable("sessions");
		m.select();
		m.setSort(m.fieldIndex("id"), Qt::SortOrder::DescendingOrder);
		m.select();
		Prospector::sessions ret;
		ret.id=m.record(0).value("id").toInt();
		ret.start=m.record(0).value("start").toString();
		return ret;
		}
	catch (QException e) {
		Helpers::ConsolePrint("PROSPECTORSQL", e.what());
		return Prospector::sessions();
		}
}

Prospector::Prospector()
	: Miner("Prospector", 3600000)
{
	_ConectionType=Enums::NhmConectionType::NONE;
//	IsNeverHideMiningWindow=true;
}

QString Prospector::DeviceIDString(int id, Enums::DeviceType type)
{
	int platform=0;
	if (InitPlatforms()) {
		platform=ProspectorPlatforms::PlatformForDeviceType(type);
		}
	else {
		// fallback
		Helpers::ConsolePrint(MinerTag(), "Failed to get platform, falling back");
		if (ComputeDeviceManager.Avaliable.HasNvidia && type!=Enums::DeviceType::NVIDIA) {
			platform=1;
			}
		}
	return QString("%1-%2").arg(platform).arg(id);
}

QString Prospector::GetConfigFileName()
{
	return QString("config_%1.toml").arg(_MiningSetup->MiningPairs->at(0)->Device->ID);
}

void Prospector::PrepareConfigFile(QString pool, QString wallet)
{
	if (_MiningSetup->MiningPairs->count()<=0) {
		return;
		}
	try {
		QStringList sb;
		sb << "[general]";
		sb << QString("gpu-coin = \"%1\"").arg(_MiningSetup->MinerName);
		sb << QString("default-username = \"%1\"").arg(wallet);
		sb << "default-password = \"x\"";

		sb << QString("[pools.%1]").arg(_MiningSetup->MinerName);
		sb << QString("url = \"stratum+tcp://%1\"").arg(pool);

		foreach (MiningPair* dev, *_MiningSetup->MiningPairs) {
			sb << QString("[gpus.%1]").arg(DeviceIDString(dev->Device->ID, dev->Device->DeviceType));
			sb << "enabled = true";
			sb << QString("label = \"%1\"").arg(dev->Device->Name);
			}

		sb << "[cpu]";
		sb << "enabled = false";

		QFile f(WorkingDirectory()+GetConfigFileName());
		f.open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Truncate);
		f.write(sb.join('\n').toLatin1());
		f.close();
		}
	catch (...) { }
}

bool Prospector::InitPlatforms()
{
	if (ProspectorPlatforms::IsInit()) {
		return true;
		}

	CleanAllOldLogs();
	QProcess* handle=BenchmarkStartProcess({"list-devices"});
	handle->start();

	handle->waitForFinished(20*1000); // 20s
	delete handle;
	handle=nullptr;

	try {
		QString latestLogFile="";
		QDir dirInfo(WorkingDirectory()+"logs/");
		foreach (QString file, dirInfo.entryList(QDir::NoDotAndDotDot | QDir::Files)) {
			latestLogFile=file;
			break;
			}
		if (QFile::exists(dirInfo.path()+"/"+latestLogFile)) {
			QFile f(dirInfo.path()+"/"+latestLogFile);
			f.open(QIODevice::Text | QIODevice::ReadOnly);
			QStringList lines=QString(f.readAll()).split('\n');
			f.close();
			foreach (QString line, lines) {
				if (line.isEmpty()) {
					continue;
					}
				QString lineLowered=line.toLower();
				if (!lineLowered.contains(PlatformStart)) {
					continue;
					}
				int platStart=lineLowered.indexOf(PlatformStart);
				QString plat=lineLowered.mid(platStart, line.length()-platStart).remove(PlatformStart);
				plat=plat.mid(0, plat.indexOf(PlatformEnd));

				bool ok;
				int platIndex=plat.toInt(&ok);
				if (!ok) {
					continue;
					}
				if (lineLowered.contains("nvidia")) {
					Helpers::ConsolePrint(MinerTag(), QString("Setting nvidia platform: %1").arg(platIndex));
					ProspectorPlatforms::NVPlatform=platIndex;
					if (ProspectorPlatforms::AmdPlatform>=0) {
						break;
						}
					}
				else if (lineLowered.contains("amd")) {
					Helpers::ConsolePrint(MinerTag(), QString("Setting amd platform: %1").arg(platIndex));
					ProspectorPlatforms::AmdPlatform=platIndex;
					if (ProspectorPlatforms::NVPlatform>=0) {
						break;
						}
					}
				}
			}
		}
	catch (QException e) {
		Helpers::ConsolePrint(MinerTag(), e.what());
		}

	return ProspectorPlatforms::IsInit();
}

void Prospector::CleanAllOldLogs()
{
	// clean old logs
	try {
		QDir dirInfo(WorkingDirectory()+"logs/");
		if (dirInfo.exists()) {
			foreach (QFileInfo file, dirInfo.entryInfoList()) {
				QFile(file.absoluteFilePath()).remove();
				}
			}
		}
	catch (...) { }
}

void Prospector::_Stop(Enums::MinerStopType willswitch)
{
	Stop_cpu_ccminer_sgminer_nheqminer(willswitch);
}

ApiData* Prospector::GetSummaryAsync()
{
	CurrentMinerReadStatus=Enums::MinerApiReadStatus::NONE;
	ApiData* ad=new ApiData(_MiningSetup->CurrentAlgorithmType, _MiningSetup->CurrentSecondaryAlgorithmType);

	QList<HashrateApiResponse>* resp=nullptr;
	try {
		QNetworkRequest client(QString("http://localhost:%1/api/hashrates").arg(apiPort));
		QNetworkReply* reply;
		QEventLoop cLoop;
		QObject::connect(qnam, SIGNAL(finished(QNetworkReply*)), &cLoop, SLOT(quit()));
		reply=qnam->get(client);
		cLoop.exec();
		reply->deleteLater();
		resp=new QList<HashrateApiResponse>;
		foreach (QJsonValue v, QJsonDocument::fromJson(reply->readAll()).array()) {
			QJsonObject vo=v.toObject();
			HashrateApiResponse r;
			r.coin=vo.value("coin").toString();
			r.device=vo.value("device").toString();
			r.rate=vo.value("rate").toDouble();
			r.time=vo.value("time").toString();
			resp->append(r);
			}
		}
	catch (QException ex) {
		Helpers::ConsolePrint(MinerTag(), QString("GetSummary exception: ")+ex.what());
		}

	if (resp!=nullptr && !resp->count()) {
		ad->Speed=0;
		foreach (HashrateApiResponse response, *resp) {
			if (response.coin==_MiningSetup->MinerName) {
				ad->Speed+=response.rate;
				CurrentMinerReadStatus=Enums::MinerApiReadStatus::GOT_READ;
				}
			}
		if (!ad->Speed) {
			CurrentMinerReadStatus=Enums::MinerApiReadStatus::READ_SPEED_ZERO;
			}
		}

	return ad;
}

void Prospector::Start(QString url, QString btcAdress, QString worker)
{
	if (!IsInit()) {
		Helpers::ConsolePrint(MinerTag(), "MiningSetup is not initialized exiting Start()");
		return;
		}
	LastCommandLine=GetStartupCommand(url, btcAdress, worker);

	ProcessHandle=_Start();
}

QStringList Prospector::GetStartupCommand(QString url, QString btcAddress, QString worker)
{
	PrepareConfigFile(url, GetUsername(btcAddress, worker));
	return QStringList() << "--config" << GetConfigFileName();
}

QStringList Prospector::BenchmarkCreateCommandLine(Algorithm* algorithm, int time)
{
	// Prospector can take a very long time to start up
	_benchmarkTimeWait=time+60;
	// network stub
	QString url=Globals::GetLocationURL(algorithm->NiceHashID, Globals::MiningLocation[ConfigManager.generalConfig->ServiceLocation], _ConectionType);
	return GetStartupCommand(url, Globals::GetBitcoinUser(), ConfigManager.generalConfig->WorkerName.trimmed());
}

void Prospector::BenchmarkThreadRoutine(QStringList commandLine)
{
	QThread::msleep(ConfigManager.generalConfig->MinerRestartDelayMS);

	BenchmarkSignalQuit=false;
	BenchmarkSignalHanged=false;
	BenchmarkSignalFinnished=false;
	BenchmarkException=nullptr;

	QDateTime startTime=QDateTime::currentDateTime();

	try {
		Helpers::ConsolePrint("BENCHMARK", "Benchmark starts");
		Helpers::ConsolePrint(MinerTag(), QString("Benchmark should end in : %1 seconds").arg(_benchmarkTimeWait));
		BenchmarkHandle=BenchmarkStartProcess(commandLine);
		QElapsedTimer benchmarkTimer;
		benchmarkTimer.start();
		_BenchmarkProcessStatus=Enums::BenchmarkProcessStatus::Running;
		bool keepRunning=true;
		while (keepRunning && IsActiveProcess(BenchmarkHandle->processId())) {
			if (benchmarkTimer.elapsed()/1000>=(_benchmarkTimeWait+2)
				|| BenchmarkSignalQuit
				|| BenchmarkSignalHanged
				|| BenchmarkSignalTimedout
				|| BenchmarkException!=nullptr
				) {
				QString imageName=MinerExeName();
				// maybe will have to KILL process
//				KillProspectorClaymoreMinerBase(imageName);
				KillProspectorClaymoreMinerBase(BenchmarkHandle->processId());
				if (BenchmarkSignalTimedout) {
					throw LException("Benchmark timedou");
					}
				if (BenchmarkException!=nullptr) {
					throw *BenchmarkException;
					}
				if (BenchmarkSignalQuit) {
					throw LException("Termined by user request");
					}
				if (BenchmarkSignalFinnished) {
					break;
					}
				keepRunning=false;
				break;
				}
			QThread::msleep(1000); // wait a second reduce CPU load
			}
		BenchmarkHandle->waitForFinished(20*1000); // Wait up to 20s for exit
		}
	catch (LException ex) {
		BenchmarkThreadRoutineCatch(ex);
		}
//	finally {
		BenchmarkAlgorithm->BenchmarkSpeed=0;

		if (_database==nullptr) {
			try {
				_database=new ProspectorDatabase(WorkingDirectory()+"info.db");
				}
			catch (QException e) {
				Helpers::ConsolePrint(MinerTag(), e.what());
				}
			}

		sessions session=_database->LastSession();
		QDateTime sessionStart=QDateTime::fromString(session.start);
		if (sessionStart<startTime) {
			throw LException("Session not recorded!");
			}

		QVector<hashrates> hashrates_=_database->QuerySpeedsForSession(session.id);

		double speed=0;
		int speedRead=0;
		foreach (hashrates hashrate, hashrates_) {
			if (hashrate.coin==_MiningSetup->MinerName && hashrate.rate>0) {
				speed+=hashrate.rate;
				speedRead++;
				}
			}

		BenchmarkAlgorithm->BenchmarkSpeed=(speed/speedRead)*(1-DevFee);

		BenchmarkThreadRoutineFinish();
//		}
}

void Prospector::BenchmarkOutputErrorDataReceivedImpl(QString outdata)
{
	CheckOutdata(outdata);
}

bool Prospector::BenchmarkParseLine(QString outdata)
{
	Helpers::ConsolePrint("BENCHMARK", outdata);
	return false;
}
