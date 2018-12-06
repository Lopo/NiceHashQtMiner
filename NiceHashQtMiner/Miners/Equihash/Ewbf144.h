#ifndef MINERS_EQUIHASH_EWBF144_H_
#define MINERS_EQUIHASH_EWBF144_H_

#include "Miners/Equihash/Ewbf.h"


class Ewbf144
	: public Ewbf
{
	Q_OBJECT
public:
	Ewbf144();

protected:
	QStringList GetStartCommand(QString url, QString btcAddress, QString worker) override;
};

#endif
