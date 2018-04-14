#include "Forms/Form_Benchmark.h"
#include "Devices/ComputeDeviceManager.h"
#include "Forms/Components/BenchmarkOptions.h"
#include "Forms/Components/DevicesListViewEnableControl.h"
#include "Forms/Components/AlgorithmsListView.h"
#include "Utils/Helpers.h"
#include "Miners/Miner.h"
#include "Miners/MinerFactory.h"
#include "Globals.h"
#include "Utils/AlgorithmNiceHashNames.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Miners/Grouping/MiningPair.h"
#include "International.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Properties/Resources.h"


Form_Benchmark::Form_Benchmark(QWidget *parent, Enums::BenchmarkPerformanceType benchmarkPerformanceType/*=Enums::BenchmarkPerformanceType::Standard*/, bool autostart/*=false*/)
	: QDialog(parent)
{
	InitializeComponent();
	Resources resources;
	setWindowIcon(resources.logo);

	_StartMining=false;

	// clear prev pending statuses
	foreach (ComputeDevice* dev, *ComputeDeviceManager.Avaliable.AllAvailableDevices) {
		foreach (Algorithm* algo, *dev->GetAlgorithmSettings()) {
			algo->ClearBenchmarkPendingFirst();
			}
		}

	benchmarkOptions1->SetPerformanceType(benchmarkPerformanceType);

	// benchmark only unique devices
	devicesListViewEnableControl1->SetIListItemCheckColorSetter(this);
	devicesListViewEnableControl1->SetComputeDevices(ComputeDeviceManager.Avaliable.AllAvailableDevices);

	// use this to track miner benchmark statuses
	_benchmarkMiners=new QList<Miner*>;

	InitLocale();

	_benchmarkingTimer=new QTimer(this);
	connect(_benchmarkingTimer, SIGNAL(timeout()), this, SLOT(BenchmarkingTimer_Tick()));
	_benchmarkingTimer->setInterval(1000); // 1s

	devicesListViewEnableControl1->setEnabled(true);
//	devicesListViewEnableControl1->SetDeviceSelectionChangedCallback((void (*)(int))&Form_Benchmark::DevicesListView1_ItemSelectionChanged);
	connect(devicesListViewEnableControl1, SIGNAL(ItemSelectionChanged(int)), this, SLOT(DevicesListView1_ItemSelectionChanged(int)));

	devicesListViewEnableControl1->SetAlgorithmsListView(algorithmsListView1);
	devicesListViewEnableControl1->IsBenchmarkForm=true;
	devicesListViewEnableControl1->IsSettingsCopyEnabled=true;

	ResetBenchmarkProgressStatus();
	CalcBenchmarkDevicesAlgorithmQueue();
	devicesListViewEnableControl1->ResetListItemColors();

	// to update calculation status
	devicesListViewEnableControl1->BenchmarkCalculation=this;
	algorithmsListView1->BenchmarkCalculation=this;

	// set first device selected
	if (ComputeDeviceManager.Avaliable.AllAvailableDevices->count()>0) {
		ComputeDevice* firstComputeDevice=ComputeDeviceManager.Avaliable.AllAvailableDevices->first();
		algorithmsListView1->SetAlgorithms(firstComputeDevice, firstComputeDevice->Enabled);
		}

	if (autostart) {
		_exitWhenFinished=true;
		StartStopBtn_Click();
		}
}

