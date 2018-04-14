#ifndef FORMS_FORM_3RDPARTY_TOS_H_
#define FORMS_FORM_3RDPARTY_TOS_H_

#include <QtWidgets>


class Form_3rdParty_TOS
	: public QDialog
{
	Q_OBJECT

public:
	Form_3rdParty_TOS(QWidget* parent=Q_NULLPTR);

private:
	QPushButton* button_Agree=nullptr;
	QPushButton* button_Decline=nullptr;
	QLabel* label_Tos=nullptr;
	void InitializeComponent();

private Q_SLOTS:
	void Button_Agree_Click();
	void Button_Decline_Click();
};

#endif
