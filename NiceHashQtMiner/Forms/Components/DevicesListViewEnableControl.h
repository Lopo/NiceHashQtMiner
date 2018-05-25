#ifndef FORMS_COMPONENTS_DEVICESLISTVIEWENABLECONTROL_H_
#define FORMS_COMPONENTS_DEVICESLISTVIEWENABLECONTROL_H_

#include <QtWidgets>
#include "Forms/Components/DevicesListViewEnableControlModel.h"
#include "Interfaces/IListItemCheckColorSetter.h"

class IBenchmarkCalculation;
class IListItemCheckColorSetter;
class AlgorithmsListView;


class DevicesListViewEnableControl
	: public QWidget
{
	Q_OBJECT
	Q_PROPERTY(bool IsInBenchmark READ IsInBenchmark WRITE setIsInBenchmark) // helper for benchmarking logic
	Q_PROPERTY(bool IsMining READ IsMining WRITE setIsMining)
//	Q_PROPERTY(QString FirstColumnText READ FirstColumnText WRITE setFirstColumnText)
public:
	IBenchmarkCalculation* BenchmarkCalculation=nullptr;

	inline bool IsInBenchmark() const { return _isInBenchmark; };
	void setIsInBenchmark(const bool value);
	inline bool IsMining() const { return _isMining; };
	void setIsMining(const bool value);

	bool IsBenchmarkForm=false;
	bool IsSettingsCopyEnabled=false;

//	inline QString FirstColumnText() const { return listViewDevices->headerItem()->text(ENABLED); };
//	inline void setFirstColumnText(const QString value) { listViewDevices->headerItem()->setText(ENABLED, value); };

	bool SaveToGeneralConfig=false;

	explicit DevicesListViewEnableControl(QWidget* parent=Q_NULLPTR);

	void SetIListItemCheckColorSetter(IListItemCheckColorSetter* listItemCheckColorSetter);
	void SetAlgorithmsListView(AlgorithmsListView* algorithmsListView);
	void ResetListItemColors();
	void SetComputeDevices(QList<ComputeDevice*>* computeDevices);
	void ResetComputeDevices(QList<ComputeDevice*>* computeDevices);
	void InitLocale();
	void SetDeviceSelectionChangedCallback(void (*callback)(int));
	void SetFirstSelected();

//	bool event(QEvent* event);

protected:
//	virtual void resizeEvent(QResizeEvent *event);
//	virtual void mousePressEvent(QMouseEvent* event);

private:
	QTreeView* listViewDevices;
	void InitializeComponent();
	DevicesListViewEnableControlModel* model=new DevicesListViewEnableControlModel(this);

	static const int ENABLED=0;
	static const int DEVICE=1;

	class DefaultDevicesColorSeter
		: public IListItemCheckColorSetter
	{
	private:
		QColor EnabledColor=Qt::white;
		QColor DisabledColor=Qt::darkGray;
	public:
		void LviSetColor(QAbstractTableModel* model, int row);
	};

	IListItemCheckColorSetter* _listItemCheckColorSetter=new DefaultDevicesColorSeter;

	AlgorithmsListView* _algorithmsListView=nullptr;

	// disable checkboxes when in benchmark mode
	bool _isInBenchmark=false;

	bool _isMining=false;

Q_SIGNALS:
//	void Resize();
//	void MouseClick();
	void ItemSelectionChanged(int);
//	void selectionChanged(const QItemSelection &selected);

private Q_SLOTS:
	void listViewDevicesItemChecked(int);
//	void ListViewDevices_MouseClick(QTreeWidgetItem*, int);
//	void ToolStripMenuItem_Click(bool justTuning);
//	void ToolStripMenuItemCopySettings_Click();
//	void ToolStripMenuItemCopyTuning_Click();
//	void DevicesListViewEnableControl_Resize();
	void forwardSelectionChanged(const QItemSelection&, const QItemSelection&);
};

#endif