void Form_Benchmark::InitializeComponent()
{
	QFont f;
	f.setPointSize(6);

	StartStopBtn=new QPushButton(this);
	StartStopBtn->setObjectName("StartStopBtn");
	StartStopBtn->setGeometry(436, 366, 75, 23);
	StartStopBtn->setText("&Start");
	connect(StartStopBtn, SIGNAL(clicked()), this, SLOT(StartStopBtn_Click()));

	CloseBtn=new QPushButton(this);
	CloseBtn->setObjectName("CloseBtn");
	CloseBtn->setGeometry(517, 366, 75, 23);
	CloseBtn->setText("&Close");
	connect(CloseBtn, SIGNAL(clicked()), this, SLOT(CloseBtn_Click()));

	groupBoxBenchmarkProgress=new QGroupBox(this);
	groupBoxBenchmarkProgress->setObjectName("groupBoxBenchmarkProgress");
	groupBoxBenchmarkProgress->setGeometry(12, 342, 418, 47);
	groupBoxBenchmarkProgress->setTitle("Benchmark progress status:");
	groupBoxBenchmarkProgress->setStyleSheet("QGroupBox { border: 1px solid; margin-top: 0.3em;} QGroupBox::title { top: -0.4em; left: 10px;}");
	groupBoxBenchmarkProgress->setFont(f);

	labelBenchmarkSteps=new QLabel(groupBoxBenchmarkProgress);
	labelBenchmarkSteps->setObjectName("labelBenchmarkSteps");
	labelBenchmarkSteps->setGeometry(6, 24, 116, 13);
	labelBenchmarkSteps->setText("Benchmark step (0/10)");
	labelBenchmarkSteps->setFont(f);

	progressBarBenchmarkSteps=new QProgressBar(groupBoxBenchmarkProgress);
	progressBarBenchmarkSteps->setObjectName("progressBarBenchmarkSteps");
	progressBarBenchmarkSteps->setGeometry(162, 16, 161, 23);

	radioButton_SelectUnbenchmarked=new QRadioButton(this);
	radioButton_SelectUnbenchmarked->setObjectName("radioButton_SelectUnbenchmarked");
	radioButton_SelectUnbenchmarked->setGeometry(21, 295, 260, 17);
	radioButton_SelectUnbenchmarked->setText("Benchmark Selected Unbenchmarked Algorithms ");
	radioButton_SelectUnbenchmarked->setChecked(true);
	connect(radioButton_SelectUnbenchmarked, SIGNAL(toggled(bool)), this, SLOT(RadioButton_SelectedUnbenchmarked_CheckedChanged_1()));

	radioButton_RE_SelectedUnbenchmarked=new QRadioButton(this);
	radioButton_RE_SelectedUnbenchmarked->setObjectName("radioButton_RE_SelectedUnbenchmarked");
	radioButton_RE_SelectedUnbenchmarked->setGeometry(21, 318, 192, 17);
	radioButton_RE_SelectedUnbenchmarked->setText("Benchmark All Selected Algorithms ");
	connect(radioButton_RE_SelectedUnbenchmarked, SIGNAL(toggled(bool)), this, SLOT(RadioButton_RE_SelectedUnbenchmarked_CheckedChanged()));

	checkBox_StartMiningAfterBenchmark=new QCheckBox(this);
	checkBox_StartMiningAfterBenchmark->setObjectName("checkBox_StartMiningAfterBenchmark");
	checkBox_StartMiningAfterBenchmark->setGeometry(350, 318, 161, 17);
	checkBox_StartMiningAfterBenchmark->setText("Start mining after benchmark");
	connect(checkBox_StartMiningAfterBenchmark, SIGNAL(stateChanged(int)), this, SLOT(CheckBox_StartMiningAfterBenchmark_CheckedChanged()));

	algorithmsListView1=new AlgorithmsListView(this);
	algorithmsListView1->setObjectName("algorithmsListView1");
	algorithmsListView1->setGeometry(12, 133, 580, 155);
	algorithmsListView1->BenchmarkCalculation=nullptr;
	algorithmsListView1->ComunicationInterface=nullptr;
	algorithmsListView1->setIsInBenchmark(false);

	benchmarkOptions1=new BenchmarkOptions(this);
	benchmarkOptions1->setObjectName("benchmarkOptions1");
	benchmarkOptions1->setGeometry(394, 15, 208, 112);

	devicesListViewEnableControl1=new DevicesListViewEnableControl(this);
	devicesListViewEnableControl1->setObjectName("devicesListViewEnableControl1");
	devicesListViewEnableControl1->setGeometry(12, 15, 376, 112);
	devicesListViewEnableControl1->BenchmarkCalculation=nullptr;
//	devicesListViewEnableControl1->setFirstColumnText("Benchmark");
	devicesListViewEnableControl1->setIsInBenchmark(false);
	devicesListViewEnableControl1->setIsMining(false);
	devicesListViewEnableControl1->SaveToGeneralConfig=false;

	resize(604, 401);
	setFixedSize(604, 401);
	setObjectName("Form_Benchmark");
	setWindowTitle("Benchmark");
	setFont(f);
	connect(this, SIGNAL(closing(QCloseEvent*)), this, SLOT(FormBenchmark_New_FormClosing(QCloseEvent*)));
}

