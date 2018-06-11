#include "Forms/Components/DevicesListViewEnableControl.h"
#include "Configs/ConfigManager.h"
#include "Forms/Components/AlgorithmsListView.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "International.h"


void DevicesListViewEnableControl::InitializeComponent()
{
	listViewDevices=new QTreeView(this);
	listViewDevices->setObjectName("listViewDevices");
	listViewDevices->setGeometry(0, 0, 374, 226);
	listViewDevices->setUniformRowHeights(true);
	listViewDevices->setRootIsDecorated(false);
	listViewDevices->setModel(model);
//	listViewDevices->setHeaderLabels({"Device", ""});
	listViewDevices->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	listViewDevices->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
	listViewDevices->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	listViewDevices->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	listViewDevices->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
	QVBoxLayout* l=new QVBoxLayout(this);
	l->addWidget(listViewDevices);
	setLayout(l);
//	connect(/*listViewDevices*/this, SIGNAL(MouseClick()), this, SLOT(listViewDevices_MouseClick()));
//	connect(listViewDevices, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(listViewDevices_MouseClick(QTreeWidgetItem*, int)));

	setObjectName("DevicesListViewEnableControl");
	resize(374, 226);
//	connect(this, SIGNAL(Resize()), this, SLOT(DevicesListViewEnableControl_Resize()));
}

void DevicesListViewEnableControl::DefaultDevicesColorSeter::LviSetColor(QAbstractTableModel* model, int row)
{
	QModelIndex mIdx=model->index(row, 0, QModelIndex());
	ComputeDevice* cdvo=model->data(mIdx, Qt::EditRole).value<ComputeDevice*>();
	model->setData(mIdx, cdvo->Enabled? EnabledColor : DisabledColor, Qt::BackgroundColorRole);
}

void DevicesListViewEnableControl::setIsInBenchmark(const bool value)
{
	_isInBenchmark=value;
	model->CheckBoxes=!value;
}

void DevicesListViewEnableControl::setIsMining(const bool value)
{
	_isMining=value;
	model->CheckBoxes=!value;
}

DevicesListViewEnableControl::DevicesListViewEnableControl(QWidget* parent)
	: QWidget(parent)
{
	InitializeComponent();

	SaveToGeneralConfig=false;
	// initialize ListView callbacks
//	connect(listViewDevices, SIGNAL(/*ItemChecked*/itemChanged(QTreeWidgetItem*, int)), this, SLOT(listViewDevicesItemChecked(QTreeWidgetItem*, int)));
//	connect(model, SIGNAL(ItemChecked(ComputeDevice*, Qt::CheckState)), this, SLOT(listViewDevicesItemChecked(ComputeDevice*, Qt::CheckState)));
	connect(model, SIGNAL(ItemChecked(int)), this, SLOT(listViewDevicesItemChecked(int)));
	setIsMining(false);
	BenchmarkCalculation=nullptr;

//	connect(listViewDevices, SIGNAL(ItemSelectionChanged()), this, SLOT(forwardItemSelectionChanged()));
//	connect(listViewDevices->selectionModel(), SIGNAL(selectionChanged()), this, SLOT(selectionChanged));
	connect(listViewDevices->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(forwardSelectionChanged(const QItemSelection&, const QItemSelection&)));
}

void DevicesListViewEnableControl::SetIListItemCheckColorSetter(IListItemCheckColorSetter* listItemCheckColorSetter)
{
	_listItemCheckColorSetter=listItemCheckColorSetter;
}

void DevicesListViewEnableControl::SetAlgorithmsListView(AlgorithmsListView* algorithmsListView)
{
	_algorithmsListView=algorithmsListView;
}

void DevicesListViewEnableControl::ResetListItemColors()
{
	if (_listItemCheckColorSetter==nullptr) {
		return;
		}
	for (int i=0; i<model->rowCount(); i++) {
		_listItemCheckColorSetter->LviSetColor(model, i);
		}
}

