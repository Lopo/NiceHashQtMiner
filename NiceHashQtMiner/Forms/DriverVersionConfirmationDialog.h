#ifndef FORMS__DRIVERVERSIONCONFIRMATIONDIALOG_H_
#define FORMS__DRIVERVERSIONCONFIRMATIONDIALOG_H_

#include <QtWidgets>


class DriverVersionConfirmationDialog
	: public QDialog
{
	Q_OBJECT

public:
	DriverVersionConfirmationDialog(QWidget* parent=Q_NULLPTR);

private:
	QLabel* labelWarning=nullptr;
	QPushButton* buttonOK=nullptr;
	QCheckBox* chkBoxDontShowAgain=nullptr;
	QLabel* linkToDriverDownloadPage=nullptr;
	void InitializeComponent();

private slots:
	void ButtonOK_Click();
	void LinkToDriverDownloadPage_LinkClicked(const QString&);
};

#endif
