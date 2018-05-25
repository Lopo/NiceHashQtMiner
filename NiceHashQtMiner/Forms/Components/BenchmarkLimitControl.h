#ifndef FORMS_COMPONENTS_BENCHMARKLIMITCONTROL_H_
#define FORMS_COMPONENTS_BENCHMARKLIMITCONTROL_H_

#include <QtWidgets>

#include "Enums.h"


class BenchmarkLimitControl
	: public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QString GroupName READ GroupName WRITE setGroupName)
	Q_PROPERTY(QVector<int>* TimeLimits READ TimeLimits WRITE setTimeLimits)
public:
	BenchmarkLimitControl(QWidget* parent=Q_NULLPTR);

	inline QString GroupName() const { return groupBox1->title(); };
	void setGroupName(const QString value);
	inline QVector<int>* TimeLimits() const { return _timeLimits; };
	void setTimeLimits(QVector<int>* value);

	void SetToolTip(/*QToolTip* toolTip, */QString groupTypeName);

	void InitLocale();

private:
	QGroupBox* groupBox1=nullptr;
//	QWidget* flowLayoutPanel1=nullptr;
//	QVBoxLayout* flowLayoutPanel1;
//	QWidget* panel1=nullptr;
	QSpinBox* textBoxQuick=nullptr;
	QLabel* labelQuick=nullptr;
//	QWidget* panel2=nullptr;
	QSpinBox* textBoxPrecise=nullptr;
	QLabel* labelPrecise=nullptr;
//	QWidget* panel3=nullptr;
	QSpinBox* textBoxStandard=nullptr;
	QLabel* labelStandard=nullptr;
	void InitializeComponent();

	QVector<int>* _timeLimits=nullptr;
	QVector<QSpinBox*>* _textBoxes=nullptr;

	void SetTimeLimit(Enums::BenchmarkPerformanceType type, int num);

private Q_SLOTS:
	void TextBoxQuick_TextChanged();
	void TextBoxStandard_TextChanged();
	void TextBoxPrecise_TextChanged();
};

#endif
