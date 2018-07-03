#include "Forms/Form_ChooseLanguage.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Globals.h"
#include "International.h"
#include "Eula.h"


Form_ChooseLanguage::Form_ChooseLanguage(QWidget *parent)
	: QDialog(parent)
{
	InitializeComponent();

	// Add language selections list
	QMap<Enums::LanguageType, QString>* lang=International::GetAvailableLanguages();

	comboBox_Languages->clear();
	for (int i=0; i<lang->count(); i++) {
		comboBox_Languages->addItem(lang->value((Enums::LanguageType)i));
		}

	comboBox_Languages->setCurrentIndex(0);

	textBox_TOS->setPlainText(Eula::Text);
	textBox_TOS->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	textBox_TOS->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void Form_ChooseLanguage::InitializeComponent()
{
	label_Instruction=new QLabel(this);
	label_Instruction->setObjectName("label_Instruction");
	label_Instruction->setGeometry(10, 367, 231, 13);
	label_Instruction->setText("Choose a default language for NiceHash QtMiner:");

	comboBox_Languages=new QComboBox(this);
	comboBox_Languages->setObjectName("comboBox_Languages");
	comboBox_Languages->setGeometry(15, 384, 195, 21);
	comboBox_Languages->setEnabled(false);

	button_OK=new QPushButton(this);
	button_OK->setObjectName("button_OK");
	button_OK->setGeometry(216, 382, 106, 23);
	button_OK->setText("OK");
	button_OK->setEnabled(false);
	connect(button_OK, SIGNAL(clicked()), this, SLOT(Button_OK_Click()));

	checkBox_TOS=new QCheckBox(this);
	checkBox_TOS->setObjectName("checkBox_TOS");
	checkBox_TOS->setGeometry(292, 359, 151, 17);
	checkBox_TOS->setText("I accept the Terms Of Use");
	connect(checkBox_TOS, SIGNAL(stateChanged(int)), this, SLOT(CheckBox1_CheckedChanged(int)));

	textBox_TOS=new QTextEdit(this);
	textBox_TOS->setObjectName("textBox_TOS");
	textBox_TOS->setGeometry(13, 13, 430, 340);
	textBox_TOS->setText(Eula::Text);
	textBox_TOS->setReadOnly(true);

	setTabOrder(comboBox_Languages, button_OK);
	setTabOrder(button_OK, checkBox_TOS);
	setTabOrder(checkBox_TOS, textBox_TOS);

	setObjectName("Form_ChooseLanguage");
	resize(455, 427);
	setWindowTitle("NiceHash QtMiner EULA / Choose Language");
}

void Form_ChooseLanguage::Button_OK_Click()
{
	ConfigManager.generalConfig->Language=(Enums::LanguageType)comboBox_Languages->currentIndex();
	ConfigManager.GeneralConfigFileCommit();
	close();
}

void Form_ChooseLanguage::CheckBox1_CheckedChanged(int)
{
	if (checkBox_TOS->isChecked()) {
		ConfigManager.generalConfig->agreedWithTOS=Globals::CurrentTosVer;
		comboBox_Languages->setEnabled(true);
		button_OK->setEnabled(true);
		}
	else {
		ConfigManager.generalConfig->agreedWithTOS=0;
		comboBox_Languages->setEnabled(false);
		button_OK->setEnabled(false);
		}
}
