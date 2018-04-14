#include "Forms/Components/AlgorithmSettingsControl.h"
#include "Utils/Helpers.h"
#include "Globals.h"
#include "International.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Properties/Resources.h"


AlgorithmSettingsControl::AlgorithmSettingsControl(QWidget* parent)
	: QWidget(parent)
{
	InitializeComponent();

	connect(field_LessThreads, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i){ LessThreads_Leave(); });
	connect(fieldBoxBenchmarkSpeed, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double d){ TextChangedBenchmarkSpeed(d); });
	connect(secondaryFieldBoxBenchmarkSpeed, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double d){ SecondaryTextChangedBenchmarkSpeed(d); });
	connect(richTextBoxExtraLaunchParameters, SIGNAL(textChanged()), this, SLOT(TextChangedExtraLaunchParameters()));
}

void AlgorithmSettingsControl::InitializeComponent()
{
	Resources resources;
	QFont fnt;
	fnt.setPointSizeF(6);

	groupBoxSelectedAlgorithmSettings=new QGroupBox(this);
	groupBoxSelectedAlgorithmSettings->setObjectName("groupBoxSelectedAlgorithmSettings");
	groupBoxSelectedAlgorithmSettings->setGeometry(3, 3, 229, 285);
	groupBoxSelectedAlgorithmSettings->setTitle("Selected Algorithm Settings:");
	groupBoxSelectedAlgorithmSettings->setFont(fnt);
	groupBoxSelectedAlgorithmSettings->setStyleSheet("QGroupBox { border: 1px solid; margin-top: 0.3em;} QGroupBox::title { top: -0.4em; left: 10px;}");

	flowLayoutPanel1=new QWidget(groupBoxSelectedAlgorithmSettings);
	flowLayoutPanel1->setObjectName("flowLayoutPanel1");
	flowLayoutPanel1->setGeometry(3, 16, 223, 266);
	flowLayoutPanel1->setFont(fnt);

	labelFieldIndicator_LessThreads=new QLabel(this);
	labelFieldIndicator_LessThreads->setObjectName("label_LessThreads");
	labelFieldIndicator_LessThreads->setGeometry(3+3, 3+6, 98, 13);
	labelFieldIndicator_LessThreads->setText("Label field indicator");

	field_LessThreads=new QSpinBox(flowLayoutPanel1);
	field_LessThreads->setObjectName("field_LessThreads");
//	field_LessThreads->setGeometry(3, 3, 220, 47);
	field_LessThreads->setGeometry(3+3, 3+24, 214, 20);
//	field_LessThreads->setTitle("LessThreads:");
	field_LessThreads->setButtonSymbols(QSpinBox::ButtonSymbols::NoButtons);
	field_LessThreads->setMinimum(0);
	field_LessThreads->setFont(fnt);

	labelFieldIndicator_fieldBoxBenchmarkSpeed=new QLabel(this);
	labelFieldIndicator_fieldBoxBenchmarkSpeed->setObjectName("labelFieldIndicator_fieldBoxBenchmarkSpeed");
	labelFieldIndicator_fieldBoxBenchmarkSpeed->setGeometry(3+3, 56+6, 98, 13);
	labelFieldIndicator_fieldBoxBenchmarkSpeed->setText("Label field indicator");

	fieldBoxBenchmarkSpeed=new QDoubleSpinBox(flowLayoutPanel1);
	fieldBoxBenchmarkSpeed->setObjectName("fieldBoxBenchmarkSpeed");
//	fieldBoxBenchmarkSpeed->setGeometry(3, 56, 220, 47);
	fieldBoxBenchmarkSpeed->setGeometry(3+3, 56+24, 214, 20);
//	fieldBoxBenchmarkSpeed->setTitle("Benchmark Speed (H/s):");
	fieldBoxBenchmarkSpeed->setButtonSymbols(QDoubleSpinBox::ButtonSymbols::NoButtons);
	fieldBoxBenchmarkSpeed->setMinimum(0.0d);
	fieldBoxBenchmarkSpeed->setMaximum(999999999999999);
	fieldBoxBenchmarkSpeed->setFont(fnt);

	labelFieldIndicator_secondaryFieldBoxBenchmarkSpeed=new QLabel(this);
	labelFieldIndicator_secondaryFieldBoxBenchmarkSpeed->setObjectName("labelFieldIndicator_secondaryFieldBoxBenchmarkSpeed");
	labelFieldIndicator_secondaryFieldBoxBenchmarkSpeed->setGeometry(3+3, 109+6, 98, 13);
	labelFieldIndicator_secondaryFieldBoxBenchmarkSpeed->setText("Label field indicator");

	secondaryFieldBoxBenchmarkSpeed=new QDoubleSpinBox(flowLayoutPanel1);
	secondaryFieldBoxBenchmarkSpeed->setObjectName("secondaryFieldBoxBenchmarkSpeed");
//	secondaryFieldBoxBenchmarkSpeed->setGeometry(3, 109, 220, 47);
	secondaryFieldBoxBenchmarkSpeed->setGeometry(3+3, 109+24, 214, 20);
//	secondaryFieldBoxBenchmarkSpeed->setTitle("Secondary Benchmark Speed (H/s):");
	secondaryFieldBoxBenchmarkSpeed->setButtonSymbols(QDoubleSpinBox::ButtonSymbols::NoButtons);
	secondaryFieldBoxBenchmarkSpeed->setMinimum(0.0d);
	secondaryFieldBoxBenchmarkSpeed->setMaximum(999999999999999);
	secondaryFieldBoxBenchmarkSpeed->setFont(fnt);

	groupBoxExtraLaunchParameters=new QGroupBox(flowLayoutPanel1);
	groupBoxExtraLaunchParameters->setObjectName("groupBoxExtraLaunchParameters");
	groupBoxExtraLaunchParameters->setGeometry(3, 162, 217, 95);
	groupBoxExtraLaunchParameters->setTitle("Extra Launch Parameters:");
	groupBoxExtraLaunchParameters->setFont(fnt);
	groupBoxExtraLaunchParameters->setStyleSheet("QGroupBox { border: 1px solid; margin-top: 0.3em;} QGroupBox::title { top: -0.4em; left: 10px;}");

	pictureBox1=new QLabel(groupBoxExtraLaunchParameters);
	pictureBox1->setObjectName("pictureBox1");
	pictureBox1->setGeometry(193, 0, 18, 18);
	pictureBox1->setPixmap(resources.info_black_18);

	richTextBoxExtraLaunchParameters=new QPlainTextEdit(groupBoxExtraLaunchParameters);
	richTextBoxExtraLaunchParameters->setObjectName("richTextBoxExtraLaunchParameters");
	richTextBoxExtraLaunchParameters->setGeometry(3, 16, 211, 82);
	richTextBoxExtraLaunchParameters->setPlainText("");
	richTextBoxExtraLaunchParameters->setFont(fnt);

	setObjectName("AlgorithmSettingsControl");
	resize(235, 291);
	setFont(fnt);
}