void Form_Benchmark::LviSetColor(QAbstractTableModel* model, int row)
{
	QModelIndex idx=model->index(row, 0, QModelIndex());
	ComputeDevice* cDevice=model->data(idx, Qt::EditRole).value<ComputeDevice*>();
	if (cDevice!=nullptr && _benchmarkDevicesAlgorithmStatus!=nullptr) {
		QString uuid=cDevice->Uuid();
		if (!cDevice->Enabled) {
			model->setData(idx, DisabledColor, Qt::BackgroundColorRole);
			}
		else {
			switch ((*_benchmarkDevicesAlgorithmStatus)[uuid]) {
				case BenchmarkSettingsStatus::TODO:
				case BenchmarkSettingsStatus::DISABLED_TODO:
					model->setData(idx, UnbenchmarkedColor, Qt::BackgroundColorRole);
					break;
				case BenchmarkSettingsStatus::NONE:
				case BenchmarkSettingsStatus::DISABLED_NONE:
					model->setData(idx, BenchmarkedColor, Qt::BackgroundColorRole);
					break;
				}
			}
		}
}

void Form_Benchmark::CopyBenchmarks()
{
	Helpers::ConsolePrint("CopyBenchmarks", "Checking for benchmarks to copy");
	foreach (ComputeDevice* cDev, *ComputeDeviceManager.Avaliable.AllAvailableDevices) {
		// check if copy
		if (!cDev->Enabled && !cDev->BenchmarkCopyUuid.isEmpty()) {
			ComputeDevice* copyCdevSettings=ComputeDeviceManager.Avaliable.GetDeviceWithUuid(cDev->BenchmarkCopyUuid);
			if (copyCdevSettings!=nullptr) {
				Helpers::ConsolePrint("CopyBenchmarks", QString("Copy from %1 to %2").arg(cDev->Uuid()).arg(cDev->BenchmarkCopyUuid));
				cDev->CopyBenchmarkSettingsFrom(copyCdevSettings);
				}
			}
		}
}

void Form_Benchmark::BenchmarkingTimer_Tick()
{
	if (_inBenchmark) {
		algorithmsListView1->SetSpeedStatus(_currentDevice, _currentAlgorithm, GetDotsWaitString());
		}
}

QString Form_Benchmark::GetDotsWaitString()
{
	++_dotCount;
	if (_dotCount>3) {
		_dotCount=1;
		}
	return QString(_dotCount, QChar('.'));
}

void Form_Benchmark::InitLocale()
{
	setWindowTitle(International::GetText("Form_Benchmark_title"));
	StartStopBtn->setText(International::GetText("SubmitResultDialog_StartBtn"));
	CloseBtn->setText(International::GetText("SubmitResultDialog_CloseBtn"));

	// @todo fix locale for benchmark enabled label
	devicesListViewEnableControl1->InitLocale();
	benchmarkOptions1->InitLocale();
	algorithmsListView1->InitLocale();
	groupBoxBenchmarkProgress->setTitle(International::GetText("FormBenchmark_Benchmark_GroupBoxStatus"));
	radioButton_SelectUnbenchmarked->setText(International::GetText("FormBenchmark_Benchmark_All_Selected_Unbenchmarked"));
	radioButton_RE_SelectedUnbenchmarked->setText(International::GetText("FormBenchmark_Benchmark_All_Selected_ReUnbenchmarked"));
	checkBox_StartMiningAfterBenchmark->setText(International::GetText("Form_Benchmark_checkbox_StartMiningAfterBenchmark"));
}

