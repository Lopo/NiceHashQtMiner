#include "Forms/Components/GroupProfitControl.h"
#include "International.h"


GroupProfitControl::GroupProfitControl(QWidget* parent)
	: QWidget(parent)
{
	InitializeComponent();

	labelSpeedIndicator->setText(International::GetText("ListView_Speed"));
	labelBTCRateIndicator->setText(International::GetText("Rate"));
}


void GroupProfitControl::InitializeComponent()
{
	setObjectName("GroupProfitControl");

	groupBoxMinerGroup=new QGroupBox(this);
	groupBoxMinerGroup->setObjectName("groupBoxMinerGroup");
	groupBoxMinerGroup->setGeometry(0, 0, 536, 32);
	groupBoxMinerGroup->setTitle("Mining Devices { N/A } ");
	groupBoxMinerGroup->setFlat(true);
	groupBoxMinerGroup->setStyleSheet("QGroupBox { border: 1px solid; margin-top: 0.25em;} QGroupBox::title { top: -0.5em; left: 10px;}");

	labelSpeedIndicator=new QLabel(groupBoxMinerGroup);
	labelSpeedIndicator->setObjectName("labelSpeedIndicator");
	labelSpeedIndicator->setGeometry(6, 16-5, 47+1, 13+1);
	labelSpeedIndicator->setText("Speed:");
	labelSpeedIndicator->setStyleSheet("QLabel { font: \"Sans Serif\"; font-weight: bold; }");

	labelCurentcyPerDayVaue=new QLabel(groupBoxMinerGroup);
	labelCurentcyPerDayVaue->setObjectName("labelCurentcyPerDayVaue");
	labelCurentcyPerDayVaue->setGeometry(434, 16-5, 61+35, 13+1);
	labelCurentcyPerDayVaue->setText("0.00 $/Day");

	labelBTCRateValue=new QLabel(groupBoxMinerGroup);
	labelBTCRateValue->setObjectName("labelBTCRateValue");
	labelBTCRateValue->setGeometry(317, 16-5, 112+2, 13+1);
	labelBTCRateValue->setText("0.00000000 BTC/Day");
	labelBTCRateValue->setStyleSheet("QLabel { font: \"Sans Serif\"; }");

	labelBTCRateIndicator=new QLabel(groupBoxMinerGroup);
	labelBTCRateIndicator->setObjectName("labelBTCRateIndicator");
	labelBTCRateIndicator->setGeometry(279, 16-5, 33+2, 13+1);
	labelBTCRateIndicator->setText("Rate:");

	labelSpeedValue=new QLabel(groupBoxMinerGroup);
	labelSpeedValue->setObjectName("labelSpeedValue");
	labelSpeedValue->setGeometry(56, 16-5, 30+190, 13+1);
	labelSpeedValue->setText("N/A");
	labelSpeedValue->setStyleSheet("QLabel { font: \"Sans Serif\"; font-weight: bold; }");

	setFixedSize(537, 36);
}

void GroupProfitControl::UpdateProfitStats(QString groupName, QString deviceStringInfo, QString speedString, QString btcRateString, QString currencyRateString)
{
	groupBoxMinerGroup->setTitle(International::GetText("Form_Main_MiningDevices").arg(deviceStringInfo));
	labelSpeedValue->setText(speedString);
	labelBTCRateValue->setText(btcRateString);
	labelCurentcyPerDayVaue->setText(currencyRateString);
}