void AlgorithmSettingsControl::Deselect()
{
	_selected=false;
	groupBoxSelectedAlgorithmSettings->setTitle(International::GetText("AlgorithmsListView_GroupBox").arg(International::GetText("AlgorithmsListView_GroupBox_NONE")));
	setEnabled(false);
	fieldBoxBenchmarkSpeed->setValue(0);
	secondaryFieldBoxBenchmarkSpeed->setValue(0);
	field_LessThreads->setValue(0);
	richTextBoxExtraLaunchParameters->setPlainText("");
}

void AlgorithmSettingsControl::InitLocale(QToolTip* toolTip1)
{
	labelFieldIndicator_LessThreads->setText(International::GetText("Form_Settings_General_CPU_LessThreads")+":");
	labelFieldIndicator_fieldBoxBenchmarkSpeed->setText(International::GetText("Form_Settings_Algo_BenchmarkSpeed")+":");
	labelFieldIndicator_secondaryFieldBoxBenchmarkSpeed->setText(International::GetText("Form_Settings_Algo_SecondaryBenchmarkSpeed")+":");
	groupBoxExtraLaunchParameters->setTitle(International::GetText("Form_Settings_General_ExtraLaunchParameters"));
	groupBoxExtraLaunchParameters->setToolTip(International::GetText("Form_Settings_ToolTip_AlgoExtraLaunchParameters"));
	pictureBox1->setToolTip(International::GetText("Form_Settings_ToolTip_AlgoExtraLaunchParameters"));
}

QString AlgorithmSettingsControl::ParseStringDefault(QString value)
{
	return value;
}

QString AlgorithmSettingsControl::ParseDoubleDefault(double value)
{
	return value<=0? "" : QString::number(value);
}

