#include "Forms/Form_ChooseLanguage.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Globals.h"
#include "International.h"


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

	textBox_TOS->setPlainText(TosText);
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
	textBox_TOS->setText("NiceHash Miner Legacy Term Of Use NiceHash operates as in intermediate service by providing hashing power from hashing power owners to hashing power buyers. NiceHash does not directly provide it's own hashing power. We will do our best to provide stable, secure and feature full service. We do not take responsibility for any kind of hashing power loss or funds loss and do not take any kind of financial, material, legal or other responsibilities for any issues that my arise from using NiceHash Miner Legacy. NiceHash service and it's products (NiceHash Miner Legacy, etc.) is still in development, therefore some bugs or other issues may arise. We will work hard to fix any issues as soon as possible, add new features and overall improve our service. NiceHash reserves the rights to seize any funds of suspicious illegal activity such as mining with botnets, money laundering, hacking attempts, etc.\r\n\r\nAs a user of NiceHash Miner Legacy, you are providing your hashing power (your are a seller) to the hashing power buyers though NiceHash's hashing power marketplace. You earn Bitcoins from selling your hashing power for every valid share your miner generates and is accepted by NiceHash service. In some cases no shares are sent to or are accepted by NiceHash service. This cases are rare and includes usage of slower hardware, software or network errors or simmilar. In these cases (no shares generated and accepted by NiceHash service) no reward in form of Bitcoins can be accounted to you. Payouts are automatic and are paid to BTC address, used in NiceHash Miner Legacy. Payment schedule can be found in FAQ on our website and is subject to change. You can always monitor your statistics on this site: <a href=\"https://www.nicehash.com/?p=myminer\">https://www.nicehash.com/?p=myminer</a> \r\n\r\nIf you have any questions relating to these Terms of Use, your rights and obligations arising from these Terms and/or your use of the NiceHash service, or any other matter, please contact us at support@nicehash.com.");
	textBox_TOS->setReadOnly(true);

	setObjectName("Form_ChooseLanguage");
	resize(455, 427);
	setWindowTitle("NiceHash QtMiner EULA / Choose Language");
	QFont f6;
	f6.setPointSize(6);
	setFont(f6);
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
