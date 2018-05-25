#ifndef FORMS_FORM_BENCHMARK_H_
#define FORMS_FORM_BENCHMARK_H_

#include "Interfaces/IListItemCheckColorSetter.h"
#include "Interfaces/IBenchmarkForm.h"
#include "Interfaces/IBenchmarkCalculation.h"
#include "Benchmarking/BenchmarkHandler.h"
#include "Enums.h"
#include <QtWidgets>

class DevicesListViewEnableControl;
class BenchmarkOptions;
class AlgorithmsListView;
class Miner;
class ComputeDevice;
class Algorithm;


class Form_Benchmark
	: public QDialog, IListItemCheckColorSetter, IBenchmarkForm, IBenchmarkCalculation
{
	Q_OBJECT
	Q_PROPERTY(bool StartMining READ StartMining)
public:
	virtual ~Form_Benchmark() {};

	inline bool StartMining() const { return StartMining_;};
	inline bool InBenchmark() override { return InBenchmark_;};

	Form_Benchmark(QWidget* parent=Q_NULLPTR, Enums::BenchmarkPerformanceType benchmarkPerformanceType=Enums::BenchmarkPerformanceType::Standard, bool autostart=false);

	void CalcBenchmarkDevicesAlgorithmQueue();

	void AddToStatusCheck(ComputeDevice* device, Algorithm* algorithm);
	void RemoveFromStatusCheck(ComputeDevice* device, Algorithm* algorithm);
	void EndBenchmarkForDevice(ComputeDevice* device, bool failedAlgos) override;
	void SetCurrentStatus(ComputeDevice* device, Algorithm* algorithm, QString status) override;

	void StepUpBenchmarkStepProgress();

	void LviSetColor(QAbstractTableModel* model, int row);

	void StopBenchmark();

	void closeEvent(QCloseEvent* ev) override;

private:
	DevicesListViewEnableControl* devicesListViewEnableControl1=nullptr;
	QPushButton* StartStopBtn=nullptr;
	QPushButton* CloseBtn=nullptr;
	BenchmarkOptions* benchmarkOptions1=nullptr;
	QGroupBox* groupBoxBenchmarkProgress=nullptr;
	QLabel* labelBenchmarkSteps=nullptr;
	QProgressBar* progressBarBenchmarkSteps=nullptr;
	AlgorithmsListView* algorithmsListView1=nullptr;
	QRadioButton* radioButton_SelectedUnbenchmarked=nullptr;
	QRadioButton* radioButton_RE_SelectedUnbenchmarked=nullptr;
	QCheckBox* checkBox_StartMiningAfterBenchmark=nullptr;
	void InitializeComponent();

	static QColor DisabledColor/*=Qt::darkGray*/;
	static QColor BenchmarkedColor/*=QColor("#90EE90")*/; // lightGreen
	static QColor UnbenchmarkedColor/*=QColor("#ADD8E6")*/; // lightBlue

	Enums::AlgorithmBenchmarkSettingsType _algorithmOption=Enums::AlgorithmBenchmarkSettingsType::SelectedUnbenchmarkedAlgorithms;

	int _benchmarkCurrentIndex=0;
	int _benchmarkAlgorithmsCount=0;

	QList<QPair<ComputeDevice*, QQueue<Algorithm*>*>*>* _benchmarkDevicesAlgorithmQueue=nullptr;

	enum class BenchmarkSettingsStatus {
		NONE=0,
		TODO,
		DISABLED_NONE,
		DISABLED_TODO
		};
//	Q_ENUM(BenchmarkSettingsStatus)

	QMap<QString, BenchmarkSettingsStatus>* _benchmarkDevicesAlgorithmStatus=nullptr;

	QTimer* _benchmarkingTimer=nullptr;
	int _dotCount=0;

	bool _hasFailedAlgorithms=false;
	QList<BenchmarkHandler*> _runningBenchmarkThreads;
	static QMutex _runningBenchmarkThreadsMtx;
	QMap<ComputeDevice*, Algorithm*> _statusCheckAlgos;

	bool ExitWhenFinished=false;

	bool StartMining_=false;

	bool InBenchmark_=false;

	void CopyBenchmarks();
	QString GetDotsWaitString();
	void InitLocale();

	void BenchmarkStoppedGuiSettings();
	void StopButonClick();
	bool StartButonClick();
	bool ShoulBenchmark(Algorithm* algorithm);
	void StartBenchmark();
	void EndBenchmark();

	void SetLabelBenchmarkSteps(int current, int max);
	void ResetBenchmarkProgressStatus();

private Q_SLOTS:
	void BenchmarkingTimer_Tick();
	void StartStopBtn_Click();
	void CloseBtn_Click();
	void FormBenchmark_New_FormClosing(QCloseEvent* e);
	void DevicesListView1_ItemSelectionChanged(int);
	void RadioButton_SelectedUnbenchmarked_CheckedChanged_1();
	void RadioButton_RE_SelectedUnbenchmarked_CheckedChanged();
	void CheckBox_StartMiningAfterBenchmark_CheckedChanged();

Q_SIGNALS:
	void closing(QCloseEvent* ev);
};

#endif
