#ifndef FORMS_COMPONENTS_GROUPPROFITCONTROL_H_
#define FORMS_COMPONENTS_GROUPPROFITCONTROL_H_

#include <QtWidgets>


class GroupProfitControl
	: public QWidget
{
	Q_OBJECT
public:
	GroupProfitControl(QListWidget* parent=Q_NULLPTR);
	virtual ~GroupProfitControl() {};
	void UpdateProfitStats(QString groupName, QString deviceStringInfo, QString speedString, QString btcRateString, QString currencyRateString);

private:
	QGroupBox* groupBoxMinerGroup=nullptr;
	QLabel* labelSpeedIndicator=nullptr;
	QLabel* labelCurentcyPerDayVaue=nullptr;
	QLabel* labelBTCRateValue=nullptr;
	QLabel* labelBTCRateIndicator=nullptr;
	QLabel* labelSpeedValue=nullptr;
	void InitializeComponent();
};

#endif
