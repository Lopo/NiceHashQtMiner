#define __ExchangeRateAPI_cpp__
#include "ExchangeRateAPI.h"
#include "Utils/Helpers.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Stats/NiceHashStats.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


bool ExchangeRateAPI::ConverterActive()
{
	return ConfigManager.generalConfig->DisplayCurrency!="USD";
}

double ExchangeRateAPI::ConvertToActiveCurrency(double amount)
{
	if (!ConverterActive()) {
		return amount;
		}

	// if  we are still null after an update something went wrong. just use USD hopefully itll update next tick
	if (_exchangesFiat==nullptr || ActiveDisplayCurrency=="USD") {
		// Moved logging to update for berevity
		return amount;
		}
	if (_exchangesFiat->contains(ActiveDisplayCurrency)) {
		return amount*_exchangesFiat->value(ActiveDisplayCurrency);
		}
	Helpers::ConsolePrint("CurrencyConverter", "Unknown Currency Tag: "+ActiveDisplayCurrency+" falling back to USD rates");
	ActiveDisplayCurrency="USD";
	return amount;
}

double ExchangeRateAPI::GetUsdExchangeRate()
{
	return _usdBtcRate>0
		? _usdBtcRate
		: 0.0;
}

void ExchangeRateAPI::UpdateApi(QString worker)
{
	QString resp=NiceHashStats::GetNiceHashApiData(ApiUrl, worker);
	if (resp!=nullptr) {
		try {
			ExchangeRateJSON* lastResponse=ExchangeRateJSON::fromJson(resp);
			if (lastResponse!=nullptr) { // set that we have a response
				Result lastResult=lastResponse->result;
				if (_exchangesFiat!=nullptr) {delete _exchangesFiat;}
				_exchangesFiat=lastResult.exchanges_fiat;
				if (_exchangesFiat==nullptr) {
					Helpers::ConsolePrint("CurrencyConverter", "Unable to retrieve update, Falling back to USD");
					ActiveDisplayCurrency="USD";
					}
				else {
					ActiveDisplayCurrency=ConfigManager.generalConfig->DisplayCurrency;
					}
				// check if currency avaliable and fill currency list
				QMap<QString, QString> pair;
				foreach (pair, lastResult.exchanges) {
					if (pair.contains("USD") && pair.contains("coin") && pair["coin"]=="BTC" && pair["USD"]!=nullptr) {
						_usdBtcRate=Helpers::ParseDouble(pair["USD"]);
						break;
						}
					}
				}
			}
		catch (std::exception e) {
			Helpers::ConsolePrint("ExchangeRateAPI", "UpdateAPI got Exception: "+QString(e.what()));
			}
		}
	else {
		Helpers::ConsolePrint("ExchangeRateAPI", "UpdateAPI got NULL");
		}
}

void ExchangeRateAPI::Result::loadJsonObject(QJsonObject o)
{
	algorithms=o.value("algorithms").toObject();
	servers=o.value("servers").toObject();
	idealratios=o.value("idealratios").toObject();

	exchanges.clear();
	foreach (QJsonValue ex, o.value("exchanges").toArray()) {
		QMap<QString, QString> t;
		QJsonObject exo=ex.toObject();
		foreach (QString key, exo.keys()) {
			t.insert(key, exo.value(key).toString());
			}
		exchanges.append(t);
		}

	if (exchanges_fiat==nullptr) {
		exchanges_fiat=new QMap<QString, double>;
		}
	else {
		exchanges_fiat->clear();
		}
	exchanges_fiat->clear();
	QJsonObject ef=o.value("exchanges_fiat").toObject();
	foreach (QString key, ef.keys()) {
		exchanges_fiat->insert(key, ef.value(key).toDouble());
		}
}

ExchangeRateAPI::ExchangeRateJSON* ExchangeRateAPI::ExchangeRateJSON::fromJson(QString json)
{
	try {
		ExchangeRateJSON* ret=new ExchangeRateJSON;
		QJsonObject o=QJsonDocument::fromJson(json.toUtf8()).object();
		ret->method=o.value("method").toString();
		ret->result.loadJsonObject(o.value("result").toObject());
		return ret;
		}
	catch (...) {
		return nullptr;
		}
}
