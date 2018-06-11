#include "Forms/DriverVersionConfirmationDialog.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Utils/Helpers.h"
#include "International.h"
#include <QDesktopServices>
#include <QUrl>
#include <QString>


DriverVersionConfirmationDialog::DriverVersionConfirmationDialog(QWidget* parent)
	: QDialog(parent)
{
	InitializeComponent();

	setWindowTitle(International::GetText("DriverVersionConfirmationDialog_title"));
	labelWarning->setText(International::GetText("DriverVersionConfirmationDialog_labelWarning"));
	linkToDriverDownloadPage->setText(QString("<a href=\"#\">%1</a>").arg(International::GetText("DriverVersionConfirmationDialog_linkToDriverDownloadPage")));
	chkBoxDontShowAgain->setText(International::GetText("DriverVersionConfirmationDialog_chkBoxDontShowAgain"));
	buttonOK->setText(International::GetText("Global_OK"));
}

void DriverVersionConfirmationDialog::InitializeComponent()
{
	labelWarning=new QLabel(this);
	labelWarning->setObjectName("labelWarning");
	labelWarning->setGeometry(7, 10, 488, 30);
	labelWarning->setText("You\'re not using optimal AMD Driver version. Most stable driver for mining is the 15.7.1 version.\r\nWe strongly suggest you to use this driver version.");

	buttonOK=new QPushButton(this);
	buttonOK->setObjectName("buttonOK");
	buttonOK->setGeometry(410, 67, 75, 23);
	buttonOK->setText("&OK");
	connect(buttonOK, SIGNAL(clicked()), this, SLOT(ButtonOK_Click()));

	chkBoxDontShowAgain=new QCheckBox(this);
	chkBoxDontShowAgain->setObjectName(("chkBoxDontShowAgain"));
	chkBoxDontShowAgain->setGeometry(7, 70, 180, 18);
	chkBoxDontShowAgain->setText("&Do not show this warning again");

	linkToDriverDownloadPage=new QLabel(this);
	linkToDriverDownloadPage->setObjectName("linkToDriverDownloadPage");
	linkToDriverDownloadPage->setGeometry(5, 44, 149, 13);
	linkToDriverDownloadPage->setText("<a href=\"#\">&Link to Driver Download Page</a>");
	connect(linkToDriverDownloadPage, SIGNAL(linkActivated(const QString&)), this, SLOT(LinkToDriverDownloadPage_LinkClicked(const QString&)));

	setTabOrder(buttonOK, linkToDriverDownloadPage);
	setTabOrder(linkToDriverDownloadPage, chkBoxDontShowAgain);

	setObjectName("DriverVersionConfirmationDialog");
	setFixedSize(497, 97);
	setWindowIcon(QIcon());
	setWindowTitle("Update AMD Driver Recommended");
}

void DriverVersionConfirmationDialog::ButtonOK_Click()
{
	if (chkBoxDontShowAgain->isChecked()) {
		Helpers::ConsolePrint("NICEHASH", "Setting ShowDriverVersionWarning to false");
		ConfigManager.generalConfig->ShowDriverVersionWarning=false;
		}
	close();
}

void DriverVersionConfirmationDialog::LinkToDriverDownloadPage_LinkClicked(const QString&)
{
	QDesktopServices::openUrl(QUrl("http://support.amd.com/en-us/download/linux"));
}
