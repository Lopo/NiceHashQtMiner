#ifndef STATS_EXCHANGERATEAPI_H_
#define STATS_EXCHANGERATEAPI_H_

#include <QString>
#include <QMap>
#include <QJsonObject>


class ExchangeRateApi
{
public:
	static inline double UsdBtcRate() { return _usdBtcRate;};
	static void UsdBtcRate(double value);
	static QString ActiveDisplayCurrency/*="USD"*/;
	static void UpdateExchangesFiat(QMap<QString, double>* newExchanges);
	static double ConvertToActiveCurrency(double amount);
	static double GetUsdExchangeRate();
	static double GetKwhPriceInBtc();
/*	static void UpdateApi(QString worker);

	class Result
	{
	public:
		QJsonObject algorithms;
		QJsonObject servers;
		QJsonObject idealratios;
		QList<QMap<QString, QString>> exchanges;
		QMap<QString, double>* exchanges_fiat=nullptr;

		void loadJsonObject(QJsonObject o);
	};

	class ExchangeRateJSON
	{
	public:
		Result result;
		QString method;

		static ExchangeRateJSON* fromJson(QString json);
	};
*/
private:
	const QString ApiUrl="https://api.nicehash.com/api?method=nicehash.service.info";
	static QMap<QString, double> ExchangesFiat;
	static double _usdBtcRate/*=-1*/;
	static bool ConverterActive();
};

#ifdef __ExchangeRateApi_cpp__
	ExchangeRateApi ExchangeRateAPI;
#else
	extern ExchangeRateApi ExchangeRateAPI;
#endif

#endif