void DevicesListViewEnableControl::SetComputeDevices(QList<ComputeDevice*>* computeDevices)
{
	// to not run callbacks when setting new
	bool tmpSaveToGeneralConfig=SaveToGeneralConfig;
	SaveToGeneralConfig=false;
	listViewDevices->setUpdatesEnabled(false); // BeginUpdate();
	model->removeRows(0, model->rowCount(), QModelIndex());
	foreach (ComputeDevice* computeDevice, *computeDevices) {
		int rc=model->rowCount();
		model->insertRows(rc, 1, QModelIndex());
		QModelIndex idx=model->index(rc, 0, QModelIndex());
		model->setData(idx, computeDevice->Enabled? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole);
		model->setData(idx, QVariant::fromValue<ComputeDevice*>(computeDevice), Qt::EditRole);
		_listItemCheckColorSetter->LviSetColor(model, rc);
		}
	listViewDevices->setUpdatesEnabled(true);
	listViewDevices->update();
	// reset properties
	SaveToGeneralConfig=tmpSaveToGeneralConfig;
}

void DevicesListViewEnableControl::ResetComputeDevices(QList<ComputeDevice*>* computeDevices)
{
	SetComputeDevices(computeDevices);
}

void DevicesListViewEnableControl::InitLocale()
{
//	listViewDevices->header()->setWindowTitle(International::GetText("ListView_Device"));
}

void DevicesListViewEnableControl::listViewDevicesItemChecked(int row)
{
	QModelIndex idx=model->index(row, 0, QModelIndex());
	ComputeDevice* cDevice=model->data(idx, Qt::EditRole).value<ComputeDevice*>();
	if (cDevice!=nullptr) {
		cDevice->Enabled= model->data(idx, Qt::CheckStateRole)==Qt::Checked;

		if (SaveToGeneralConfig) {
			ConfigManager.GeneralConfigFileCommit();
			}
		_listItemCheckColorSetter->LviSetColor(model, row);
		if (_algorithmsListView!=nullptr) {
			_algorithmsListView->RepaintStatus(cDevice->Enabled, cDevice->Uuid());
			}
		}

	if (BenchmarkCalculation!=nullptr) {
		BenchmarkCalculation->CalcBenchmarkDevicesAlgorithmQueue();
		}
}

void DevicesListViewEnableControl::SetDeviceSelectionChangedCallback(void (*callback)(int))
{
	connect(this, SIGNAL(ItemSelectionChanged(int)), this, SLOT((*callback)(int)));
}
/*
void DevicesListViewEnableControl::ListViewDevices_MouseClick(QTreeWidgetItem* item, int column)
{
	if (IsInBenchmark()) {
		return;
		}
	if (IsMining()) {
		return;
		}
//	if (listViewDevices.se)
}

void DevicesListViewEnableControl::ToolStripMenuItem_Click(bool justTuning)
{
}

void DevicesListViewEnableControl::toolStripMenuItemCopySettings_Click()
{
//	ToolStripMenuItem_Click(false);
}

void DevicesListViewEnableControl::ToolStripMenuItemCopyTuning_Click()
{
//	ToolStripMenuItem_Click(true);
}

void DevicesListViewEnableControl::DevicesListViewEnableControl_Resize()
{
	// only one
	for (int i=0; i<=listViewDevices->columnCount(); i++) {
		listViewDevices->header()->resizeSection(i, width()-10); // @todo check
		}
}
*/
void DevicesListViewEnableControl::SetFirstSelected()
{
	if (model->rowCount()>0) {
		listViewDevices->selectionModel()->clear();
		listViewDevices->selectionModel()->select(model->index(0, 0, QModelIndex()), QItemSelectionModel::Select /*ClearAndSelect*/);
		}
}
/*
void DevicesListViewEnableControl::mousePressEvent(QMouseEvent* event)
{
	QWidget::mousePressEvent(event);
}

void DevicesListViewEnableControl::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);
	DevicesListViewEnableControl_Resize();
}

bool DevicesListViewEnableControl::event(QEvent* event)
{
	if (event->type()==QEvent::Resize) {
		Q_EMIT Resize();
		}
	if (event->type()==QEvent::ContextMenu) {
		Q_EMIT MouseClick();
		}
//	auto t=event->type();
//printf("%s\n", QMetaEnum::fromType<QEvent::Type>().valueToKey(t));
	return QWidget::event(event);
}
*/
void DevicesListViewEnableControl::forwardSelectionChanged(const QItemSelection&/* newSel*/, const QItemSelection&/* oldSel*/)
{
	const QModelIndex index=listViewDevices->selectionModel()->currentIndex();
	Q_EMIT ItemSelectionChanged(index.isValid()? index.row() : -1);
}