void Form_Benchmark::StartStopBtn_Click()
{
	if (_inBenchmark) {
		StopButonClick();
		BenchmarkStoppedGuiSettings();
		}
	else if (StartButonClick()) {
		StartStopBtn->setText(International::GetText("Form_Benchmark_buttonStopBenchmark"));
		}
}

void Form_Benchmark::StopBenchmark()
{
	if (_inBenchmark) {
		StopButonClick();
		BenchmarkStoppedGuiSettings();
		}
}

void Form_Benchmark::BenchmarkStoppedGuiSettings()
{
	StartStopBtn->setText(International::GetText("Form_Benchmark_buttonStartBenchmark"));
	// clear benchmark pending status
	if (_currentAlgorithm!=nullptr) {
		_currentAlgorithm->ClearBenchmarkPending();
		}
	QPair<ComputeDevice*, QQueue<Algorithm*>*>* deviceAlgosTuple;
	foreach (deviceAlgosTuple, *_benchmarkDevicesAlgorithmQueue) {
		foreach (Algorithm* algo, *deviceAlgosTuple->second) {
			algo->ClearBenchmarkPending();
			}
		}
	ResetBenchmarkProgressStatus();
	CalcBenchmarkDevicesAlgorithmQueue();
	benchmarkOptions1->setEnabled(true);

	algorithmsListView1->setIsInBenchmark(false);
	devicesListViewEnableControl1->setIsInBenchmark(false);
	if (_currentDevice!=nullptr) {
		algorithmsListView1->RepaintStatus(_currentDevice->Enabled, _currentDevice->Uuid());
		}
	CloseBtn->setEnabled(true);
}

// @todo add list for safety and kill all miners
void Form_Benchmark::StopButonClick()
{
	_benchmarkingTimer->stop();
	_inBenchmark=false;
	Helpers::ConsolePrint("FormBenchmark", "StopButtonClick() benchmark routine stopped");
	// copy benchmarked
	if (_currentMiner!=nullptr) {
		_currentMiner->BenchmarkSignalQuit=true;
		_currentMiner->InvokeBenchmarkSignalQuit();
		}
	if (_exitWhenFinished) {
		close();
		}
}

bool Form_Benchmark::StartButonClick()
{
	CalcBenchmarkDevicesAlgorithmQueue();
	// device selection check scope
	{
		bool noneSelected=true;
		foreach (ComputeDevice* cDev, *ComputeDeviceManager.Avaliable.AllAvailableDevices) {
			if (cDev->Enabled) {
				noneSelected=false;
				break;
				}
			}
		if (noneSelected) {
			QMessageBox::information(this, International::GetText("FormBenchmark_No_Devices_Selected_Title"), International::GetText("FormBenchmark_No_Devices_Selected_Msg"), QMessageBox::Ok);
			return false;
			}
	}
	// device todo benchmark check scope
	{
		bool nothingToBench=true;
		foreach (BenchmarkSettingsStatus statusKpv, *_benchmarkDevicesAlgorithmStatus) {
			if (statusKpv==BenchmarkSettingsStatus::TODO) {
				nothingToBench=false;
				break;
				}
			}
		if (nothingToBench) {
			QMessageBox::information(this, International::GetText("FormBenchmark_Nothing_to_Benchmark_Title"), International::GetText("FormBenchmark_Nothing_to_Benchmark_Msg"), QMessageBox::Ok);
			return false;
			}
	}

	// current failed new list
	_benchmarkFailedAlgoPerDev=new QList<DeviceAlgo*>;
	// disable gui controls
	benchmarkOptions1->setEnabled(false);
	CloseBtn->setEnabled(false);
	algorithmsListView1->setIsInBenchmark(true);
	devicesListViewEnableControl1->setIsInBenchmark(true);
	// set benchmark pending status
	QPair<ComputeDevice*, QQueue<Algorithm*>*>* deviceAlgosTuple;
	foreach (deviceAlgosTuple, *_benchmarkDevicesAlgorithmQueue) {
		foreach (Algorithm* algo, *deviceAlgosTuple->second) {
			algo->SetBenchmarkPending();
			}
		}
	if (_currentDevice!=nullptr) {
		algorithmsListView1->RepaintStatus(_currentDevice->Enabled, _currentDevice->Uuid());
		}

	StartBenchmark();

	return true;
}

