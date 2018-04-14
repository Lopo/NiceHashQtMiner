#ifndef FORMS_COMPONENTS_BENCHMARKOPTIONS_H_
#define FORMS_COMPONENTS_BENCHMARKOPTIONS_H_

#include <QtWidgets>
#include "Enums.h"


class BenchmarkOptions
	: public QWidget
{
	Q_OBJECT
	Q_PROPERTY(Enums::BenchmarkPerformanceType PerformanceType READ PerformanceType)
public:
	BenchmarkOptions(QWidget* parent=Q_NULLPTR);

	inline Enums::BenchmarkPerformanceType PerformanceType() const { return _PerformanceType; };

	void SetPerformanceType(const Enums::BenchmarkPerformanceType performanceType);

	void InitLocale();

private:
	QGroupBox* groupBox1;
	QRadioButton* radioButton_PreciseBenchmark=nullptr;
	QRadioButton* radioButton_StandardBenchmark=nullptr;
	QRadioButton* radioButton_QuickBenchmark=nullptr;
	void InitializeComponent();

	Enums::BenchmarkPerformanceType _PerformanceType;

private slots:
	void RadioButton_QuickBenchmark_CheckedChanged(const bool checked);
	void RadioButton_StandardBenchmark_CheckedChanged(const bool checked);
	void RadioButton_PreciseBenchmark_CheckedChanged(const bool checked);
};

#endif
