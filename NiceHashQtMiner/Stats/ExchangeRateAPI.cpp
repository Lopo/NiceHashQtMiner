#define __ExchangeRateApi_cpp__
#include "ExchangeRateAPI.h"
#include "Utils/Helpers.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Stats/NiceHashStats.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


QMap<QString, double> ExchangeRateApi::ExchangesFiat;
double ExchangeRateApi::_usdBtcRate=-1;

void ExchangeRateApi::UsdBtcRate(double value)
{
	if (value>0) {
		_usdBtcRate=value;
		Helpers::ConsolePrint("NICEHASH", QString("USD rate updated: %1 BTC").arg(value));
		}
}

QString ExchangeRateApi::ActiveDisplayCurrency="USD";

bool ExchangeRateApi::ConverterActive()
{
	return ConfigManager.generalConfig->DisplayCurrency!="USD";
}

void ExchangeRateApi::UpdateExchangesFiat(QMap<QString, double>* newExchanges)
{
	if (newExchanges==nullptr) {
		return;
		}
	foreach (QString key, newExchanges->keys()) {
		ExchangesFiat.insert(key, newExchanges->value(key));
		}
}

double ExchangeRateApi::ConvertToActiveCurrency(double amount)
{
	if (!ConverterActive()) {
		return amount;
		}

	// if  we are still null after an update something went wrong. just use USD hopefully itll update next tick
	if (!ExchangesFiat.count() || ActiveDisplayCurrency=="USD") {
		return amount;
		}
	if (ExchangesFiat.contains(ActiveDisplayCurrency)) {
		return amount*ExchangesFiat.value(ActiveDisplayCurrency);
		}
	Helpers::ConsolePrint("CurrencyConverter", "Unknown Currency Tag: "+ActiveDisplayCurrency+" falling back to USD rates");
	ActiveDisplayCurrency="USD";
	return amount;
}

double ExchangeRateApi::GetUsdExchangeRate()
{
	return UsdBtcRate()>0
		? UsdBtcRate()
		: 0.0;
}

/**
 * Get price of kW-h in BTC if it is set and exchanges are working
 * Otherwise, returns 0
 */
double ExchangeRateApi::GetKwhPriceInBtc()
{
	double price=ConfigManager.generalConfig->KwhPrice;
	if (price<=0) {
		return 0;
		}
	// Converting with 1/price will give us 1/usdPrice
	double invertedUsdRate=ConvertToActiveCurrency(1/price);
	if (invertedUsdRate<=0) {
		// Should never happen, indicates error in ExchangesFiat
		// Fall back with 0
		Helpers::ConsolePrint("EXCHANGE", "Bitcoin price is unknown, power switching disabled");
		return 0;
		}
	return price/UsdBtcRate();
}
/*
void ExchangeRateApi::UpdateApi(QString worker)
{
	QString resp=NiceHashStats::GetNiceHashApiData(ApiUrl, worker);
	if (resp!=nullptr) {
		try {
			ExchangeRateJSON* lastResponse=ExchangeRateJSON::fromJson(resp);
			if (lastResponse!=nullptr) { // set that we have a response
				Result lastResult=lastResponse->result;
				if (ExchangesFiat!=nullptr) {delete ExchangesFiat;}
				ExchangesFiat=lastResult.exchanges_fiat;
				if (ExchangesFiat==nullptr) {
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

void ExchangeRateApi::Result::loadJsonObject(QJsonObject o)
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
	exchanges_fiat->clear();
	QJsonObject ef=o.value("exchanges_fiat").toObject();
	foreach (QString key, ef.keys()) {
		exchanges_fiat->insert(key, ef.value(key).toDouble());
		}
}

ExchangeRateApi::ExchangeRateJSON* ExchangeRateApi::ExchangeRateJSON::fromJson(QString json)
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
*/