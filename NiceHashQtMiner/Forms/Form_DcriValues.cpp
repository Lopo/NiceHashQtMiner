#include "Forms/Form_DcriValues.h"
#include "Properties/Resources.h"
#include "Algorithms/DualAlgorithm.h"
#include "Stats/NiceHashStats.h"
#include "International.h"
#include "Configs/ConfigManager.h"
#include "Components/Field.h"


void FormDcriValues::InitializeComponent()
{
	Resources resources;

	listView_Intensities=new QTableView(this);
	listView_Intensities->setObjectName("listView_Intensities");
	listView_Intensities->setGeometry(8, 27, 388, 367);
	listView_Intensities->setModel(model);
	listView_Intensities->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	listView_Intensities->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	listView_Intensities->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	listView_Intensities->verticalHeader()->hide();
	listView_Intensities->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	listView_Intensities->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	listView_Intensities->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);

	connect(listView_Intensities->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(ListView_Intensities_ItemSelectionChanged(const QItemSelection&, const QItemSelection&)));
	listView_Intensities->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(listView_Intensities, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ListView_Intensities_MouseClick(const QPoint&)));

	button_Close=new QPushButton(this);
	button_Close->setObjectName("button_Close");
	button_Close->setGeometry(486, 370, 134, 23);
	button_Close->setText("Close");
	connect(button_Close, SIGNAL(clicked()), this, SLOT(Button_Close_Clicked()));

	button_Save=new QPushButton(this);
	button_Save->setObjectName("button_Save");
	button_Save->setGeometry(486, 344, 134, 23);
	button_Save->setText("Save");
	connect(button_Save, SIGNAL(clicked()), this, SLOT(Button_Save_Clicked()));

	checkBox_TuningEnabled=new QCheckBox(this);
	checkBox_TuningEnabled->setObjectName("checkBox_TuningEnabled");
	checkBox_TuningEnabled->setGeometry(8, 8, 136, 27);
	checkBox_TuningEnabled->setText("Dcri Tuning Enabled");
	connect(checkBox_TuningEnabled, SIGNAL(stateChanged(int)), this, SLOT(CheckBox_TuningEnabledCheckedChanged()));

	pictureBox_TuningEnabled=new QLabel(this);
	pictureBox_TuningEnabled->setObjectName("pictureBox_TuningEnabled");
	pictureBox_TuningEnabled->setGeometry(131, 8, 18, 18);
	pictureBox_TuningEnabled->setPixmap(resources.info_black_18);

	field_Power=new Field<int>(this);
	field_Power->setObjectName("field_Power");
	field_Power->setGeometry(400, 287, 220, 47);
	field_Power->LabelText("Power Usage (W)");
	field_Power->setEnabled(false);

	field_TuningEnd=new Field<int>(this);
	field_TuningEnd->setObjectName("field_TuningEnd");
	field_TuningEnd->setGeometry(400, 179, 220, 47);
	field_TuningEnd->LabelText("Tuning End");

	field_TuningInterval=new Field<int>(this);
	field_TuningInterval->setObjectName("field_TuningInterval");
	field_TuningInterval->setGeometry(400, 230, 220, 47);
	field_TuningInterval->LabelText("Tuning Interval");

	field_TuningStart=new Field<int>(this);
	field_TuningStart->setObjectName("field_TuningStart");
	field_TuningStart->setGeometry(400, 129, 220, 47);
	field_TuningStart->LabelText("Tuning Start");

	field_SecondarySpeed=new Field<double>(this);
	field_SecondarySpeed->setObjectName("field_SecondarySpeed");
	field_SecondarySpeed->setGeometry(400, 78, 220, 47);
	field_SecondarySpeed->LabelText("Secondary Speed (H/s)");

	field_Speed=new Field<double>(this);
	field_Speed->setObjectName("field_Speed");
	field_Speed->setGeometry(400, 27, 224, 47);
	field_Speed->LabelText("Speed (H/s)");

	setTabOrder(listView_Intensities, button_Close);
	setTabOrder(button_Close, button_Save);
	setTabOrder(button_Save, checkBox_TuningEnabled);
	setTabOrder(checkBox_TuningEnabled, field_Speed);
	setTabOrder(field_Speed, field_SecondarySpeed);
	setTabOrder(field_SecondarySpeed, field_TuningStart);
	setTabOrder(field_TuningStart, field_TuningInterval);
	setTabOrder(field_TuningInterval, field_TuningEnd);
	setTabOrder(field_TuningEnd, field_Power);

	setObjectName("FormDcriValues");
	resize(632, 411);
	setMinimumSize(609, 380);
	setWindowTitle("Dcri Values");
	setWindowFlag(Qt::WindowType::Dialog);
}

