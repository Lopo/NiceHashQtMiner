#ifndef FORMS_FORM_CHOOSELANGUAGE_H_
#define FORMS_FORM_CHOOSELANGUAGE_H_

#include <QtWidgets>


class Form_ChooseLanguage
	: public QDialog
{
	Q_OBJECT

public:
	Form_ChooseLanguage(QWidget* parent=Q_NULLPTR);

private:
	QLabel* label_Instruction=nullptr;
	QComboBox* comboBox_Languages=nullptr;
	QPushButton* button_OK=nullptr;
	QCheckBox* checkBox_TOS=nullptr;
	QTextEdit* textBox_TOS=nullptr;
	void InitializeComponent();

private Q_SLOTS:
	void Button_OK_Click();
	void CheckBox1_CheckedChanged(int);
};

#endif
