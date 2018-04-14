#ifndef FORMS_FORM_BENCHMARK_H_
#define FORMS_FORM_BENCHMARK_H_

#include "Interfaces/IListItemCheckColorSetter.h"
#include "Interfaces/IBenchmarkComunicator.h"
#include "Interfaces/IBenchmarkCalculation.h"
#include "Enums.h"
#include <QtWidgets>

class DevicesListViewEnableControl;
class BenchmarkOptions;
class AlgorithmsListView;
class Miner;
class ComputeDevice;
class Algorithm;


class Form_Benchmark
	: public QDialog, IListItemCheckColorSetter, IBenchmarkComunicator, IBenchmarkCalculation
{
	Q_OBJECT
	Q_PROPERTY(bool StartMining READ StartMining)
public:
	virtual ~Form_Benchmark() {};

	inline bool StartMining() const { return _StartMining;}

	enum class BenchmarkSettingsStatus {
		NONE=0,
		TODO,
		DISABLED_NONE,
		DISABLED_TODO
		};
	Q_ENUM(BenchmarkSettingsStatus)

	Form_Benchmark(QWidget* parent=Q_NULLPTR, Enums::BenchmarkPerformanceType benchmarkPerformanceType=Enums::BenchmarkPerformanceType::Standard, bool autostart=false);

	void StopBenchmark();
	void CalcBenchmarkDevicesAlgorithmQueue();
	void SetCurrentStatus(QString status);
	void OnBenchmarkComplete(bool success, QString status);

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
	QRadioButton* radioButton_SelectUnbenchmarked=nullptr;
	QRadioButton* radioButton_RE_SelectedUnbenchmarked=nullptr;
	QCheckBox* checkBox_StartMiningAfterBenchmark=nullptr;
	void InitializeComponent();

	bool _inBenchmark=false;
	int _benchmarkCurrentIndex=0;
	int _benchmarkedSuccesCount=0;
	int _benchmarkAlgorithmsCount=0;

	Enums::AlgorithmBenchmarkSettingsType _algorithmOption=Enums::AlgorithmBenchmarkSettingsType::SelectedUnbenchmarkedAlgorithms;

	QList<Miner*>* _benchmarkMiners=nullptr;
	Miner* _currentMiner=nullptr;
	QList<QPair<ComputeDevice*, QQueue<Algorithm*>*>*>* _benchmarkDevicesAlgorithmQueue=nullptr;

	bool _exitWhenFinished=false;

	QTimer* _benchmarkingTimer=nullptr;
	int _dotCount=0;

	bool _StartMining=false;

	struct DeviceAlgo {
		QString Device;
		QString Algorithm;
		};
	QList<DeviceAlgo*>* _benchmarkFailedAlgoPerDev=nullptr;

	QMap<QString, BenchmarkSettingsStatus>* _benchmarkDevicesAlgorithmStatus=nullptr;
	ComputeDevice* _currentDevice=nullptr;
	Algorithm* _currentAlgorithm=nullptr;

	QString _currentAlgoName;

	// CPU benchmarking helpers
	class CpuBenchmarkStatus {
	private:
		class CpuBenchmark {
		public:
			CpuBenchmark(int lt, double bench) {
				LessThreads=lt;
				Benchmark=bench;
				};
			int LessThreads;
			double Benchmark;
			};

	public:
		CpuBenchmarkStatus(int maxThreads) {
			_maxThreads=maxThreads;
			};
		bool HasTest() {
			return LessTreads<_maxThreads;
			};
		void SetNextSpeed(double speed) {
			if (HasTest()) {
				_benchmarks->append(new CpuBenchmark(LessTreads, speed));
				++LessTreads;
				}
			};
		void FindFastest() {
			qSort(_benchmarks->begin(), _benchmarks->end(), [](CpuBenchmark* a, CpuBenchmark* b) { return -a->Benchmark-b->Benchmark; });
			};
		double GetBestSpeed() {
			return (*_benchmarks)[0]->Benchmark;
			};
		int GetLessThreads() {
			return (*_benchmarks)[0]->LessThreads;
			};
	private:
		int _maxThreads;
		QList<CpuBenchmark*>* _benchmarks=new QList<CpuBenchmark*>;
		int LessTreads=0;
	public:
		inline int LessThreads() const { return LessTreads; };
		int Time;
		};

	CpuBenchmarkStatus* _cpuBenchmarkStatus=nullptr;

	class ClaymoreZcashStatus {
	private:
		const int MaxBench=2;
		QVector<QStringList> _asmModes={{"-asm", "1"}, {"-asm", "0"}};

		QVector<double> _speeds={0.0d, 0.0d};
		int _curIndex=0;
		QStringList _originalExtraParams;

	public:
		ClaymoreZcashStatus(QStringList oep) {
			_originalExtraParams=oep;
			};
		inline bool HasTest() { return _curIndex<MaxBench; };
		void SetSpeed(double speed) {
			if (HasTest()) {
				_speeds[_curIndex]=speed;
				}
			};
		inline void SetNext() { _curIndex++; };
		QStringList GetTestExtraParams() {
			if (HasTest()) {
				return _originalExtraParams << _asmModes[_curIndex];
				}
			return _originalExtraParams;
			};
	private:
		int FastestIndex() {
			int maxIndex=0;
			double maxValue=_speeds[maxIndex];
			for (int i=1; i<_speeds.length(); ++i) {
				if (_speeds[i]>maxValue) {
					maxIndex=i;
					maxValue=_speeds[i];
					}
				}
			return 0;
			};
	public:
		inline QStringList GetFastestExtraParams() { return _originalExtraParams << _asmModes[FastestIndex()]; };
		inline double GetFastestTime() { return _speeds[FastestIndex()]; };
		int Time=180;
		};

	ClaymoreZcashStatus* _claymoreZcashStatus=nullptr;

	// CPU sweet spots
	QList<Enums::AlgorithmType>* _cpuAlgos=new QList<Enums::AlgorithmType>({ Enums::AlgorithmType::CryptoNight, Enums::AlgorithmType::CryptoNightV7 });

	QColor DisabledColor=Qt::darkGray;
	QColor BenchmarkedColor=QColor("#90EE90"); // lightgreen
	QColor UnbenchmarkedColor=QColor("#ADD8E6"); // lightblue

	void LviSetColor(QAbstractTableModel* model, int row);
	void CopyBenchmarks();
	QString GetDotsWaitString();
	void InitLocale();
	void BenchmarkStoppedGuiSettings();
	void StopButonClick();
	bool StartButonClick();
	bool ShoulBenchmark(Algorithm* algorithm);
	void StartBenchmark();
	void NextBenchmark();
	void EndBenchmark();
	void SetLabelBenchmarkSteps(int current, int max);
	void StepUpBenchmarkStepProgress();
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
