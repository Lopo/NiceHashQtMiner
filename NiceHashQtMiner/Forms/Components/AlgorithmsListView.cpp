#include "Forms/Components/AlgorithmsListView.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "International.h"


QColor AlgorithmsListView::DefaultAlgorithmColorSeter::DisabledColor=Qt::darkGray;
QColor AlgorithmsListView::DefaultAlgorithmColorSeter::BenchmarkedColor=QColor("#90EE90"); // lightGreen
QColor AlgorithmsListView::DefaultAlgorithmColorSeter::UnbenchmarkedColor=QColor("#ADD8E6"); // lightBlue


AlgorithmsListView::AlgorithmsListView(QWidget* parent)
	: QWidget(parent)
{
	InitializeComponent();
	// callback initializations
	connect(this, SIGNAL(itemSelectionChanged(int)), this, SLOT(ListViewAlgorithms_ItemSelectionChanged(int)));
	connect(model, SIGNAL(ItemChecked(int)), this, SLOT(ListViewAlgorithms_ItemChecked(int)));
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	setIsInBenchmark(false);
}

void AlgorithmsListView::InitializeComponent()
{
	setObjectName("AlgorithmsListView");

	QFont f6;
	f6.setPointSize(6);

	listViewAlgorithms=new QTableView(this);
	listViewAlgorithms->setObjectName("listViewAlgorithms");
//	listViewAlgorithms->setGeometry(0, 0, 539, 380);
	listViewAlgorithms->setGeometry(0, 0, 539, 150);
	model->CheckBoxes=true;
	listViewAlgorithms->setModel(model);
	listViewAlgorithms->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	listViewAlgorithms->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
//	connect(listViewAlgorithms, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(listViewAlgorithms_MouseClick()));

	listViewAlgorithms->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	listViewAlgorithms->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	listViewAlgorithms->verticalHeader()->hide();
	listViewAlgorithms->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	listViewAlgorithms->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	listViewAlgorithms->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
//	connect(listViewAlgorithms, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(listViewAlgorithms_MouseClick(const QPoint&)));
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ListViewAlgorithms_MouseClick(const QPoint&)));

	connect(listViewAlgorithms->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(forwardSelectionChanged(const QItemSelection&, const QItemSelection&)));
	listViewAlgorithms->setFont(f6);

	resize(539, 380);
	setFont(f6);
}

void AlgorithmsListView::DefaultAlgorithmColorSeter::LviSetColor(QAbstractTableModel* model, int row)
{
	QModelIndex idx=model->index(row, 0, QModelIndex());
	Algorithm* algorithm=model->data(idx, Qt::EditRole).value<Algorithm*>();
	if (!algorithm->Enabled && !algorithm->IsBenchmarkPending()) {
		model->setData(idx, DisabledColor, Qt::BackgroundColorRole);
		}
	else if (algorithm->BenchmarkSpeed>0&& !algorithm->IsBenchmarkPending()) {
		model->setData(idx, BenchmarkedColor, Qt::BackgroundColorRole);
		}
	else {
		model->setData(idx, UnbenchmarkedColor, Qt::BackgroundColorRole);
		}
}

void AlgorithmsListView::setIsInBenchmark(const bool value)
{
	IsInBenchmark_=value;
	model->CheckBoxes=!value;
}

void AlgorithmsListView::InitLocale()
{
	model->setHeaderData(ENABLED, Qt::Horizontal, International::GetText("AlgorithmsListView_Enabled"));
	model->setHeaderData(ALGORITHM, Qt::Horizontal, International::GetText("AlgorithmsListView_Algorithm"));
	model->setHeaderData(SPEED, Qt::Horizontal, International::GetText("AlgorithmsListView_Speed"));
	model->setHeaderData(RATIO, Qt::Horizontal, International::GetText("AlgorithmsListView_Ratio"));
	model->setHeaderData(RATE, Qt::Horizontal, International::GetText("AlgorithmsListView_Rate"));
}

