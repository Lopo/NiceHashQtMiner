#include "Forms/Components/BenchmarkOptions.h"
#include "Enums.h"
#include "International.h"

BenchmarkOptions::BenchmarkOptions(QWidget* parent)
	: QWidget(parent)
{
	InitializeComponent();
}

void BenchmarkOptions::InitializeComponent()
{
	setObjectName("BenchmarkOptions");

	groupBox1=new QGroupBox(this);
	groupBox1->setObjectName("groupBox1");
	groupBox1->setGeometry(3, 3, 198, 103);
	groupBox1->setStyleSheet("QGroupBox { border: 1px solid; margin-top: 0.3em;} QGroupBox::title { top: -0.4em; left: 10px;}");
	groupBox1->setTitle("Benchmark Type");

	radioButton_PreciseBenchmark=new QRadioButton(groupBox1);
	radioButton_PreciseBenchmark->setObjectName("radioButton_PreciseBenchmark");
	radioButton_PreciseBenchmark->setGeometry(6, 77, 139, 17);
	radioButton_PreciseBenchmark->setText("&Precise (will take longer)");
	connect(radioButton_PreciseBenchmark, SIGNAL(toggled(bool)), this, SLOT(RadioButton_PreciseBenchmark_CheckedChanged(bool)));

	radioButton_StandardBenchmark=new QRadioButton(groupBox1);
	radioButton_StandardBenchmark->setObjectName("radioButton_StandardBenchmark");
	radioButton_StandardBenchmark->setGeometry(6, 48, 68, 17);
	radioButton_StandardBenchmark->setText("&Standard");
	connect(radioButton_StandardBenchmark, SIGNAL(toggled(bool)), this, SLOT(RadioButton_StandardBenchmark_CheckedChanged(bool)));

	radioButton_QuickBenchmark=new QRadioButton(groupBox1);
	radioButton_QuickBenchmark->setObjectName("radioButton_QuickBenchmark");
	radioButton_QuickBenchmark->setGeometry(6, 19, 148, 17);
	radioButton_QuickBenchmark->setText("&Quick (can be inacurate)");
	connect(radioButton_QuickBenchmark, SIGNAL(toggled(bool)), this, SLOT(RadioButton_QuickBenchmark_CheckedChanged(bool)));

	resize(208, 108);
}

void BenchmarkOptions::SetPerformanceType(const Enums::BenchmarkPerformanceType performanceType)
{
	switch (performanceType) {
		case Enums::BenchmarkPerformanceType::Quick:
			radioButton_QuickBenchmark->setChecked(true);
			_PerformanceType=Enums::BenchmarkPerformanceType::Quick;
			break;
		case Enums::BenchmarkPerformanceType::Standard:
			radioButton_StandardBenchmark->setChecked(true);
			_PerformanceType=Enums::BenchmarkPerformanceType::Standard;
			break;
		case Enums::BenchmarkPerformanceType::Precise:
			radioButton_PreciseBenchmark->setChecked(true);
			_PerformanceType=Enums::BenchmarkPerformanceType::Precise;
			break;
		default:
			radioButton_StandardBenchmark->setChecked(true);
			break;
		}
}

void BenchmarkOptions::InitLocale()
{
	groupBox1->setTitle(International::GetText("BenchmarkOptions_Benchmark_Type"));
	radioButton_QuickBenchmark->setText(International::GetText("Form_Benchmark_radioButton_QuickBenchmark"));
	radioButton_StandardBenchmark->setText(International::GetText("Form_Benchmark_radioButton_StandardBenchmark"));
	radioButton_PreciseBenchmark->setText(International::GetText("Form_Benchmark_radioButton_PreciseBenchmark"));
}

void BenchmarkOptions::RadioButton_QuickBenchmark_CheckedChanged(const bool checked)
{
	if (checked) {
		_PerformanceType=Enums::BenchmarkPerformanceType::Quick;
		}
}

void BenchmarkOptions::RadioButton_StandardBenchmark_CheckedChanged(const bool checked)
{
	if (checked) {
		_PerformanceType=Enums::BenchmarkPerformanceType::Standard;
		}
}

void BenchmarkOptions::RadioButton_PreciseBenchmark_CheckedChanged(const bool checked)
{
	if (checked) {
		_PerformanceType=Enums::BenchmarkPerformanceType::Precise;
		}
}
