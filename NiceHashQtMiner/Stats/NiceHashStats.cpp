#define __NiceHashStats_cpp__
#include "Stats/NiceHashStats.h"
#include "Enums.h"
#include "Miners/MinersManager.h"
#include "Utils/BitcoinAddress.h"
#include "Devices/ComputeDeviceManager.h"
#include "Switching/NHSmaData.h"
#include "ExchangeRateAPI.h"
#include "Stats/NiceHashSocket.h"

#include <QtNetwork>
#include <QtConcurrent/QtConcurrentRun>
#include <QApplication>
#if WITH_DEBUG
#include <iostream>
#include <QException>
#endif


extern QNetworkAccessManager* qnam;

double NiceHashStats::Balance_=0;
QString NiceHashStats::Version_="";
NiceHashSocket* NiceHashStats::_socket=nullptr;
QTimer* NiceHashStats::_deviceUpdateTimer=nullptr;


bool NiceHashStats::IsAlive()
{
	if (_socket==nullptr) {
		return false;
		}
	return _socket->IsAlive();
}

void NiceHashStats::StartConnection(QString address)
{
	if (_socket==nullptr) {
		_socket=new NiceHashSocket(address);
		connect(_socket, SIGNAL(ConnectionEstablished()), this, SLOT(SocketOnOnConnectionEstablished()));
		connect(_socket, SIGNAL(DataReceived(QString)), this, SLOT(SocketOnOnDataReceived(QString)));
		connect(_socket, SIGNAL(ConnectionLost()), this, SLOT(SocketOnOnConnectionLost()));
		}
	_socket->StartConnection();
	_deviceUpdateTimer=new QTimer;
	_deviceUpdateTimer->setInterval(DeviceUpdateInterval);
	QObject::connect(_deviceUpdateTimer, SIGNAL(timeout()), this, SLOT(DeviceStatus_Tick()));
}

void NiceHashStats::SocketOnOnConnectionLost()
{
	emit ConnectionLost();
}

void NiceHashStats::SocketOnOnDataReceived(QString e)
{
	try {
		Helpers::ConsolePrint("SOCKET", "Received: "+e);
		QJsonObject message=QJsonDocument::fromJson(e.toLatin1()).object();
		QString method=message.value("method").toString();
		if (method=="sma") { // Try in case stable is not sent, we still get updated paying rates
			try {
				SetStableAlgorithms(message.value("stable").toArray());
				}
			catch (...) {}
			SetAlgorithmRates(message.value("data").toArray());
			}
		else if (method=="balance") {
			SetBalance(message.value("value").toString());
			}
		else if (method=="versions") {
//			SetVersion(message.value("qt").toString());
			}
		else if (method=="burn") {
			emit VersionBurn(message.value("message").toString());
			}
		else if (method=="exchange_rates") {
			SetExchangeRates(message.value("data").toString());
			}
		}
	catch (QException& er) {
		Helpers::ConsolePrint("SOCKET", er.what());
		}
}

void NiceHashStats::SocketOnOnConnectionEstablished()
{
	DeviceStatus_Tick(); // Send device to populate rig stats
	emit ConnectionEstablished();
}

void NiceHashStats::SetAlgorithmRates(QJsonArray data)
{
	try {
		QMap<Enums::AlgorithmType, double> payingDict;
		foreach (QJsonValue algo, data) {
			QJsonArray algoArr=algo.toArray();
			Enums::AlgorithmType algoKey=(Enums::AlgorithmType)algoArr[0].toInt();
			payingDict.insert(algoKey, algoArr[1].toString().toDouble());
			}
		NHSmaData::UpdateSmaPaying(payingDict);
		emit SmaUpdate();
		}
	catch (QException& e) {
		Helpers::ConsolePrint("SOCKET", e.what());
		}
}

void NiceHashStats::SetStableAlgorithms(QJsonArray stable)
{
	QList<Enums::AlgorithmType> stables;
	foreach (QJsonValue algo, stable) {
		stables.append((Enums::AlgorithmType)algo.toInt());
		}
	NHSmaData::UpdateStableAlgorithms(stables);
}

void NiceHashStats::SetBalance(QString balance)
{
	try {
		bool ok=false;
		double bal=balance.toDouble(&ok);
		if (ok) {
			Balance_=bal;
			emit BalanceUpdate(bal);
			}
		}
	catch (QException& e) {
		Helpers::ConsolePrint("SOCKET", e.what());
		}
}

