#ifndef FORMS_COMPONENTS_ALGORITHMSETTINGSCONTROL_H_
#define FORMS_COMPONENTS_ALGORITHMSETTINGSCONTROL_H_

#include "Forms/Components/AlgorithmsListView.h"
#include "Forms/Components/Field.h"
#include <QtWidgets>


class AlgorithmSettingsControl
	: public QWidget,
		public AlgorithmsListView::IAlgorithmListView
{
	Q_OBJECT
public:
	AlgorithmSettingsControl(QWidget* parent=Q_NULLPTR);

	void Deselect();
	void InitLocale(/*QToolTip* toolTip1*/);

	void SetCurrentlySelected(AlgorithmsListViewModel* model, int row, ComputeDevice* computeDevice);
	void HandleCheck(AlgorithmsListViewModel* model, int row);
	void ChangeSpeed(AlgorithmsListViewModel* model, int row);

private:
	QGroupBox* groupBoxSelectedAlgorithmSettings=nullptr;
	QGroupBox* groupBoxExtraLaunchParameters=nullptr;
	QPlainTextEdit* richTextBoxExtraLaunchParameters=nullptr;
	QWidget* flowLayoutPanel1=nullptr;
	Field<double>* fieldBoxBenchmarkSpeed=nullptr;
	Field<double>* secondaryFieldBoxBenchmarkSpeed=nullptr;
	Field<double>* field_PowerUsage=nullptr;
//	QDoubleSpinBox* secondaryFieldBoxBenchmarkSpeed=nullptr;
//	QLabel* labelFieldIndicator_secondaryFieldBoxBenchmarkSpeed=nullptr;
//	QDoubleSpinBox* fieldBoxBenchmarkSpeed=nullptr;
//	QLabel* labelFieldIndicator_fieldBoxBenchmarkSpeed=nullptr;
//	QDoubleSpinBox* field_PowerUsage=nullptr;
//	QLabel* labelFieldIndicator_PowerUsage=nullptr;
	QLabel* pictureBox1=nullptr;
	void InitializeComponent();

	ComputeDevice* _computeDevice=nullptr;
	Algorithm* _currentlySelectedAlgorithm=nullptr;
	int _currentlySelectedLvi=-1;
	// winform crappy event workaround
	bool _selected=false;

	QString ParseStringDefault(QString value);
	QString ParseDoubleDefault(double value);

	bool CanEdit();

	void UpdateSpeedText();

	AlgorithmsListViewModel* _model=nullptr;

private Q_SLOTS:
	void TextChangedBenchmarkSpeed();
	void SecondaryTextChangedBenchmarkSpeed();
	void PowerUsage_Leave();
	void TextChangedExtraLaunchParameters();
};

#endif
