#ifndef FORMS_COMPONENTS_ALGORITHMSLISTVIEW_H_
#define FORMS_COMPONENTS_ALGORITHMSLISTVIEW_H_

#include <QtWidgets>
#include "Forms/Components/AlgorithmsListViewModel.h"
#include "Interfaces/IBenchmarkCalculation.h"
#include "Interfaces/IListItemCheckColorSetter.h"

class ComputeDevice;


class AlgorithmsListView
	: public QWidget
{
	Q_OBJECT
public:
	explicit AlgorithmsListView(QWidget* parent=Q_NULLPTR);

	class IAlgorithmListView {
	public:
		virtual void SetCurrentlySelected(AlgorithmsListViewModel* model, int row, ComputeDevice* computeDevice)=0;
		virtual void HandleCheck(AlgorithmsListViewModel* model, int row)=0;
		virtual void ChangeSpeed(AlgorithmsListViewModel* model, int row)=0;
		};

	IAlgorithmListView* ComunicationInterface=nullptr;

	IBenchmarkCalculation* BenchmarkCalculation=nullptr;

	ComputeDevice* _computeDevice=nullptr;

	void InitLocale();

	void SetAlgorithms(ComputeDevice* computeDevice, bool isEnabled);
	void RepaintStatus(bool isEnabled, const QString uuid);

	void ResetListItemColors();
	void SetSpeedStatus(ComputeDevice* computeDevice, Algorithm* algorithm, QString status); // benchmark settings

	inline bool IsInBenchmark() const { return _isInBenchmark; };
	void IsInBenchmark(const bool value);

private:
	QTableView* listViewAlgorithms=nullptr;
	void InitializeComponent();

	AlgorithmsListViewModel* model=new AlgorithmsListViewModel(this);

	static const int ENABLED=0;
	static const int ALGORITHM=1;
	static const int SPEED=2;
	static const int SECSPEED=3;
	static const int RATIO=4;
	static const int RATE=5;

	class DefaultAlgorithmColorSeter
		: public IListItemCheckColorSetter
	{
	private:
		static QColor DisabledColor;
		static QColor BenchmarkedColor;
		static QColor UnbenchmarkedColor;
	public:
		void LviSetColor(QAbstractTableModel* model, int row);
		};

	IListItemCheckColorSetter* _listItemCheckColorSetter=new DefaultAlgorithmColorSeter;

	// disable checkboxes when in benchmark mode
	bool _isInBenchmark=false;

Q_SIGNALS:
	void itemSelectionChanged(int);

private Q_SLOTS:
	void ListViewAlgorithms_ItemSelectionChanged(int row);
	void ListViewAlgorithms_ItemChecked(int row);
	void ListViewAlgorithms_MouseClick(const QPoint&);
	void ToolStripMenuItemEnableAll_Click();
	void ToolStripMenuItemDisableAll_Click();
	void ToolStripMenuItemClear_Click();
	void ToolStripMenuItemTest_Click();
	void toolStripMenuItemOpenDcri_Click();
	void ToolStripMenuItemEnableBenched_Click();
	void toolStripMenuItemTuningEnabled_Checked(bool checked);

	void forwardSelectionChanged(const QItemSelection&, const QItemSelection&);
};

#endif