FormDcriValues::FormDcriValues(DualAlgorithm* algorithm, QWidget* parent)
{
	InitializeComponent();
	_algorithm=algorithm;
	algorithm->MakeIntensityBackup();

	InitLocale();
	SetIntensities();
	InitializeControls();
	connect(&NiceHashStats, SIGNAL(SmaUpdate()), this, SLOT(UpdateProfits()));

	_isInitFinished=true;
}

void FormDcriValues::InitLocale()
{
	model->setHeaderData(Intensity, Qt::Horizontal, International::GetText("Form_DcriValues_Intensity"));
	model->setHeaderData(Speed, Qt::Horizontal, International::GetText("AlgorithmsListView_Speed"));
	model->setHeaderData(SecondarySpeed, Qt::Horizontal, International::GetText("Form_DcriValues_SecondarySpeed"));
	model->setHeaderData(Profit, Qt::Horizontal, International::GetText("AlgorithmsListView_Rate"));
	model->setHeaderData(Power, Qt::Horizontal, International::GetText("Form_DcriValues_Power"));
	setWindowTitle(International::GetText("Form_DcriValues_Title"));
	button_Close->setText(International::GetText("Form_Settings_buttonCloseNoSaveText"));
	button_Save->setText(International::GetText("Form_Settings_buttonSaveText"));
	checkBox_TuningEnabled->setText(International::GetText("Form_DcriValues_TuningEnabled"));

	field_Speed->InitLocale(International::GetText("Form_Settings_Algo_BenchmarkSpeed")+":", International::GetText("Form_Settings_ToolTip_AlgoBenchmarkSpeed"));
	field_SecondarySpeed->InitLocale(International::GetText("Form_Settings_Algo_SecondaryBenchmarkSpeed")+":", International::GetText("Form_Settings_ToolTip_AlgoSecondaryBenchmarkSpeed"));
	field_TuningStart->InitLocale(International::GetText("Form_DcriValues_TuningStart")+":", International::GetText("Form_DcriValues_ToolTip_TuningStart"));
	field_TuningEnd->InitLocale(International::GetText("Form_DcriValues_TuningEnd")+":", International::GetText("Form_DcriValues_ToolTip_TuningEnd"));
	field_TuningInterval->InitLocale(International::GetText("Form_DcriValues_TuningInterval")+":", International::GetText("Form_DcriValues_ToolTip_TuningInterval"));
	field_Power->InitLocale(International::GetText("Form_Settings_Algo_PowerUsage")+":", International::GetText("Form_Settings_ToolTip_PowerUsage"));

	checkBox_TuningEnabled->setToolTip(International::GetText("Form_DcriValues_ToolTip_TuningEnabled"));
	pictureBox_TuningEnabled->setToolTip(International::GetText("Form_DcriValues_ToolTip_TuningEnabled"));
}

void FormDcriValues::SetIntensities()
{
	listView_Intensities->setUpdatesEnabled(false);
	foreach (int intensity, _algorithm->AllIntensities()) {
		int rc=model->rowCount();
		model->insertRows(rc, 1, QModelIndex());
		QModelIndex idx=model->index(rc, 0, QModelIndex());
		model->setData(model->index(rc, Intensity, QModelIndex()), QString::number(intensity), Qt::DisplayRole);
		model->setData(model->index(rc, Speed, QModelIndex()), _algorithm->SpeedStringForIntensity(intensity), Qt::DisplayRole);
		model->setData(model->index(rc, SecondarySpeed, QModelIndex()), _algorithm->SecondarySpeedStringForIntensity(intensity), Qt::DisplayRole);
		model->setData(model->index(rc, Profit, QModelIndex()), QString::number(_algorithm->ProfitForIntensity(intensity), 'f', 8), Qt::DisplayRole);
		model->setData(model->index(rc, Power, QModelIndex()), QString::number(_algorithm->IntensityPowers.value(intensity, 0), 'f', 2), Qt::DisplayRole);
		model->setData(idx, QVariant::fromValue<int>(intensity), Qt::EditRole);
		}
	listView_Intensities->setUpdatesEnabled(true);
}


