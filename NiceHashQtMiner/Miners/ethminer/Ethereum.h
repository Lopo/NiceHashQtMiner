#ifndef MINERS_QTHMINER_ETHEREUM_H_
#define MINERS_QTHMINER_ETHEREUM_H_


class QString;


class Ethereum
{
public:
//	static Ethereum();

	static QString CurrentBlockNum;
	static void GetCurrentBlock(QString worker);
};

#endif