void Form_Benchmark::CalcBenchmarkDevicesAlgorithmQueue()
{
	_benchmarkAlgorithmsCount=0;
	_benchmarkDevicesAlgorithmStatus=new QMap<QString, BenchmarkSettingsStatus>;
	_benchmarkDevicesAlgorithmQueue=new QList<QPair<ComputeDevice*, QQueue<Algorithm*>*>*>;
	foreach (ComputeDevice* cDev, *ComputeDeviceManager.Avaliable.AllAvailableDevices) {
		QQueue<Algorithm*>* algorithmQueue=new QQueue<Algorithm*>;
		foreach (Algorithm* algo, *cDev->GetAlgorithmSettings()) {
			if (ShoulBenchmark(algo)) {
				algorithmQueue->enqueue(algo);
				algo->SetBenchmarkPendingNoMsg();
				}
			else {
				algo->ClearBenchmarkPending();
				}
			}

		BenchmarkSettingsStatus status=BenchmarkSettingsStatus::NONE;
		if (cDev->Enabled) {
			_benchmarkAlgorithmsCount+=algorithmQueue->count();
			status= algorithmQueue->count()? BenchmarkSettingsStatus::TODO : BenchmarkSettingsStatus::NONE;
			_benchmarkDevicesAlgorithmQueue->append(new QPair<ComputeDevice*, QQueue<Algorithm*>*>(cDev, algorithmQueue));
			}
		else {
			status= algorithmQueue->count()? BenchmarkSettingsStatus::DISABLED_TODO : BenchmarkSettingsStatus::DISABLED_NONE;
			}
		_benchmarkDevicesAlgorithmStatus->insert(cDev->Uuid(), status);
		}
	// GUI stuff
	progressBarBenchmarkSteps->setMaximum(_benchmarkAlgorithmsCount);
	progressBarBenchmarkSteps->setValue(0);
	SetLabelBenchmarkSteps(0, _benchmarkAlgorithmsCount);
}

bool Form_Benchmark::ShoulBenchmark(Algorithm* algorithm)
{
	bool isBenchmarked=algorithm->BenchmarkSpeed>0;
	if (_algorithmOption==Enums::AlgorithmBenchmarkSettingsType::SelectedUnbenchmarkedAlgorithms
		&& !isBenchmarked
		&& algorithm->Enabled
		) {
		return true;
		}
	if (_algorithmOption==Enums::AlgorithmBenchmarkSettingsType::UnbenchmarkedAlgorithms && !isBenchmarked) {
		return true;
		}
	if (_algorithmOption==Enums::AlgorithmBenchmarkSettingsType::ReBenchSelectedAlgorithms && algorithm->Enabled) {
		return true;
		}
	if (_algorithmOption==Enums::AlgorithmBenchmarkSettingsType::AllAlgorithms) {
		return true;
		}

	return false;
}

void Form_Benchmark::StartBenchmark()
{
	_inBenchmark=true;
	_benchmarkCurrentIndex=-1;
	NextBenchmark();
}

