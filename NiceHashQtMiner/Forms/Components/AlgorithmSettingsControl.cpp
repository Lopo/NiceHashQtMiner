#include "Forms/Components/AlgorithmSettingsControl.h"
#include "Utils/Helpers.h"
#include "Globals.h"
#include "International.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Properties/Resources.h"
#include "Algorithms/DualAlgorithm.h"
#include "Field.h"


AlgorithmSettingsControl::AlgorithmSettingsControl(QWidget* parent)
	: QWidget(parent)
{
	InitializeComponent();

	connect(field_PowerUsage, SIGNAL(TextLeave()), this, SLOT(PowerUsage_Leave()));
	connect(fieldBoxBenchmarkSpeed, SIGNAL(TextChanged()), this, SLOT(TextChangedBenchmarkSpeed()));
	connect(secondaryFieldBoxBenchmarkSpeed, SIGNAL(TextChanged()), this, SLOT(SecondaryTextChangedBenchmarkSpeed()));
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

	field_PowerUsage=new Field<double>(flowLayoutPanel1);
	field_PowerUsage->setObjectName("field_PowerUsage");
	field_PowerUsage->setGeometry(3, 3, 220, 47);
	field_PowerUsage->LabelText("Power Usage (W):");
	field_PowerUsage->setFont(fnt);

	fieldBoxBenchmarkSpeed=new Field<double>(flowLayoutPanel1);
	fieldBoxBenchmarkSpeed->setObjectName("fieldBoxBenchmarkSpeed");
	fieldBoxBenchmarkSpeed->setGeometry(3, 56, 220, 47);
	fieldBoxBenchmarkSpeed->LabelText("Benchmark Speed (H/s):");
	fieldBoxBenchmarkSpeed->setFont(fnt);

	secondaryFieldBoxBenchmarkSpeed=new Field<double>(flowLayoutPanel1);
	secondaryFieldBoxBenchmarkSpeed->setObjectName("secondaryFieldBoxBenchmarkSpeed");
	secondaryFieldBoxBenchmarkSpeed->setGeometry(3, 109, 220, 47);
	secondaryFieldBoxBenchmarkSpeed->LabelText("Secondary Benchmark Speed (H/s):");
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
	fieldBoxBenchmarkSpeed->EntryText("0");
	secondaryFieldBoxBenchmarkSpeed->EntryText("0");
	field_PowerUsage->EntryText("0");
	richTextBoxExtraLaunchParameters->setPlainText("");
}

void AlgorithmSettingsControl::InitLocale(/*QToolTip* toolTip1*/)
{
	field_PowerUsage->InitLocale(International::GetText("Form_Settings_Algo_PowerUsage")+":", International::GetText("Form_Settings_ToolTip_PowerUsage"));
	fieldBoxBenchmarkSpeed->InitLocale(International::GetText("Form_Settings_Algo_BenchmarkSpeed")+":", International::GetText("Form_Settings_ToolTip_AlgoBenchmarkSpeed"));
	secondaryFieldBoxBenchmarkSpeed->InitLocale(International::GetText("Form_Settings_Algo_SecondaryBenchmarkSpeed")+":", International::GetText("Form_Settings_ToolTip_AlgoSecondaryBenchmarkSpeed"));
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
	return value<=0? "" : QString::number(value, 'f');
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

		groupBoxSelectedAlgorithmSettings->setTitle(International::GetText("AlgorithmsListView_GroupBox").arg(QString("%1 (%2)").arg(algorithm->AlgorithmName()).arg(algorithm->MinerBaseTypeName)));

		field_PowerUsage->EntryText(ParseDoubleDefault(algorithm->PowerUsage()));
		fieldBoxBenchmarkSpeed->EntryText(ParseDoubleDefault(algorithm->BenchmarkSpeed()));
		richTextBoxExtraLaunchParameters->setPlainText(algorithm->ExtraLaunchParameters.join(' '));
		DualAlgorithm* dualAlgo=qobject_cast<DualAlgorithm*>(algorithm);
		if (dualAlgo!=nullptr) {
			secondaryFieldBoxBenchmarkSpeed->EntryText(ParseDoubleDefault(dualAlgo->SecondaryBenchmarkSpeed()));
			secondaryFieldBoxBenchmarkSpeed->setEnabled(true);
			}
		else {
			secondaryFieldBoxBenchmarkSpeed->setEnabled(false);
			}
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
			fieldBoxBenchmarkSpeed->EntryText(ParseDoubleDefault(algorithm->BenchmarkSpeed()));
			field_PowerUsage->EntryText(ParseDoubleDefault(algorithm->PowerUsage()));
			DualAlgorithm* dualAlgo=qobject_cast<DualAlgorithm*>(algorithm);
			if (dualAlgo!=nullptr) {
				secondaryFieldBoxBenchmarkSpeed->EntryText(ParseDoubleDefault(dualAlgo->SecondaryBenchmarkSpeed()));
				}
			else {
				secondaryFieldBoxBenchmarkSpeed->EntryText("0");
				}
			}
		}
}

bool AlgorithmSettingsControl::CanEdit()
{
	return _currentlySelectedAlgorithm!=nullptr && _selected;
}

void AlgorithmSettingsControl::TextChangedBenchmarkSpeed()
{
	if (!CanEdit()) {
		return;
		}
	bool ok=false;
	double value=fieldBoxBenchmarkSpeed->EntryText().toDouble(&ok);
	if (ok) {
		_currentlySelectedAlgorithm->BenchmarkSpeed(value);
		}
	UpdateSpeedText();
}

void AlgorithmSettingsControl::SecondaryTextChangedBenchmarkSpeed()
{
	bool ok=false;
	double secondaryValue=secondaryFieldBoxBenchmarkSpeed->EntryText().toDouble(&ok);
	if (ok) {
		DualAlgorithm* dualAlgo=qobject_cast<DualAlgorithm*>(_currentlySelectedAlgorithm);
		if (dualAlgo!=nullptr) {
			dualAlgo->SecondaryBenchmarkSpeed(secondaryValue);
			}
		}
	UpdateSpeedText();
}

void AlgorithmSettingsControl::UpdateSpeedText()
{
	DualAlgorithm* dualAlgo=qobject_cast<DualAlgorithm*>(_currentlySelectedAlgorithm);
	// update lvi speed
	if (_currentlySelectedLvi!=-1 && _model!=nullptr) {
		_model->setData(_model->index(_currentlySelectedLvi, 2, QModelIndex()), Helpers::FormatDualSpeedOutput(_currentlySelectedAlgorithm->BenchmarkSpeed(), dualAlgo!=nullptr? dualAlgo->SecondaryBenchmarkSpeed() : 0, _currentlySelectedAlgorithm->NiceHashID), Qt::EditRole);
		}
}

void AlgorithmSettingsControl::PowerUsage_Leave()
{
	if (!CanEdit()) {
		return;
		}
	bool ok=false;
	double value=field_PowerUsage->EntryText().toDouble(&ok);
	if (ok) {
		_currentlySelectedAlgorithm->PowerUsage(value);
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
