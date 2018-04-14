#include "Forms/Components/BenchmarkLimitControl.h"
#include "International.h"


BenchmarkLimitControl::BenchmarkLimitControl(QWidget* parent)
	: QWidget(parent)
{
	InitializeComponent();

	_textBoxes=new QVector<QSpinBox*>({textBoxQuick, textBoxStandard, textBoxPrecise});
}

void BenchmarkLimitControl::InitializeComponent()
{
	setObjectName("BenchmarkLimitControl1");

	QFont f7;
	f7.setPointSize(6);
	QFont f6;
	f6.setPointSize(6);

	groupBox1=new QGroupBox(this);
	groupBox1->setObjectName("groupBox1");
	groupBox1->setGeometry(0, 0, 173, 117);
	groupBox1->setFont(f7);
	groupBox1->setStyleSheet("QGroupBox { border: 1px solid; margin-top: 0.3em;} QGroupBox::title { top: -0.4em; left: 10px;}");
	groupBox1->setTitle("groupName");

//	flowLayoutPanel1=new QWidget(groupBox1);
//	flowLayoutPanel1=new QVBoxLayout(groupBox1);
//	flowLayoutPanel1->setObjectName("flowLayoutPanel1");
//	flowLayoutPanel1->setGeometry(QRect(3, 16, 167, 98));

//	panel1=new QWidget(groupBox1);
//	panel1->setObjectName("panel1");
//	panel1->setGeometry(0, 0, 200, 32);

	textBoxQuick=new QSpinBox(groupBox1);
	textBoxQuick->setObjectName("textBoxQuick");
	textBoxQuick->setGeometry(62, 22, 100, 20);
	textBoxQuick->setFont(f7);
//	textBoxQuick->setButtonSymbols(QAbstractSpinBox::NoButtons);
	textBoxQuick->setRange(1, 999);
	textBoxQuick->setCursor(QCursor(Qt::IBeamCursor));
	connect(textBoxQuick, SIGNAL(valueChanged(int)), this, SLOT(TextBoxQuick_TextChanged()));
	connect(textBoxQuick, SIGNAL(editingFinished()), this, SLOT(TextBoxQuick_TextChanged()));

	labelQuick=new QLabel(groupBox1);
	labelQuick->setObjectName("labelQuick");
	labelQuick->setFont(f6);
	labelQuick->setGeometry(3, 25, 55, 13);
	labelQuick->setText("Quick");

//	panel3=new QWidget(groupBox1);
//	panel3->setObjectName("panel3");
//	panel3->setGeometry(0, 32, 200, 32);

	textBoxStandard=new QSpinBox(groupBox1);
	textBoxStandard->setObjectName("textBoxStandard");
	textBoxStandard->setGeometry(62, 54, 100, 20);
	textBoxStandard->setFont(f7);
//	textBoxStandard->setButtonSymbols(QAbstractSpinBox::NoButtons);
	textBoxStandard->setRange(1, 999);
	textBoxStandard->setCursor(QCursor(Qt::IBeamCursor));
	connect(textBoxStandard, SIGNAL(valueChanged(int)), this, SLOT(TextBoxStandard_TextChanged()));
	connect(textBoxStandard, SIGNAL(editingFinished()), this, SLOT(TextBoxStandard_TextChanged()));

	labelStandard=new QLabel(groupBox1);
	labelStandard->setObjectName("labelStandard");
	labelStandard->setFont(f6);
	labelStandard->setGeometry(3, 57, 55, 13);
	labelStandard->setText("Standard");

//	panel2=new QWidget(groupBox1);
//	panel2->setObjectName("panel2");
//	panel2->setGeometry(0, 64, 200, 32);

	textBoxPrecise=new QSpinBox(groupBox1);
	textBoxPrecise->setObjectName("textBoxPrecise");
	textBoxPrecise->setGeometry(62, 86, 100, 20);
	textBoxPrecise->setFont(f7);
//	textBoxPrecise->setButtonSymbols(QAbstractSpinBox::NoButtons);
	textBoxPrecise->setRange(1, 999);
	textBoxPrecise->setCursor(QCursor(Qt::IBeamCursor));
	connect(textBoxPrecise, SIGNAL(valueChanged(int)), this, SLOT(TextBoxPrecise_TextChanged()));
	connect(textBoxPrecise, SIGNAL(editingFinished()), this, SLOT(TextBoxPrecise_TextChanged()));

	labelPrecise=new QLabel(groupBox1);
	labelPrecise->setObjectName("labelPrecise");
	labelPrecise->setFont(f6);
	labelPrecise->setGeometry(3, 89, 55, 13);
	labelPrecise->setText("Precise");

	resize(179, 117);
}

void BenchmarkLimitControl::setGroupName(const QString value)
{
	groupBox1->setTitle(value);
}

void BenchmarkLimitControl::setTimeLimits(QVector<int>* value)
{
	if (value==nullptr) {
		return;
		}
	_timeLimits=value;
	for (int indexKey=0; indexKey<_timeLimits->length(); ++indexKey) {
		(*_textBoxes)[indexKey]->setValue((*_timeLimits)[indexKey]);
		}
}

void BenchmarkLimitControl::SetToolTip(QToolTip* toolTip, QString groupTypeName)
{
	labelQuick->setToolTip(International::GetText("Form_Settings_ToolTip_BenchmarkTimeLimits").arg(International::GetText("Quick")).arg(groupTypeName)+".");
	labelStandard->setToolTip(International::GetText("Form_Settings_ToolTip_BenchmarkTimeLimits").arg(International::GetText("Standard")).arg(groupTypeName)+".");
	labelPrecise->setToolTip(International::GetText("Form_Settings_ToolTip_BenchmarkTimeLimits").arg(International::GetText("Precise")).arg(groupTypeName)+".");
}

void BenchmarkLimitControl::InitLocale()
{
	labelQuick->setText(International::GetText("Quick")+":");
	labelStandard->setText(International::GetText("Standard")+":");
	labelPrecise->setText(International::GetText("Precise")+":");
}

void BenchmarkLimitControl::TextBoxQuick_TextChanged()
{
	SetTimeLimit(Enums::BenchmarkPerformanceType::Quick, textBoxQuick->value());
}

void BenchmarkLimitControl::TextBoxStandard_TextChanged()
{
	SetTimeLimit(Enums::BenchmarkPerformanceType::Standard, textBoxStandard->value());
}

void BenchmarkLimitControl::TextBoxPrecise_TextChanged()
{
	SetTimeLimit(Enums::BenchmarkPerformanceType::Precise, textBoxPrecise->value());
}

void BenchmarkLimitControl::SetTimeLimit(Enums::BenchmarkPerformanceType type, int num)
{
	if (_timeLimits==nullptr) {
		return;
		}
	(*_timeLimits)[(int)type]=num;
}