void Form_Benchmark::NextBenchmark()
{
	if (_benchmarkCurrentIndex>-1) {
		StepUpBenchmarkStepProgress();
		}
	++_benchmarkCurrentIndex;
	if (_benchmarkCurrentIndex>=_benchmarkAlgorithmsCount) {
		EndBenchmark();
		return;
		}

	QPair<ComputeDevice*, QQueue<Algorithm*>*>* currentDeviceAlgosTuple=nullptr;
	QQueue<Algorithm*>* algorithmBenchmarkQueue=nullptr;
	while (_benchmarkDevicesAlgorithmQueue->count()>0) {
		currentDeviceAlgosTuple=_benchmarkDevicesAlgorithmQueue->at(0);
		_currentDevice=currentDeviceAlgosTuple->first;
		algorithmBenchmarkQueue=currentDeviceAlgosTuple->second;
		if (algorithmBenchmarkQueue->count()) {
			_currentAlgorithm=algorithmBenchmarkQueue->dequeue();
			break;
			}
		_benchmarkDevicesAlgorithmQueue->removeAt(0);
		}

	if (_currentDevice!=nullptr && _currentAlgorithm!=nullptr) {
		_currentMiner=MinerFactory::CreateMiner(_currentDevice, _currentAlgorithm);
		if (_currentAlgorithm->MinerBaseType==Enums::MinerBaseType::XmrStackCPU
			&& (_currentAlgorithm->NiceHashID==Enums::AlgorithmType::CryptoNight || _currentAlgorithm->NiceHashID==Enums::AlgorithmType::CryptoNightV7)
			&& _currentAlgorithm->ExtraLaunchParameters.isEmpty()
			&& !_currentAlgorithm->ExtraLaunchParameters.contains("enable_ht=true")
			) {
			_cpuBenchmarkStatus=new CpuBenchmarkStatus(Globals::ThreadsPerCpu);
			_currentAlgorithm->LessThreads=_cpuBenchmarkStatus->LessThreads();
			}
		else {
//			if (_cpuBenchmarkStatus!=nullptr) {delete _cpuBenchmarkStatus;}
			_cpuBenchmarkStatus=nullptr;
			}
		if (_currentAlgorithm->MinerBaseType==Enums::MinerBaseType::Claymore
			&& _currentAlgorithm->NiceHashID==Enums::AlgorithmType::Equihash
			&& !_currentAlgorithm->ExtraLaunchParameters.isEmpty()
			&& !_currentAlgorithm->ExtraLaunchParameters.contains("-asm")
			) {
			_claymoreZcashStatus=new ClaymoreZcashStatus(_currentAlgorithm->ExtraLaunchParameters);
			_currentAlgorithm->ExtraLaunchParameters=_claymoreZcashStatus->GetTestExtraParams();
			}
		else {
//			if (_claymoreZcashStatus!=nullptr) {delete _claymoreZcashStatus;}
			_claymoreZcashStatus=nullptr;
			}
		}

	if (_currentMiner!=nullptr && _currentAlgorithm!=nullptr) {
		_benchmarkMiners->append(_currentMiner);
		_currentAlgoName=AlgorithmNiceHashNames::GetName(_currentAlgorithm->NiceHashID);
		_currentMiner->InitBenchmarkSetup(new MiningPair(_currentDevice, _currentAlgorithm));

		if (_currentDevice!=nullptr) {
			int time=ConfigManager.generalConfig->BenchmarkTimeLimits->GetBenchmarktime(benchmarkOptions1->PerformanceType(), _currentDevice->DeviceGroupType);
			if (_cpuBenchmarkStatus!=nullptr) {
				_cpuBenchmarkStatus->Time=time;
				}
			if (_claymoreZcashStatus!=nullptr) {
				_claymoreZcashStatus->Time=time;
				}

			// dagger about 4 minutes
//			int showWaitTime= _currentAlgorithm->NiceHashID==Enums::AlgorithmType::DaggerHashimoto? 4*60 : time;

			_dotCount=0;
			_benchmarkingTimer->start();

			_currentMiner->BenchmarkStart(time, this);
			}
		algorithmsListView1->SetSpeedStatus(_currentDevice, _currentAlgorithm, GetDotsWaitString());
		}
	else {
		NextBenchmark();
		}
}

void Form_Benchmark::EndBenchmark()
{
	_benchmarkingTimer->stop();
	_inBenchmark=false;
	Helpers::ConsolePrint("FormBenchmark", "EndBenchmark() benchmark routine finished");

	BenchmarkStoppedGuiSettings();
	// check if all ok
	if (!_benchmarkFailedAlgoPerDev->count() && StartMining()==false) {
		QMessageBox::information(this, International::GetText("FormBenchmark_Benchmark_Finish_MsgBox_Title"), International::GetText("FormBenchmark_Benchmark_Finish_Succes_MsgBox_Msg"), QMessageBox::Ok);
		}
	else if (StartMining()==false) {
		QMessageBox::StandardButton result=QMessageBox::question(this, International::GetText("FormBenchmark_Benchmark_Finish_MsgBox_Title"), International::GetText("FormBenchmark_Benchmark_Finish_Fail_MsgBox_Msg"), QMessageBox::Retry|QMessageBox::Cancel);
		if (result==QMessageBox::Retry) {
			StartButonClick();
			return;
			}
		// get unbenchmarked from criteria and disable
		CalcBenchmarkDevicesAlgorithmQueue();
		QPair<ComputeDevice*, QQueue<Algorithm*>*>* deviceAlgoQueue;
		foreach (deviceAlgoQueue, *_benchmarkDevicesAlgorithmQueue) {
			foreach (Algorithm* algorithm, *deviceAlgoQueue->second) {
				algorithm->Enabled=false;
				}
			}
		}
	if (_exitWhenFinished || StartMining()) {
		close();
		}
}