void AlgorithmsListView::SetAlgorithms(ComputeDevice* computeDevice, bool isEnabled)
{
	_computeDevice=computeDevice;
	listViewAlgorithms->setUpdatesEnabled(false);
	model->removeRows(0, model->rowCount(), QModelIndex());
	foreach (Algorithm* alg, *computeDevice->GetAlgorithmSettings()) {
/*		QTreeWidgetItem* lvi=new QTreeWidgetItem(QStringList("") << QString("%1 (%2)").arg(alg->AlgorithmName, alg->MinerBaseTypeName) << alg->BenchmarkSpeedString() << alg->CurPayingRatio() << alg->CurPayingRate());
		lvi->setTextAlignment(SPEED, Qt::AlignRight|Qt::AlignVCenter);*/
		int rc=model->rowCount();
		model->insertRows(rc, 1, QModelIndex());
		QModelIndex idx=model->index(rc, 0, QModelIndex());
		model->setData(model->index(rc, ALGORITHM, QModelIndex()), QString("%1 (%2)").arg(alg->AlgorithmName, alg->MinerBaseTypeName), Qt::EditRole); // DisplayRole?
		model->setData(model->index(rc, SPEED, QModelIndex()), alg->BenchmarkSpeedString(), Qt::EditRole); // DisplayRole?
		model->setData(model->index(rc, RATIO, QModelIndex()), alg->CurPayingRatio(), Qt::EditRole); // DisplayRole?
		model->setData(model->index(rc, RATE, QModelIndex()), alg->CurPayingRate(), Qt::EditRole); // DisplayRole?
		model->setData(idx, QVariant::fromValue<Algorithm*>(alg), Qt::EditRole);
		model->setData(idx, alg->Enabled? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole);
		_listItemCheckColorSetter->LviSetColor(model, rc);
		}
	listViewAlgorithms->setUpdatesEnabled(true);
	setEnabled(isEnabled);
}

void AlgorithmsListView::RepaintStatus(bool isEnabled, const QString uuid)
{
	if (_computeDevice!=nullptr && _computeDevice->Uuid()==uuid) {
		for (int i=0; i<model->rowCount(); i++) {
			model->setData(model->index(i, SPEED, QModelIndex()), model->data(model->index(i, 0, QModelIndex()), Qt::EditRole).value<Algorithm*>()->BenchmarkSpeedString(), Qt::EditRole);
			_listItemCheckColorSetter->LviSetColor(model, i);
			}
		setEnabled(isEnabled);
		}
}

void AlgorithmsListView::ListViewAlgorithms_ItemSelectionChanged(int row)
{
	if (ComunicationInterface!=nullptr) {
		ComunicationInterface->SetCurrentlySelected(model, row, _computeDevice);
		}
}

void AlgorithmsListView::ListViewAlgorithms_ItemChecked(int row)
{
	QModelIndex idx=model->index(row, 0, QModelIndex());
	if (IsInBenchmark_) {
		model->setData(idx, model->data(idx, Qt::CheckStateRole)==Qt::Checked? Qt::Unchecked : Qt::Checked);
		return;
		}
	Algorithm* algo=model->data(idx, Qt::EditRole).value<Algorithm*>();
	if (algo!=nullptr) {
		algo->Enabled= model->data(idx, Qt::CheckStateRole)==Qt::Checked;
		}
	if (ComunicationInterface!=nullptr) {
		ComunicationInterface->HandleCheck(model, row);
		}
	_listItemCheckColorSetter->LviSetColor(model, row);
	// update benchmark status data
	if (BenchmarkCalculation!=nullptr) {
		BenchmarkCalculation->CalcBenchmarkDevicesAlgorithmQueue();
		}
}

void AlgorithmsListView::ResetListItemColors()
{
	if (_listItemCheckColorSetter==nullptr) {
		return;
		}
	for (int i=0; i<model->rowCount(); i++) {
		_listItemCheckColorSetter->LviSetColor(model, i);
		}
}

void AlgorithmsListView::SetSpeedStatus(ComputeDevice* computeDevice, Algorithm* algorithm, QString status)
{
	if (algorithm!=nullptr) {
		algorithm->BenchmarkStatus=status;
		// gui update only if same as selected
		if (_computeDevice!=nullptr && computeDevice->Uuid()==_computeDevice->Uuid()) {
			for (int i=0; i<model->rowCount(); i++) {
				Algorithm* algo=model->data(model->index(i, 0, QModelIndex()), Qt::EditRole).value<Algorithm*>();
				if (algo!=nullptr && algo->AlgorithmStringID==algorithm->AlgorithmStringID) {
					// @todo handle numbers
					model->setData(model->index(i, SPEED, QModelIndex()), algorithm->BenchmarkSpeedString(), Qt::EditRole);
					model->setData(model->index(i, RATE, QModelIndex()), algorithm->CurPayingRate(), Qt::EditRole);
					model->setData(model->index(i, RATIO, QModelIndex()), algorithm->CurPayingRatio(), Qt::EditRole);
					_listItemCheckColorSetter->LviSetColor(model, i);
					break;
					}
				}
			}
		}
}

