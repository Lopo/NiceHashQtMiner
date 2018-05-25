#ifndef FORMS_FORM_DCRIVALUES_H_
#define FORMS_FORM_DCRIVALUES_H_

#include "Forms/Components/Field.h"
#include "Components/IntensitiesModel.h"
#include <QtWidgets>
class DualAlgorithm;


class FormDcriValues
	: public QDialog
{
	Q_OBJECT
public:
	FormDcriValues(DualAlgorithm* algorithm, QWidget* parent=Q_NULLPTR);

private:
	QTableView* listView_Intensities=nullptr;
	QPushButton* button_Close=nullptr;
	QPushButton* button_Save=nullptr;
	QCheckBox* checkBox_TuningEnabled=nullptr;
	Field<double>* field_Speed=nullptr;
	Field<double>* field_SecondarySpeed=nullptr;
	Field<int>* field_TuningStart=nullptr;
	Field<int>* field_TuningInterval=nullptr;
	Field<int>* field_TuningEnd=nullptr;
//	contextMenuStrip1;
//	QToolTip* toolTip1=nullptr;
	QLabel* pictureBox_TuningEnabled=nullptr;
	Field<int>* field_Power=nullptr;
	void InitializeComponent();

	const int Intensity=0;
	const int Speed=1;
	const int SecondarySpeed=2;
	const int Profit=3;
	const int Power=4;
	bool _isChange=false;

	bool _isInitFinished=false;

	DualAlgorithm* _algorithm=nullptr;
	int _currentlySelectedIntensity=-1;
	bool _isChangeSaved=false;

	inline bool IsChange() const { return _isChange;};
	inline void IsChange(bool value) { _isChange=_isChange || _isInitFinished && value;};

	IntensitiesModel* model=new IntensitiesModel(this);

	void InitLocale();
	void SetIntensities();
	void InitializeControls();
	void UpdateEnabled();
	void UpdateIntensities();
	void UpdateIntensityList();

private Q_SLOTS:
	void UpdateProfits();
	void ToolStripMenuItemClear_Click();
//	void Form_DcriValues_FormClosing();

	void ListView_Intensities_ItemSelectionChanged(const QItemSelection&, const QItemSelection&);
	void Button_Close_Clicked();
	void Button_Save_Clicked();
	void CheckBox_TuningEnabledCheckedChanged();
	void TextChangedSpeed();
	void TextChangedSecondarySpeed();
	void TextChangedPower();
	void TextChangedTuningStart();
	void TextChangedTuningEnd();
	void TextChangedTuningInterval();
	void ListView_Intensities_MouseClick(const QPoint& pos);

protected:
	void closeEvent(QCloseEvent* e); // Form_DcriValues_FormClosing
};

#endif