void Form_Benchmark::SetCurrentStatus(QString status)
{
//	algorithmsListView1.SetSpeedStatus(_currentDevice, _currentAlgorithm, getDotsWaitString());
	QMetaObject::invokeMethod(algorithmsListView1, "SetSpeedStatus", Q_ARG(ComputeDevice*, _currentDevice), Q_ARG(Algorithm*, _currentAlgorithm), Q_ARG(QString, GetDotsWaitString()));
}

void Form_Benchmark::OnBenchmarkComplete(bool success, QString status)
{
	if (!_inBenchmark) {
		return;
		}
	QMetaObject::invokeMethod(this, [=]{
		_benchmarkedSuccesCount+= success? 1 : 0;
		bool rebenchSame=false;
		if (success
			&& _cpuBenchmarkStatus!=nullptr
			&& _cpuAlgos->contains(_currentAlgorithm->NiceHashID)
			&& _currentAlgorithm->MinerBaseType==Enums::MinerBaseType::XmrStackCPU
			) {
			_cpuBenchmarkStatus->SetNextSpeed(_currentAlgorithm->BenchmarkSpeed);
			rebenchSame=_cpuBenchmarkStatus->HasTest();
			_currentAlgorithm->LessThreads=_cpuBenchmarkStatus->LessThreads();
			if (!rebenchSame) {
				_cpuBenchmarkStatus->FindFastest();
				_currentAlgorithm->BenchmarkSpeed=_cpuBenchmarkStatus->GetBestSpeed();
				_currentAlgorithm->LessThreads=_cpuBenchmarkStatus->GetLessThreads();
				}
			}

		if (_claymoreZcashStatus!=nullptr
			&& _currentAlgorithm->MinerBaseType==Enums::MinerBaseType::Claymore
			&& _currentAlgorithm->NiceHashID==Enums::AlgorithmType::Equihash
			) {
			if (_claymoreZcashStatus->HasTest()) {
				_currentMiner=MinerFactory::CreateMiner(_currentDevice, _currentAlgorithm);
				rebenchSame=true;
				_claymoreZcashStatus->SetSpeed(_currentAlgorithm->BenchmarkSpeed);
				_claymoreZcashStatus->SetNext();
				_currentAlgorithm->ExtraLaunchParameters=_claymoreZcashStatus->GetTestExtraParams();
				Helpers::ConsolePrint("ClaymoreAMD_Equihash", _currentAlgorithm->ExtraLaunchParameters.join(' '));
				_currentMiner->InitBenchmarkSetup(new MiningPair(_currentDevice, _currentAlgorithm));
				}

			if (!_claymoreZcashStatus->HasTest()) {
				rebenchSame=false;
				// set fastest mode
				_currentAlgorithm->BenchmarkSpeed=_claymoreZcashStatus->GetFastestTime();
				_currentAlgorithm->ExtraLaunchParameters=_claymoreZcashStatus->GetFastestExtraParams();
				}
			}

		if (!rebenchSame) {
			_benchmarkingTimer->stop();
			}

		if (!success && !rebenchSame) {
			// add new failed list
			DeviceAlgo* da=new DeviceAlgo;
			da->Device=_currentDevice->Name;
			da->Algorithm=_currentAlgorithm->AlgorithmName;
			_benchmarkFailedAlgoPerDev->append(da);
			algorithmsListView1->SetSpeedStatus(_currentDevice, _currentAlgorithm, status);
			}
		else if (!rebenchSame) {
			// set status to empty string it will return speed
			_currentAlgorithm->ClearBenchmarkPending();
			algorithmsListView1->SetSpeedStatus(_currentDevice, _currentAlgorithm, "");
			}
		if (rebenchSame) {
			if (_cpuBenchmarkStatus!=nullptr) {
				_currentMiner->BenchmarkStart(_cpuBenchmarkStatus->Time, this);
				}
			else if (_claymoreZcashStatus!=nullptr) {
				_currentMiner->BenchmarkStart(_claymoreZcashStatus->Time, this);
				}
			}
		else {
			NextBenchmark();
			}
		});
}

