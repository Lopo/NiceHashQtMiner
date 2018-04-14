#include "Qt/NHMCashRegisterBitcoin.h"
#include "Utils/Links.h"
#include <QDesktopServices>
#include <QUrl>


NHMCashRegisterBitcoin::NHMCashRegisterBitcoin(QWidget* parent/*=Q_NULLPTR*/, Qt::WindowFlags f/*=Qt::WindowFlags()*/)
	: ClickableLabel(parent)
{
}

void NHMCashRegisterBitcoin::mousePressEvent(QMouseEvent* event)
{
	QDesktopServices::openUrl(Links::NhmPayingFaq);
}
