#include "Forms/Form_3rdParty_TOS.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "International.h"


Form_3rdParty_TOS::Form_3rdParty_TOS(QWidget* parent)
	: QDialog(parent)
{
	InitializeComponent();

	// @todo update 3rd party TOS
	setWindowTitle(International::GetText("Form_Main_3rdParty_Title"));
	label_Tos->setText(International::GetText("Form_Main_3rdParty_Text"));
	button_Agree->setText(International::GetText("Form_Main_3rdParty_Button_Agree_Text"));
	button_Decline->setText(International::GetText("Form_Main_3rdParty_Button_Refuse_Text"));
}

void Form_3rdParty_TOS::InitializeComponent()
{
	button_Agree=new QPushButton(this);
	button_Agree->setObjectName("button_Agree");
	button_Agree->setText("button1");
	button_Agree->setGeometry(12, 260, 146, 23);
	connect(button_Agree, SIGNAL(clicked()), this, SLOT(Button_Agree_Click()));

	button_Decline=new QPushButton(this);
	button_Decline->setObjectName("button_Decline");
	button_Decline->setText("button2");
	button_Decline->setGeometry(226, 260, 146, 23);
	connect(button_Decline, SIGNAL(clicked()), this, SLOT(Button_Decline_Click()));

/*	QScrollArea* sa=new QScrollArea(this);
	sa->setBackgroundRole(QPalette::Light);
//	sa->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	sa->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	sa->setWidgetResizable(true);
	label_Tos=new QLabel(sa);*/
	label_Tos=new QLabel(this);
	label_Tos->move(12, 9);
	label_Tos->setMaximumWidth(370);
	label_Tos->setObjectName("label_Tos");
//	label_Tos->resize(35, 13);
	label_Tos->setText("label1");
	label_Tos->setAlignment(Qt::AlignCenter);
	label_Tos->setWordWrap(true);
	label_Tos->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
//	sa->setWidget(label_Tos);

	setObjectName("Form_ClaymoreTOS");
	resize(384, 295);
	setWindowTitle("Disclaimer on usage of 3rd party software");
	QFont f6;
	f6.setPointSize(6);
	setFont(f6);
	setWindowIcon(QIcon());
	setGeometry( // CenterScreen
		QStyle::alignedRect(
			Qt::LeftToRight,
			Qt::AlignCenter,
			size(),
			qApp->desktop()->availableGeometry()
			)
		);
}

void Form_3rdParty_TOS::Button_Agree_Click()
{
	ConfigManager.generalConfig->Use3rdPartyMiners=Enums::Use3rdPartyMiners::YES;
	close();
}

void Form_3rdParty_TOS::Button_Decline_Click()
{
	ConfigManager.generalConfig->Use3rdPartyMiners=Enums::Use3rdPartyMiners::NO;
	close();
}
