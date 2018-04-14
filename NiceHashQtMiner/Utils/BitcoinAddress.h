#ifndef UTILS_BITCOINADDRESS_H_
#define UTILS_BITCOINADDRESS_H_

#include <QString>
#include <QByteArray>


class BitcoinAddress
{
public:
	const static int Size=25;

	static bool ValidateBitcoinAddress(const QString address);
	static bool ValidateWorkerName(const QString workername);
	static bool isAlphaNumeric(const QString strToCheck);

private:
	static QByteArray DecodeBase58(const QString input);
	static QByteArray Hash(const QByteArray bytes);
	static QByteArray SubArray(const QByteArray data, const int index, const int length);
};

#endif