void Form_Benchmark::SetLabelBenchmarkSteps(int current, int max)
{
	labelBenchmarkSteps->setText(International::GetText("FormBenchmark_Benchmark_Step").arg(current).arg(max));
}

void Form_Benchmark::StepUpBenchmarkStepProgress()
{
	SetLabelBenchmarkSteps(_benchmarkCurrentIndex+1, _benchmarkAlgorithmsCount);
	progressBarBenchmarkSteps->setValue(_benchmarkCurrentIndex+1);
}

void Form_Benchmark::ResetBenchmarkProgressStatus()
{
	progressBarBenchmarkSteps->setValue(0);
}

void Form_Benchmark::CloseBtn_Click()
{
	close();
}

void Form_Benchmark::FormBenchmark_New_FormClosing(QCloseEvent* e)
{
	if (_inBenchmark) {
		e->ignore();
		return;
		}

	// disable all pending benchmark
	foreach (ComputeDevice* cDev, *ComputeDeviceManager.Avaliable.AllAvailableDevices) {
		foreach (Algorithm* algorithm, *cDev->GetAlgorithmSettings()) {
			algorithm->ClearBenchmarkPending();
			}
		}

	// save already benchmarked algorithms
	ConfigManager.CommitBenchmarks();
	// check devices without benchmarks
	foreach (ComputeDevice* cdev, *ComputeDeviceManager.Avaliable.AllAvailableDevices) {
		if (cdev->Enabled) {
			bool Enabled=false;
			foreach (Algorithm* algo, *cdev->GetAlgorithmSettings()) {
				if (algo->BenchmarkSpeed>0) {
					Enabled=true;
					break;
					}
				}
			cdev->Enabled=Enabled;
			}
		}
}

void Form_Benchmark::DevicesListView1_ItemSelectionChanged(int row)
{
	// show algorithms
	ComputeDevice* _selectedComputeDevice= row!=-1
		? ComputeDeviceManager.Avaliable.GetCurrentlySelectedComputeDevice(row, true)
		: ComputeDeviceManager.Avaliable.AllAvailableDevices->first()
		;
	algorithmsListView1->SetAlgorithms(_selectedComputeDevice, _selectedComputeDevice->Enabled);
}

void Form_Benchmark::RadioButton_SelectedUnbenchmarked_CheckedChanged_1()
{
	_algorithmOption=Enums::AlgorithmBenchmarkSettingsType::SelectedUnbenchmarkedAlgorithms;
	CalcBenchmarkDevicesAlgorithmQueue();
	devicesListViewEnableControl1->ResetListItemColors();
	algorithmsListView1->ResetListItemColors();
}

void Form_Benchmark::RadioButton_RE_SelectedUnbenchmarked_CheckedChanged()
{
	_algorithmOption=Enums::AlgorithmBenchmarkSettingsType::ReBenchSelectedAlgorithms;
	CalcBenchmarkDevicesAlgorithmQueue();
	devicesListViewEnableControl1->ResetListItemColors();
	algorithmsListView1->ResetListItemColors();
}

void Form_Benchmark::CheckBox_StartMiningAfterBenchmark_CheckedChanged()
{
	_StartMining=checkBox_StartMiningAfterBenchmark->isChecked();
}

void Form_Benchmark::closeEvent(QCloseEvent* ev)
{
	emit closing(ev);
}