void NiceHashStats::SetVersion(QString version)
{
	Version_=version;
	emit VersionUpdate(version);
}

void NiceHashStats::SetExchangeRates(QString data)
{
	try {
		ExchangeRateJson* exchange=ExchangeRateJson::fromJsonString(data);
		if (exchange!=nullptr && !exchange->exchanges_fiat.empty() && !exchange->exchanges.empty()) {
			QMap<QString, QString> exchangePair;
			foreach (exchangePair, exchange->exchanges) {
				if (exchangePair.contains("coin") && exchangePair.value("coin")=="BTC" && exchangePair.contains("USD")) {
					bool ok=false;
					double usdD=exchangePair.value("USD").toDouble(&ok);
					if (ok) {
						ExchangeRateApi::UsdBtcRate(usdD);
						break;
						}
					}
				}
			ExchangeRateApi::UpdateExchangesFiat(&exchange->exchanges_fiat);
			emit ExchangeUpdate();
			}
		}
	catch (QException& e) {
		Helpers::ConsolePrint("SOCKET", e.what());
		}
}
/*
void NiceHashStats::ExchangeUpdate(QJsonObject data)
{
	try {
		ExchangeRateJson* exchange=ExchangeRateJson::fromJsonObject(data);
		if (exchange!=nullptr && !exchange->exchanges_fiat.empty() && !exchange->exchanges.empty()) {
			QMap<QString, QString> exchangePair;
			foreach (exchangePair, exchange->exchanges) {
				if (exchangePair.contains("coin") && exchangePair.value("coin")=="BTC" && exchangePair.contains("USD")) {
					bool ok=false;
					double usdD=exchangePair.value("USD").toDouble(&ok);
					if (ok) {
						ExchangeRateApi::UsdBtcRate(usdD);
						break;
						}
					}
				}
			}
		}
	catch (QException& e) {
		Helpers::ConsolePrint("SOCKET", e.what());
		}
}
*/
void NiceHashStats::SetCredentials(QString btc, QString worker)
{
	NicehashCredentials data;
	data.btc=btc;
	data.worker=worker;
	if (BitcoinAddress::ValidateBitcoinAddress(data.btc) && BitcoinAddress::ValidateWorkerName(worker)) {
		// Send as task since SetCredentials is called from UI threads
		QtConcurrent::run(_socket, &NiceHashSocket::SendData, data.asJson(), false/*def value*/);
		}
}

void NiceHashStats::DeviceStatus_Tick()
{
	QList<ComputeDevice*>* devices=ComputeDeviceManager.Available.Devices;
	QList<QJsonArray>* deviceList=new QList<QJsonArray>;
	QList<int>* activeIDs=MinersManager::GetActiveMinersIndexes();
	foreach (ComputeDevice* device, *devices) {
		try {
			QJsonArray array;
			array.append(device->Index());
			array.append(device->Name);
			int status=(int32_t)activeIDs->contains(device->Index())+((int)device->DeviceType+1)*2;
			array.append(status);
			array.append((int)round(device->Load()));
			array.append((int)round(device->Temp()));
			array.append(device->FanSpeed());

			deviceList->append(array);
			}
		catch (QException& e) {
			Helpers::ConsolePrint("SOCKET", e.what());
			}
		}
	NicehashDeviceStatus data;
	data.devices=deviceList;
	// This function is run every minute and sends data every run which has two auxiliary effects
	// Keeps connection alive and attempts reconnection if internet was dropped
	if (_socket!=nullptr) {
		_socket->SendData(data.asJson());
		}
}