void FormDcriValues::InitializeControls()
{
	checkBox_TuningEnabled->setCheckState(_algorithm->TuningEnabled? Qt::Checked : Qt::Unchecked);

	UpdateEnabled();

	field_TuningStart->EntryText(QString::number(_algorithm->TuningStart));
	field_TuningEnd->EntryText(QString::number(_algorithm->TuningEnd));
	field_TuningInterval->EntryText(QString::number(_algorithm->TuningInterval));

	connect(field_Speed, SIGNAL(TextChanged()), this, SLOT(TextChangedSpeed()));
	connect(field_SecondarySpeed, SIGNAL(TextChanged()), this, SLOT(TextChangedSecondarySpeed()));
	connect(field_TuningStart, SIGNAL(TextChanged()), this, SLOT(TextChangedTuningStart()));
	connect(field_TuningEnd, SIGNAL(TextChanged()), this, SLOT(TextChangedTuningEnd()));
	connect(field_TuningInterval, SIGNAL(TextChanged()), this, SLOT(TextChangedTuningInterval()));
	connect(field_Power, SIGNAL(TextChanged()), this, SLOT(TextChangedPower()));
}

void FormDcriValues::UpdateEnabled()
{
	listView_Intensities->setEnabled(_algorithm->TuningEnabled);
	field_TuningStart->setEnabled(_algorithm->TuningEnabled);
	field_TuningEnd->setEnabled(_algorithm->TuningEnd);
	field_TuningInterval->setEnabled(_algorithm->TuningInterval);
	if (!_algorithm->TuningEnabled) {
		field_Speed->setEnabled(false);
		field_SecondarySpeed->setEnabled(false);
		field_Power->setEnabled(false);
		}
}

void FormDcriValues::UpdateIntensities()
{
	for (int i=0; i<model->rowCount(); i++) {
		int intensity=model->data(model->index(i, 0, QModelIndex()), Qt::EditRole).value<int>();
		model->setData(model->index(i, Speed, QModelIndex()), _algorithm->SpeedStringForIntensity(intensity), Qt::DisplayRole);
		model->setData(model->index(i, SecondarySpeed, QModelIndex()), _algorithm->SecondarySpeedStringForIntensity(intensity), Qt::DisplayRole);
		model->setData(model->index(i, Profit, QModelIndex()), QString::number(_algorithm->ProfitForIntensity(intensity), 'f', 8), Qt::DisplayRole);
		model->setData(model->index(i, Power, QModelIndex()), QString::number(_algorithm->IntensityPowers.value(intensity, 0), 'f', 2), Qt::DisplayRole);
		}
}

void FormDcriValues::UpdateIntensityList()
{
	SetIntensities();
}

void FormDcriValues::UpdateProfits()
{
	for (int i=0; i<model->rowCount(); i++) {
		int intensity=model->data(model->index(i, 0, QModelIndex()), Qt::EditRole).value<int>();
		model->setData(model->index(i, Profit, QModelIndex()), QString::number(_algorithm->ProfitForIntensity(intensity), 'f', 8), Qt::DisplayRole);
		}
}

void FormDcriValues::ToolStripMenuItemClear_Click()
{
	foreach (QModelIndex lvi, listView_Intensities->selectionModel()->selectedRows()) {
		int intensity=model->data(lvi, Qt::EditRole).value<int>();
		IsChange(true);
		_algorithm->IntensitySpeeds[intensity]=0;
		_algorithm->SecondaryIntensitySpeeds[intensity]=0;
		UpdateIntensities();
		}
}

