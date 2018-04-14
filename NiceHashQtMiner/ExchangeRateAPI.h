#ifndef EXCHANGERATEAPI_H_
#define EXCHANGERATEAPI_H_

#include <QString>
#include <QMap>
#include <QJsonObject>


class ExchangeRateAPI
{
public:
	QString ActiveDisplayCurrency="USD";
	double ConvertToActiveCurrency(double amount);
	double GetUsdExchangeRate();
	void UpdateApi(QString worker);

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

private:
	const QString ApiUrl="https://api.nicehash.com/api?method=nicehash.service.info";
	QMap<QString, double>* _exchangesFiat=nullptr;
	double _usdBtcRate=-1;
	static bool ConverterActive();
};

#ifndef __ExchangeRateAPI_cpp__
#	define EXTERN extern
#else
#	define EXTERN
#endif
EXTERN ExchangeRateAPI ExchangeRateAPI;
#undef EXTERN

#endif