QString NiceHashStats::GetNiceHashApiData(QString URL, QString worker)
{
#if WITH_DEBUG
std::cout << "GetNiceHashAPIData(" << URL.toStdString() << ", " << worker.toStdString() << std::endl;
//if (URL=="https://api.nicehash.com/api?method=nicehash.service.info") {
//	return "{\"result\":{\"servers\":[{\"name_eng\":\"Europe - Amsterdam\",\"latency\":0,\"index\":0,\"subdomain\":\"eu\",\"operational\":true},{\"name_eng\":\"USA - San Jose\",\"latency\":0,\"index\":1,\"subdomain\":\"usa\",\"operational\":true},{\"name_eng\":\"China - Hong Kong\",\"latency\":145,\"index\":2,\"subdomain\":\"hk\",\"operational\":true},{\"name_eng\":\"Japan - Tokyo\",\"latency\":95,\"index\":3,\"subdomain\":\"jp\",\"operational\":true},{\"name_eng\":\"India - Chennai\",\"latency\":217,\"index\":4,\"subdomain\":\"in\",\"operational\":true},{\"name_eng\":\"Brazil - Sao Paulo\",\"latency\":191,\"index\":5,\"subdomain\":\"br\",\"operational\":true}],\"exchanges\":[{\"USD\":\"4164.9\",\"coin\":\"BTC\"},{\"BTC\":\"0.01208674\",\"coin\":\"LTC\"},{\"BTC\":\"0.06910416\",\"coin\":\"ETH\"},{\"BTC\":\"0.05436284\",\"coin\":\"XMR\"},{}],\"exchanges_fiat\":{\"CHF\":0.97192,\"HRK\":6.3673,\"MXN\":18.145,\"ZAR\":13.56,\"INR\":65.022,\"CNY\":6.6313,\"THB\":33.32,\"AUD\":1.2714,\"ILS\":3.5105,\"KRW\":1139.1,\"JPY\":112.39,\"PLN\":3.6456,\"GBP\":0.7531,\"IDR\":13454.0,\"HUF\":264.02,\"PHP\":50.964,\"TRY\":3.5646,\"RUB\":57.536,\"HKD\":7.8058,\"EUR\":0.84839,\"DKK\":6.3143,\"CAD\":1.2453,\"MYR\":4.2275,\"BGN\":1.6593,\"NOK\":7.9353,\"RON\":3.8863,\"SGD\":1.3594,\"CZK\":21.961,\"SEK\":8.0958,\"NZD\":1.3916,\"BRL\":3.133}},\"method\":\"nicehash.service.info\"}";
//	}
//if (URL=="https://api.nicehash.com/api?method=simplemultialgo.info") {
//	return "{\"result\":{\"simplemultialgo\":[{\"paying\":\"0.00355687\",\"port\":3333,\"name\":\"scrypt\",\"algo\":0},{\"paying\":\"0.00000019\",\"port\":3334,\"name\":\"sha256\",\"algo\":1},{\"paying\":\"0\",\"port\":3335,\"name\":\"scryptnf\",\"algo\":2},{\"paying\":\"0.00003657\",\"port\":3336,\"name\":\"x11\",\"algo\":3},{\"paying\":\"0.00062885\",\"port\":3337,\"name\":\"x13\",\"algo\":4},{\"paying\":\"0.00025293\",\"port\":3338,\"name\":\"keccak\",\"algo\":5},{\"paying\":\"0.00053137\",\"port\":3339,\"name\":\"x15\",\"algo\":6},{\"paying\":\"0.00459824\",\"port\":3340,\"name\":\"nist5\",\"algo\":7},{\"paying\":\"0.40330146\",\"port\":3341,\"name\":\"neoscrypt\",\"algo\":8},{\"paying\":\"0.0054069\",\"port\":3342,\"name\":\"lyra2re\",\"algo\":9},{\"paying\":\"0\",\"port\":3343,\"name\":\"whirlpoolx\",\"algo\":10},{\"paying\":\"0.00037175\",\"port\":3344,\"name\":\"qubit\",\"algo\":11},{\"paying\":\"0.00031018\",\"port\":3345,\"name\":\"quark\",\"algo\":12},{\"paying\":\"0\",\"port\":3346,\"name\":\"axiom\",\"algo\":13},{\"paying\":\"0.00623245\",\"port\":3347,\"name\":\"lyra2rev2\",\"algo\":14},{\"paying\":\"0\",\"port\":3348,\"name\":\"scryptjanenf16\",\"algo\":15},{\"paying\":\"0\",\"port\":3349,\"name\":\"blake256r8\",\"algo\":16},{\"paying\":\"0\",\"port\":3350,\"name\":\"blake256r14\",\"algo\":17},{\"paying\":\"0\",\"port\":3351,\"name\":\"blake256r8vnl\",\"algo\":18},{\"paying\":\"110\",\"port\":3352,\"name\":\"hodl\",\"algo\":19},{\"paying\":\"0.00635731\",\"port\":3353,\"name\":\"daggerhashimoto\",\"algo\":20},{\"paying\":\"0.00004927\",\"port\":3354,\"name\":\"decred\",\"algo\":21},{\"paying\":\"360.21528968\",\"port\":3355,\"name\":\"cryptonight\",\"algo\":22},{\"paying\":\"0.00063431\",\"port\":3356,\"name\":\"lbry\",\"algo\":23},{\"paying\":\"446.80552798\",\"port\":3357,\"name\":\"equihash\",\"algo\":24},{\"paying\":\"0.00009005\",\"port\":3358,\"name\":\"pascal\",\"algo\":25},{\"paying\":\"0.0052036\",\"port\":3359,\"name\":\"x11gost\",\"algo\":26},{\"paying\":\"0.0000542\",\"port\":3360,\"name\":\"sia\",\"algo\":27},{\"paying\":\"0.00005012\",\"port\":3361,\"name\":\"blake2s\",\"algo\":28},{\"paying\":\"0.00664862\",\"port\":3362,\"name\":\"skunk\",\"algo\":29}]},\"method\":\"simplemultialgo.info\"}";
//	}
//if (URL=="https://api.nicehash.com/nicehashminer?method=version&legacy") {
//	return "{\"version\":\"1.8.1.4\"}";
//	}
#endif
	try {
		QString responseFromServer;
		QString ActiveMinersGroup=MinersManager::GetActiveMinersGroup();

		QNetworkRequest wr=QNetworkRequest(URL);
		wr.setHeader(QNetworkRequest::UserAgentHeader, "NiceHashQtMiner/"+qApp->applicationVersion());
		if (worker.length()>64) {
			worker=worker.mid(0, 64);
			}
		wr.setRawHeader("NiceHash-Worker-ID", worker.toLatin1());
		wr.setRawHeader("NHM-Active-Miners-Group", ActiveMinersGroup.toLatin1());
		QNetworkConfiguration conf=qnam->configuration();
		conf.setConnectTimeout(30*1000);
		qnam->setConfiguration(conf);

		QNetworkReply* reply;
		QEventLoop cLoop;
		QObject::connect(qnam, SIGNAL(finished(QNetworkReply*)), &cLoop, SLOT(quit()));
		reply=qnam->get(wr);
		cLoop.exec();
		reply->deleteLater();
		responseFromServer=QString(reply->readAll());
#if WITH_DEBUG
std::cout << "GetNiceHashAPIData Response: " << responseFromServer.toStdString() << std::endl;
#endif
		if (!responseFromServer.length() || responseFromServer[0]!='{') {
			Helpers::ConsolePrint("NICEHASH", "Not JSON!");
			return nullptr;
			}
		return responseFromServer;
		}
	catch (QException& e) {
		Helpers::ConsolePrint("NICEHASH", e.what());
		return nullptr;
		}
	catch (...) {
		Helpers::ConsolePrint("NICEHASH", "GetNiceHashAPIData err");
		return nullptr;
		}
}