//void FormDcriValues::Form_DcriValues_FormClosing()
void FormDcriValues::closeEvent(QCloseEvent* e)
{
	if (IsChange() && !_isChangeSaved) {
		QMessageBox::StandardButton result=QMessageBox::warning(this, International::GetText("Form_Settings_buttonCloseNoSaveTitle"), International::GetText("Form_Settings_buttonCloseNoSaveMsg"), QMessageBox::Yes|QMessageBox::No);
		if (result==QMessageBox::No) {
			e->ignore();
			return;
			}
		}

	if (_isChangeSaved) {
		_algorithm->IntensityUpToDate=false;
		ConfigManager.CommitBenchmarks();
		}
	else {
		_algorithm->RestoreIntensityBackup();
		}
}

void FormDcriValues::ListView_Intensities_ItemSelectionChanged(const QItemSelection&, const QItemSelection&)
{
	const QModelIndex index=listView_Intensities->selectionModel()->currentIndex();
	int row=index.isValid()? index.row() : -1;
	int intensity=model->data(model->index(row, 0, QModelIndex()), Qt::EditRole).value<int>();
	_currentlySelectedIntensity=intensity;
	_isInitFinished=false;
	field_Speed->EntryText(QString::number(_algorithm->SpeedForIntensity(intensity)));
	field_SecondarySpeed->EntryText(QString::number(_algorithm->SecondarySpeedForIntensity(intensity)));
	field_Power->EntryText(QString::number(_algorithm->IntensityPowers.value(intensity, 0)));
	_isInitFinished=true;

	field_Speed->setEnabled(true);
	field_SecondarySpeed->setEnabled(true);
	field_Power->setEnabled(true);
}

void FormDcriValues::Button_Close_Clicked()
{
	_isChangeSaved=false;
	close();
}

void FormDcriValues::Button_Save_Clicked()
{
	_isChangeSaved=true;
	close();
}

void FormDcriValues::CheckBox_TuningEnabledCheckedChanged()
{
	IsChange(true);

	_algorithm->TuningEnabled=checkBox_TuningEnabled->isChecked();
	UpdateEnabled();
}

void FormDcriValues::TextChangedSpeed()
{
	bool ok=false;
	double value=field_Speed->EntryText().toDouble(&ok);
	if (ok) {
		IsChange(true);
		_algorithm->IntensitySpeeds.insert(_currentlySelectedIntensity, value);
		}
	UpdateIntensities();
}

void FormDcriValues::TextChangedSecondarySpeed()
{
	bool ok=false;
	double value=field_SecondarySpeed->EntryText().toDouble(&ok);
	if (ok) {
		IsChange(true);
		_algorithm->SecondaryIntensitySpeeds.insert(_currentlySelectedIntensity, value);
		}
	UpdateIntensities();
}

void FormDcriValues::TextChangedPower()
{
	bool ok=false;
	double value=field_Power->EntryText().toDouble(&ok);
	if (ok) {
		IsChange(true);
		_algorithm->IntensityPowers.insert(_currentlySelectedIntensity, value);
		}
	UpdateIntensities();
}

void FormDcriValues::TextChangedTuningStart()
{
	bool ok=false;
	int value=field_TuningStart->EntryText().toInt(&ok);
	if (ok) {
		IsChange(true);
		_algorithm->TuningStart=value;
		}
	UpdateIntensityList();
}

void FormDcriValues::TextChangedTuningEnd()
{
	bool ok=false;
	int value=field_TuningEnd->EntryText().toInt(&ok);
	if (ok) {
		IsChange(true);
		_algorithm->TuningEnd=value;
		}
	UpdateIntensityList();
}

void FormDcriValues::TextChangedTuningInterval()
{
	bool ok=false;
	int value=field_TuningInterval->EntryText().toInt(&ok);
	if (ok) {
		IsChange(true);
		_algorithm->TuningInterval=value;
		}
	UpdateIntensityList();
}

void FormDcriValues::ListView_Intensities_MouseClick(const QPoint& pos)
{
	QMenu ctx(this);

	QAction* clearItem=ctx.addAction(International::GetText("AlgorithmsListView_ContextMenu_ClearItem"));
	connect(clearItem, SIGNAL(triggered()), this, SLOT(ToolStripMenuItemClear_Click()));

	ctx.exec(mapToGlobal(pos));
}