void AlgorithmSettingsControl::SetCurrentlySelected(AlgorithmsListViewModel* model, int row, ComputeDevice* computeDevice)
{
	// should not happen ever
	if (row==-1) {
		return;
		}

	_computeDevice=computeDevice;
	QModelIndex idx=model->index(row, 0, QModelIndex());
	Algorithm* algorithm=model->data(idx, Qt::EditRole).value<Algorithm*>();
	if (algorithm!=nullptr) {
		_selected=true;
		_currentlySelectedAlgorithm=algorithm;
		_currentlySelectedLvi=row;
		_model=model;
		setEnabled(model->data(idx, Qt::CheckStateRole)==Qt::Checked);

		groupBoxSelectedAlgorithmSettings->setTitle(International::GetText("AlgorithmsListView_GroupBox").arg(QString("%1 (%2)").arg(algorithm->AlgorithmName, algorithm->MinerBaseTypeName)));

		field_LessThreads->setEnabled(_computeDevice->DeviceGroupType==Enums::DeviceGroupType::CPU && algorithm->MinerBaseType==Enums::MinerBaseType::XmrStackCPU);
		field_LessThreads->setValue(field_LessThreads->isEnabled()? algorithm->LessThreads : 0);
		fieldBoxBenchmarkSpeed->setValue(algorithm->BenchmarkSpeed);
		richTextBoxExtraLaunchParameters->setPlainText(algorithm->ExtraLaunchParameters.join(' '));
		secondaryFieldBoxBenchmarkSpeed->setValue(algorithm->SecondaryBenchmarkSpeed);
		secondaryFieldBoxBenchmarkSpeed->setEnabled(_currentlySelectedAlgorithm->SecondaryNiceHashID!=Enums::AlgorithmType::NONE);
		update();
		}
	else {
		// TODO this should not be null
		}
}

void AlgorithmSettingsControl::HandleCheck(AlgorithmsListViewModel* model, int row)
{
	_model=model;
	if (row==_currentlySelectedLvi) {
		setEnabled(model->data(model->index(row, 0, QModelIndex()), Qt::CheckStateRole)==Qt::Checked);
		}
}

void AlgorithmSettingsControl::ChangeSpeed(AlgorithmsListViewModel* model, int row)
{
	_model=model;
	if (_currentlySelectedLvi==row) {
		Algorithm* algorithm=model->data(model->index(row, 0, QModelIndex()), Qt::EditRole).value<Algorithm*>();
		if (algorithm!=nullptr) {
			fieldBoxBenchmarkSpeed->setValue(algorithm->BenchmarkSpeed);
			secondaryFieldBoxBenchmarkSpeed->setValue(algorithm->SecondaryBenchmarkSpeed);
			}
		}
}

bool AlgorithmSettingsControl::CanEdit()
{
	return _currentlySelectedAlgorithm!=nullptr && _selected;
}

void AlgorithmSettingsControl::TextChangedBenchmarkSpeed(double)
{
	if (!CanEdit()) {
		return;
		}
	_currentlySelectedAlgorithm->BenchmarkSpeed=fieldBoxBenchmarkSpeed->value();
	UpdateSpeedText();
}

void AlgorithmSettingsControl::SecondaryTextChangedBenchmarkSpeed(double)
{
	if (!CanEdit()) {
		return;
		}
	_currentlySelectedAlgorithm->SecondaryBenchmarkSpeed=secondaryFieldBoxBenchmarkSpeed->value();
	UpdateSpeedText();
}

void AlgorithmSettingsControl::UpdateSpeedText()
{
	// update lvi speed
	if (_currentlySelectedLvi!=-1 && _model!=nullptr) {
		_model->setData(_model->index(_currentlySelectedLvi, 2, QModelIndex()), Helpers::FormatDualSpeedOutput(_currentlySelectedAlgorithm->NiceHashID, _currentlySelectedAlgorithm->BenchmarkSpeed, _currentlySelectedAlgorithm->SecondaryBenchmarkSpeed), Qt::EditRole);
		}
}

void AlgorithmSettingsControl::LessThreads_Leave()
{
	int val=field_LessThreads->value();
	if (Globals::ThreadsPerCpu-val<1) {
		QMessageBox::warning(this, International::GetText("Warning_with_Exclamation"), International::GetText("Form_Main_msgbox_CPUMiningLessThreadMsg"), QMessageBox::Ok);
		}
	else {
		_currentlySelectedAlgorithm->LessThreads=val;
		}
}

void AlgorithmSettingsControl::TextChangedExtraLaunchParameters()
{
	if (!CanEdit()) {
		return;
		}
	_currentlySelectedAlgorithm->ExtraLaunchParameters=richTextBoxExtraLaunchParameters->toPlainText().replace("\r\n", " ").replace('\n', ' ').split(' ');
	_currentlySelectedAlgorithm->ExtraLaunchParameters.removeAll("");
}