QString NiceHashStats::NicehashCredentials::asJson()
{
	QJsonObject o;
	o.insert("method", method);
	o.insert("btc", btc);
	o.insert("worker", worker);
	return QString(QJsonDocument(o).toJson());
}

QString NiceHashStats::NicehashDeviceStatus::asJson()
{
	QJsonObject o;
	QJsonArray devo;
	if (devices!=nullptr && devices->count()) {
		foreach (QJsonArray device, *devices) {
			devo.append(device);
			}
		o.insert("devices", devo);
		}
	return QString(QJsonDocument(o).toJson());
}

NiceHashStats::ExchangeRateJson* NiceHashStats::ExchangeRateJson::fromJsonString(QString str)
{
	QJsonObject o=QJsonDocument::fromJson(str.toLatin1()).object();
	NiceHashStats::ExchangeRateJson* ret=new NiceHashStats::ExchangeRateJson;

	foreach (QJsonValue eV, o.value("exchanges").toArray()) {
		QMap<QString, QString> m;
		QJsonObject eVo=eV.toObject();
		foreach (QString k, eVo.keys()) {
			m.insert(k, eVo.value(k).toString());
			}
		ret->exchanges.append(m);
		}

	QJsonObject ef=o.value("exchanges_fiat").toObject();
	foreach (QString key, ef.keys()) {
		ret->exchanges_fiat.insert(key, ef.value(key).toDouble());
		}
	
	return ret;
}
