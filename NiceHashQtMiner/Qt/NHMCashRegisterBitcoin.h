#ifndef QT_NHMCASHREGISTERBITCOIN_H_
#define QT_NHMCASHREGISTERBITCOIN_H_

#include "Qt/ClickableLabel.h"


class NHMCashRegisterBitcoin
	: public ClickableLabel
{
	Q_OBJECT

public:
	explicit NHMCashRegisterBitcoin(QWidget* parent=Q_NULLPTR, Qt::WindowFlags f=Qt::WindowFlags());

protected:
	void mousePressEvent(QMouseEvent* event);
};

#endif