void AlgorithmsListView::ListViewAlgorithms_MouseClick(const QPoint& pos)
{
	if (IsInBenchmark_) {
		return;
		}
	QMenu ctx(this);

	QAction disableAllItems(International::GetText("AlgorithmsListView_ContextMenu_DisableAll"), this); // disable all
	connect(&disableAllItems, SIGNAL(triggered()), this, SLOT(ToolStripMenuItemDisableAll_Click()));
	QAction enableAllItems(International::GetText("AlgorithmsListView_ContextMenu_EnableAll"), this); // enable all
	connect(&enableAllItems, SIGNAL(triggered()), this, SLOT(ToolStripMenuItemEnableAll_Click()));
	QAction testItem(International::GetText("AlgorithmsListView_ContextMenu_TestItem"), this); // test this
	connect(&testItem, SIGNAL(triggered()), this, SLOT(ToolStripMenuItemTest_Click()));
	QAction enableBenchedItem(International::GetText("AlgorithmsListView_ContextMenu_EnableBenched"), this); // enable benchmarked only
	connect(&enableBenchedItem, SIGNAL(triggered()), this, SLOT(ToolStripMenuItemEnableBenched_Click()));
	QAction clearItem(International::GetText("AlgorithmsListView_ContextMenu_ClearItem"), this); // clear item
	connect(&clearItem, SIGNAL(triggered()), this, SLOT(ToolStripMenuItemClear_Click()));

	ctx.addActions({&disableAllItems, &enableAllItems, &testItem, &enableBenchedItem, &clearItem});
	ctx.exec(mapToGlobal(pos));
}

void AlgorithmsListView::ToolStripMenuItemEnableAll_Click()
{
	for (int i=0; i<model->rowCount(); i++) {
		model->setData(model->index(i, 0, QModelIndex()), Qt::Checked, Qt::CheckStateRole);
		}
}

void AlgorithmsListView::ToolStripMenuItemDisableAll_Click()
{
	for (int i=0; i<model->rowCount(); i++) {
		model->setData(model->index(i, 0, QModelIndex()), Qt::Unchecked, Qt::CheckStateRole);
		}
}

void AlgorithmsListView::ToolStripMenuItemClear_Click()
{
	if (_computeDevice!=nullptr) {
		for (int i=0; i<model->rowCount(); i++) {
			const QModelIndex index=listViewAlgorithms->selectionModel()->currentIndex();
			if (index.isValid()) {
				Algorithm* algorithm=model->data(model->index(index.row(), 0, QModelIndex()), Qt::EditRole).value<Algorithm*>();
				if (algorithm!=nullptr) {
					algorithm->BenchmarkSpeed=0;
					algorithm->SecondaryBenchmarkSpeed=0;
					RepaintStatus(_computeDevice->Enabled, _computeDevice->Uuid());
					if (BenchmarkCalculation!=nullptr) { // update benchmark status data
						BenchmarkCalculation->CalcBenchmarkDevicesAlgorithmQueue();
						}
					if (ComunicationInterface!=nullptr) { // update settings
						ComunicationInterface->ChangeSpeed(model, index.row());
						}
					}
				}
			}
		}
}

void AlgorithmsListView::ToolStripMenuItemTest_Click()
{
	if (_computeDevice!=nullptr) {
		for (int i=0; i<model->rowCount(); i++) {
			QModelIndex idx=model->index(i, 0, QModelIndex());
			Algorithm* algorithm=model->data(idx, Qt::EditRole).value<Algorithm*>();
			if (algorithm!=nullptr) {
				bool selected=listViewAlgorithms->selectionModel()->selectedRows().contains(idx);
				model->setData(idx, selected? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole);
				if (selected && algorithm->BenchmarkSpeed<=0) { // If it has zero speed, set to 1 so it can be tested
					algorithm->BenchmarkSpeed=1;
					RepaintStatus(_computeDevice->Enabled, _computeDevice->Uuid());
					if (ComunicationInterface!=nullptr) {
						ComunicationInterface->ChangeSpeed(model, i);
						}
					}
				}
			}
		}
}

void AlgorithmsListView::ToolStripMenuItemEnableBenched_Click()
{
	for (int i=0; i<model->rowCount(); i++) {
		QModelIndex idx=model->index(i, 0, QModelIndex());
		if (model->data(idx, Qt::EditRole).value<Algorithm*>()->BenchmarkSpeed>0) {
			model->setData(idx, Qt::Checked, Qt::CheckStateRole);
			}
		}
}

void AlgorithmsListView::forwardSelectionChanged(const QItemSelection&, const QItemSelection&)
{
	const QModelIndex index=listViewAlgorithms->selectionModel()->currentIndex();
	emit itemSelectionChanged(index.isValid()? index.row() : -1);
}
