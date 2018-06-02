#include <hd.h>
#include <cstdio>

#include "config.h"
#include "Forms/Form_Main.h"
#include "Devices/ComputeDeviceManager.h"
#include "Configs/ConfigManager.h"
#include "Qt/NHMCashRegisterBitcoin.h"
#include "Forms/Components/DevicesListViewEnableControl.h"
#include "Configs/Data/GeneralConfig.h"
#include "Stats/ExchangeRateAPI.h"
#include "Globals.h"
#include "Forms/Form_Loading.h"
#include "Utils/Helpers.h"
#include "Miners/MinersSettingsManager.h"
#include "Forms/Form_3rdParty_TOS.h"
#include "Stats/NiceHashStats.h"
#include "Utils/MinersExistanceChecker.h"
#include "Utils/MinersDownloadManager.h"
#include "Miners/MinersManager.h"
#include "Forms/Components/GroupProfitControl.h"
#include "IFTTT.h"
#include "Forms/Form_Benchmark.h"
#include "Utils/BitcoinAddress.h"
#include "Forms/Form_Settings.h"
#include "Utils/MinersDownloader.h"
#include "International.h"
#include "Miners/Miner.h"
#include "Miners/Grouping/MiningDevice.h"
#include "Properties/Resources.h"
#include "Switching/NHSmaData.h"

#include <QCoreApplication>


Form_Main::Form_Main(QWidget *parent)
	: QMainWindow(parent)
{
	InitializeComponent();
	Resources resources;
	setWindowIcon(resources.logo);

	InitLocalization();

	ComputeDeviceManager.SystemSpecs.QueryAndLog();

	// Log the computer's amount of Total RAM and Page File Size
	hd_data_t* hd_data=(hd_data_t*)calloc(1, sizeof(*hd_data));
	hd_t* hd=hd_list(hd_data, hw_memory, 1, NULL);
	for (; hd; hd=hd->next) {
		uint64_t totalRam=hd->res->phys_mem.range;
		if (hd->res->phys_mem.type!=resource_types::res_phys_mem) {
			totalRam=hd->res->phys_mem.next->phys_mem.range;
			}
		Helpers::ConsolePrint("NICEHASH", "Total RAM: "+QString::number(totalRam>>20)+"MB");
		}

	//R=new Random
	qsrand(QTime::currentTime().msecsSinceStartOfDay());

	setWindowTitle(windowTitle()+" v"+qApp->applicationVersion()+BetaAlphaPostfixString);

	label_NotProfitable->setVisible(false);

	InitMainConfigGuiData();

	// for resizing
	InitFlowPanelStart();

	if (groupBox1->height()>0 && height()>0) {
		_emtpyGroupPanelHeight=groupBox1->height();
		_mainFormHeight=height()-_emtpyGroupPanelHeight;
		}
	else {
		_emtpyGroupPanelHeight=59;
		_mainFormHeight=330-_emtpyGroupPanelHeight;
		}
	ClearRatesALL();
}

void Form_Main::InitializeComponent()
{
	Resources resources;
	QFont f;
	f.setPointSize(5);
	setFont(f);

	buttonStartMining=new QPushButton(this);
	buttonStartMining->setObjectName("buttonStartMining");
	buttonStartMining->setGeometry(452, 162, 89, 23);
	buttonStartMining->setText("&Start");
	connect(buttonStartMining, SIGNAL(clicked()), this, SLOT(ButtonStartMining_Click()));

	textBoxBTCAddress=new QLineEdit(this);
	textBoxBTCAddress->setObjectName("textBoxBTCAddress");
	textBoxBTCAddress->setGeometry(113, 39, 237, 20);
	connect(textBoxBTCAddress, SIGNAL(editingFinished()), this, SLOT(TextBoxCheckBoxMain_Leave()));

	labelServiceLocation=new QLabel(this);
	labelServiceLocation->setObjectName("labelServiceLocation");
	labelServiceLocation->setGeometry(8, 15, 86, 13);
	labelServiceLocation->setText("Service location:");

	comboBoxLocation=new QComboBox(this);
	comboBoxLocation->setObjectName("comboBoxLocation");
	comboBoxLocation->setGeometry(113, 12, 121, 21);
	comboBoxLocation->addItems(QStringList() << "Europe - Amsterdam" << "USA - San Jose" << "China - Hong Kong" << "Japan - Tokyo" << "India - Chennai" << "Brazil - Sao Paulo");
	connect(comboBoxLocation, SIGNAL(currentIndexChanged(int)), this, SLOT(TextBoxCheckBoxMain_Leave()));

	labelBitcoinAddress=new QLabel(this);
	labelBitcoinAddress->setObjectName("labelBitcoinAddress");
	labelBitcoinAddress->setGeometry(8, 42, 82, 13);
	labelBitcoinAddress->setText("Bitcoin address:");

	statusStrip1=new QStatusBar(this);
	statusStrip1->setObjectName("statusStrip1");
	statusStrip1->setGeometry(0, 306, 565, 25);

	toolStripStatusLabelGlobalRateText=new QLabel(statusStrip1);
	toolStripStatusLabelGlobalRateText->setObjectName("toolStripStatusLabelGlobalRateText");
	toolStripStatusLabelGlobalRateText->setText("Global rate:");
	toolStripStatusLabelGlobalRateText->setFixedSize(67, 20);
	statusStrip1->addPermanentWidget(toolStripStatusLabelGlobalRateText);

	toolStripStatusLabelGlobalRateValue=new QLabel(statusStrip1);
	toolStripStatusLabelGlobalRateValue->setObjectName("toolStripStatusLabelGlobalRateValue");
	toolStripStatusLabelGlobalRateValue->setText("0.00000000");
	toolStripStatusLabelGlobalRateValue->setFixedSize(73, 20);
//	toolStripStatusLabelGlobalRateValue->setStyleSheet("QLabel { font: 9pt \"Segoe UI\"; font-weight: bold; }");
	toolStripStatusLabelGlobalRateValue->setStyleSheet("QLabel { font: \"Segoe UI\"; font-weight: bold; }");
	statusStrip1->addPermanentWidget(toolStripStatusLabelGlobalRateValue);

	toolStripStatusLabelBTCDayText=new QLabel(statusStrip1);
	toolStripStatusLabelBTCDayText->setObjectName("toolStripStatusLabelBTCDayText");
	toolStripStatusLabelBTCDayText->setText("BTC/Day");
	toolStripStatusLabelBTCDayText->setFixedSize(52, 20);
	statusStrip1->addPermanentWidget(toolStripStatusLabelBTCDayText);

	toolStripStatusLabelBTCDayValue=new QLabel(statusStrip1);
	toolStripStatusLabelBTCDayValue->setObjectName("toolStripStatusLabelBTCDayValue");
	toolStripStatusLabelBTCDayValue->setText("0.00");
	toolStripStatusLabelBTCDayValue->setFixedSize(31, 20);
//	toolStripStatusLabelBTCDayValue->setStyleSheet("QLabel { font: 9pt \"Segoe UI\"; font-weight: bold; }");
	toolStripStatusLabelBTCDayValue->setStyleSheet("QLabel { font: \"Segoe UI\"; font-weight: bold; }");
	statusStrip1->addPermanentWidget(toolStripStatusLabelBTCDayValue);

	toolStripStatusLabelBalanceText=new QLabel(statusStrip1);
	toolStripStatusLabelBalanceText->setObjectName("toolStripStatusLabelBalanceText");
	toolStripStatusLabelBalanceText->setText("$/Day     Balance:");
	toolStripStatusLabelBalanceText->setFixedSize(97, 20);
	statusStrip1->addPermanentWidget(toolStripStatusLabelBalanceText);

	toolStripStatusLabelBalanceBTCValue=new QLabel(statusStrip1);
	toolStripStatusLabelBalanceBTCValue->setObjectName("toolStripStatusLabelBalanceBTCValue");
	toolStripStatusLabelBalanceBTCValue->setText("0.00000000");
	toolStripStatusLabelBalanceBTCValue->setFixedSize(73, 20);
//	toolStripStatusLabelBalanceBTCValue->setStyleSheet("QLabel { font: 9pt \"Segoe UI\"; font-weight: bold; }");
	toolStripStatusLabelBalanceBTCValue->setStyleSheet("QLabel { font: \"Segoe UI\"; font-weight: bold; }");
	statusStrip1->addPermanentWidget(toolStripStatusLabelBalanceBTCValue);

	toolStripStatusLabelBalanceBTCCode=new QLabel(statusStrip1);
	toolStripStatusLabelBalanceBTCCode->setObjectName("toolStripStatusLabelBalanceBTCCode");
	toolStripStatusLabelBalanceBTCCode->setText("BTC");
	toolStripStatusLabelBalanceBTCCode->setFixedSize(27, 20);
	statusStrip1->addPermanentWidget(toolStripStatusLabelBalanceBTCCode);

	toolStripStatusLabelBalanceDollarText=new QLabel(statusStrip1);
	toolStripStatusLabelBalanceDollarText->setObjectName("toolStripStatusLabelBalanceDollarText");
	toolStripStatusLabelBalanceDollarText->setText("0.00");
	toolStripStatusLabelBalanceDollarText->setFixedSize(31, 20);
//	toolStripStatusLabelBalanceDollarText->setStyleSheet("QLabel { font: 9pt \"Segoe UI\"; font-weight: bold; }");
	toolStripStatusLabelBalanceDollarText->setStyleSheet("QLabel { font: \"Segoe UI\"; font-weight: bold; }");
	statusStrip1->addPermanentWidget(toolStripStatusLabelBalanceDollarText);

	toolStripStatusLabelBalanceDollarValue=new QLabel(statusStrip1);
	toolStripStatusLabelBalanceDollarValue->setObjectName("toolStripStatusLabelBalanceDollarValue");
	toolStripStatusLabelBalanceDollarValue->setText("$ ");
	toolStripStatusLabelBalanceDollarValue->setFixedSize(16, 20);
	statusStrip1->addPermanentWidget(toolStripStatusLabelBalanceDollarValue);

	toolStripStatusLabel10=new NHMCashRegisterBitcoin(statusStrip1);
	toolStripStatusLabel10->setObjectName("toolStripStatusLabel10");
	toolStripStatusLabel10->setFixedSize(35, 20);
	toolStripStatusLabel10->setCursor(Qt::PointingHandCursor);
	toolStripStatusLabel10->setPixmap(resources.NHM_Cash_Register_Bitcoin_transparent);
	connect(toolStripStatusLabel10, SIGNAL(clicked()), this, SLOT(ToolStripStatusLabel10_Click()));
	statusStrip1->addPermanentWidget(toolStripStatusLabel10);

	linkLabelCheckStats=new ClickableLabel(this);
	linkLabelCheckStats->setObjectName("linkLabelCheckStats");
	linkLabelCheckStats->setGeometry(175, 90, 113, 13);
	linkLabelCheckStats->setText("Check my stats online!");
	connect(linkLabelCheckStats, SIGNAL(clicked()), this, SLOT(LinkLabelCheckStats_LinkClicked()));

	labelWorkerName=new QLabel(this);
	labelWorkerName->setObjectName("labelWorkerName");
	labelWorkerName->setGeometry(8, 68, 74, 13);
	labelWorkerName->setText("Worker name:");

	textBoxWorkerName=new QLineEdit(this);
	textBoxWorkerName->setObjectName("textBoxWorkerName");
	textBoxWorkerName->setGeometry(113, 65, 120, 20);
	connect(textBoxWorkerName, SIGNAL(editingFinished()), this, SLOT(TextBoxCheckBoxMain_Leave()));

	buttonStopMining=new QPushButton(this);
	buttonStopMining->setObjectName("buttonStopMining");
	buttonStopMining->setGeometry(452, 188, 89, 23);
	buttonStopMining->setText("St&op");
	buttonStopMining->setEnabled(false);
	connect(buttonStopMining, SIGNAL(clicked()), this, SLOT(ButtonStopMining_Click()));

	buttonBenchmark=new QPushButton(this);
	buttonBenchmark->setObjectName("buttonBenchmark");
	buttonBenchmark->setGeometry(452, 109, 89, 23);
	buttonBenchmark->setText("&Benchmark");
	connect(buttonBenchmark, SIGNAL(clicked()), this, SLOT(ButtonBenchmark_Click()));

	buttonSettings=new QPushButton(this);
	buttonSettings->setObjectName("buttonSettings");
	buttonSettings->setGeometry(452, 135, 89, 23);
	buttonSettings->setText("S&ettings");
	connect(buttonSettings, SIGNAL(clicked()), this, SLOT(ButtonSettings_Click()));

	buttonLogo=new QPushButton(this);
	buttonLogo->setObjectName("buttonLogo");
	buttonLogo->setGeometry(355, 9, 205, 52);
	buttonLogo->setIcon(resources.NHM_logo_xsmall_light);
	buttonLogo->setIconSize(buttonLogo->size());
	buttonLogo->setCursor(Qt::PointingHandCursor);
	buttonLogo->setFlat(true);
	connect(buttonLogo, SIGNAL(clicked()), this, SLOT(ButtonLogo_Click()));

	linkLabelChooseBTCWallet=new ClickableLabel(this);
	linkLabelChooseBTCWallet->setObjectName("linkLabelChooseBTCWallet");
	linkLabelChooseBTCWallet->setGeometry(7, 90, 165, 13);
	linkLabelChooseBTCWallet->setText("Help me choose my Bitcoin wallet");
	connect(linkLabelChooseBTCWallet, SIGNAL(clicked()), this, SLOT(LinkLabelChooseBTCWallet_LinkClicked()));

	notifyIcon1=new QSystemTrayIcon(this);
	connect(notifyIcon1, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(NotifyIcon1_DoubleClick(QSystemTrayIcon::ActivationReason)));

	labelDemoMode=new QLabel(this);
	labelDemoMode->setObjectName("labelDemoMode");
	labelDemoMode->setGeometry(6, 39, 498, 25);
	labelDemoMode->setText("NiceHash QtMiner is running in DEMO mode!");
	labelDemoMode->setStyleSheet("QLabel { color: red; font: 15.25pt \"Sans Serif\"; }");
	labelDemoMode->setVisible(false);

	flowLayoutPanelRates=new QListWidget;
	flowLayoutPanelRates->setObjectName("flowLayoutPanelRates");
	flowLayoutPanelRates->setGeometry(3, 16, 545, 40);
	flowLayoutPanelRates->setLayout(new QVBoxLayout);

	groupBox1=new QGroupBox(this);
	groupBox1->setObjectName("groupBox1");
	groupBox1->setGeometry(9, 246, 551, 59);
	groupBox1->setTitle("Group/Device Rates:");
	groupBox1->setStyleSheet("QGroupBox { border: 1px solid; margin-top: 0.4em; } QGroupBox::title { top: -8px; left: 10px; }");
	flowLayoutPanelRates->setParent(groupBox1);

	label_NotProfitable=new QLabel(groupBox1);
	label_NotProfitable->setObjectName("label_NotProfitable");
	label_NotProfitable->setGeometry(6, 0, 366, 24);
	label_NotProfitable->setText("CURRENTLY MINING NOT PROFITABLE");
	label_NotProfitable->setStyleSheet("QLabel { font: 14.25pt \"Sans Serif\"; color: red; }");

	devicesListViewEnableControl1=new DevicesListViewEnableControl(this);
	devicesListViewEnableControl1->setObjectName("devicesListViewEnableControl1");
	devicesListViewEnableControl1->setGeometry(11, 109, 435, 129);
	devicesListViewEnableControl1->BenchmarkCalculation=nullptr;
//	devicesListViewEnableControl1->setFirstColumnText("Enabled");
	devicesListViewEnableControl1->setIsInBenchmark(false);
	devicesListViewEnableControl1->setIsMining(false);
	devicesListViewEnableControl1->SaveToGeneralConfig=false;

	buttonHelp=new QPushButton(this);
	buttonHelp->setObjectName("buttonHelp");
	buttonHelp->setGeometry(452, 215, 89, 23);
	buttonHelp->setText("&Help");
	connect(buttonHelp, SIGNAL(clicked()), this, SLOT(ButtonHelp_Click()));

	linkLabelNewVersion=new ClickableLabel(this);
	linkLabelNewVersion->setObjectName("linkLabelNewVersion");
	linkLabelNewVersion->setGeometry(292, 77, 0, 13);
	connect(linkLabelNewVersion, SIGNAL(clicked()), this, SLOT(LinkLabelNewVersion_LinkClicked()));

//	toolTip1=new QToolTip;

	setTabOrder(comboBoxLocation, textBoxBTCAddress);
	setTabOrder(textBoxBTCAddress, textBoxWorkerName);
	setTabOrder(textBoxWorkerName, buttonBenchmark);
	setTabOrder(buttonBenchmark, buttonSettings);
	setTabOrder(buttonSettings, buttonStartMining);
	setTabOrder(buttonStartMining, buttonStopMining);
	setTabOrder(buttonStopMining, buttonHelp);
	setTabOrder(buttonHelp, linkLabelCheckStats);
	setTabOrder(linkLabelCheckStats, linkLabelChooseBTCWallet);
	setTabOrder(linkLabelChooseBTCWallet, buttonLogo);
	setTabOrder(buttonLogo, devicesListViewEnableControl1);
	setTabOrder(devicesListViewEnableControl1, linkLabelNewVersion);

	setEnabled(false);
	setMinimumSize(576, 316);
	setObjectName("Form_Main");
	setWindowTitle("NiceHash QtMiner");
	// closeEvent(QCloseEvent* ev)
	// showEvent(QShowEvent* ev)
	// changeEvent(QEvent* ev)
	setGeometry(
		QStyle::alignedRect(
			Qt::LeftToRight,
			Qt::AlignCenter,
			size(),
			qApp->desktop()->availableGeometry()
			)
		);
}

void Form_Main::InitLocalization()
{
	//MessageBoxManager...

	labelServiceLocation->setText(International::GetText("Service_Location")+":");
	{
		int i=0;
		foreach (QString loc, Globals::MiningLocation) {
			comboBoxLocation->setItemText(i, International::GetText(QString("LocationName_%1").arg(loc).toStdString().c_str()));
			i++;
			}
	}
	labelBitcoinAddress->setText(International::GetText("BitcoinAddress")+":");
	labelWorkerName->setText(International::GetText("WorkerName")+":");

	linkLabelCheckStats->setText(International::GetText("Form_Main_check_stats"));
	linkLabelChooseBTCWallet->setText(International::GetText("Form_Main_choose_bitcoin_wallet"));

	toolStripStatusLabelGlobalRateText->setText(International::GetText("Form_Main_global_rate")+":");
	toolStripStatusLabelBTCDayText->setText("BTC/"+International::GetText(QMetaEnum::fromType<Enums::TimeUnitType>().valueToKey((int)ConfigManager.generalConfig->TimeUnit)));
	toolStripStatusLabelBalanceText->setText(ExchangeRateApi::ActiveDisplayCurrency+"/"+International::GetText(QMetaEnum::fromType<Enums::TimeUnitType>().valueToKey((int)ConfigManager.generalConfig->TimeUnit))+"     "+International::GetText("Form_Main_balance")+":");

	devicesListViewEnableControl1->InitLocale();

	buttonBenchmark->setText(International::GetText("Form_Main_benchmark"));
	buttonSettings->setText(International::GetText("Form_Main_settings"));
	buttonStartMining->setText(International::GetText("Form_Main_start"));
	buttonStopMining->setText(International::GetText("Form_Main_stop"));
	buttonHelp->setText(International::GetText("Form_Main_help"));

	label_NotProfitable->setText(International::GetText("Form_Main_MINING_NOT_PROFITABLE"));
	groupBox1->setTitle(International::GetText("Form_Main_Group_Device_Rates"));
}

void Form_Main::InitMainConfigGuiData()
{
	if (ConfigManager.generalConfig->ServiceLocation>=0
		&& ConfigManager.generalConfig->ServiceLocation<Globals::MiningLocation.size()
		) {
		comboBoxLocation->setCurrentIndex(ConfigManager.generalConfig->ServiceLocation);
		}
	else {
		comboBoxLocation->setCurrentIndex(0);
		}
	textBoxBTCAddress->setText(ConfigManager.generalConfig->BitcoinAddress);
	textBoxWorkerName->setText(ConfigManager.generalConfig->WorkerName);

	_showWarningNiceHashData=true;
	_demoMode=false;

	// init active display currency after config load
	ExchangeRateApi::ActiveDisplayCurrency=ConfigManager.generalConfig->DisplayCurrency;

	// init factor for Time Unit
	switch (ConfigManager.generalConfig->TimeUnit) {
		case Enums::TimeUnitType::Hour:
			_factorTimeUnit=1.0/24.0;
			break;
		case Enums::TimeUnitType::Day:
			_factorTimeUnit=1;
			break;
		case Enums::TimeUnitType::Week:
			_factorTimeUnit=7;
			break;
		case Enums::TimeUnitType::Month:
			_factorTimeUnit=30;
			break;
		case Enums::TimeUnitType::Year:
			_factorTimeUnit=365;
			break;
		}

	toolStripStatusLabelBalanceDollarValue->setText("("+ExchangeRateApi::ActiveDisplayCurrency+")");
	toolStripStatusLabelBalanceText->setText(ExchangeRateApi::ActiveDisplayCurrency+"/"+International::GetText(QMetaEnum::fromType<Enums::TimeUnitType>().valueToKey((int)ConfigManager.generalConfig->TimeUnit))+"     "+International::GetText("Form_Main_balance")+":");
	BalanceCallback(); // update currency changes

	if (_isDeviceDetectionInitialized) {
		devicesListViewEnableControl1->ResetComputeDevices(ComputeDeviceManager.Available.Devices);
		}
}

void Form_Main::AfterLoadComplete()
{
	if (_loadingScreen!=nullptr) {delete _loadingScreen;}
	_loadingScreen=nullptr;
	setEnabled(true);

	_idleCheck=new QTimer(this);
	connect(_idleCheck, SIGNAL(timeout()), this, SLOT(IdleCheck_Tick()));
	_idleCheck->start(500);
}

void Form_Main::IdleCheck_Tick()
{
	if (!ConfigManager.generalConfig->StartMiningWhenIdle || _isManuallyStarted) {
		return;
		}
	int msIdle=Helpers::GetIdleTime();
	if (_minerStatsCheck->isActive()) {
		if (msIdle<(ConfigManager.generalConfig->MinIdleSeconds*1000)) {
			StopMining();
			Helpers::ConsolePrint("NICEHASH", "Resumed from idling");
			}
		}
	else {
		if (_benchmarkForm==nullptr && (msIdle>(ConfigManager.generalConfig->MinIdleSeconds*1000))) {
			Helpers::ConsolePrint("NICEHASH", "Entering idling state");
			if (StartMining(false)!=StartMiningReturnType::StartMining) {
				StopMining();
				}
			}
		}
}

// This is a single shot _benchmarkTimer
void Form_Main::StartupTimer_Tick()
{
	_startupTimer->stop();
	delete _startupTimer;
	_startupTimer=nullptr;

	// Internals Init
	// TODO add loading step
	MinersSettingsManager.Init();

	if (!Helpers::Is64BitOperatingSystem()) {
		QMessageBox::warning(this, International::GetText("Warning_with_Exclamation"), International::GetText("Form_Main_x64_Support_Only"), QMessageBox::Ok);
		close();
		return;
		}

	// 3rdparty miners check scope #1
	{
	// check if settings set
	if (ConfigManager.generalConfig->Use3rdPartyMiners==Enums::Use3rdPartyMiners::NOT_SET) {
		// show TOS
		Form_3rdParty_TOS* tos=new Form_3rdParty_TOS(this);
//		Qt::WindowFlags f=tos->windowFlags();
		tos->setWindowFlag(Qt::WindowContextHelpButtonHint, false);
		tos->setWindowFlag(Qt::WindowMinMaxButtonsHint, false);
		tos->exec();
		delete tos;
		}
	}

	// Query Available ComputeDevices
	ComputeDeviceManager.Query.QueryDevices(_loadingScreen);
	_isDeviceDetectionInitialized=true;

	/////////////////////////////////////////////
	/////// from here on we have our devices and Miners initialized
	ConfigManager.AfterDeviceQueryInitialization();
	_loadingScreen->IncreaseLoadCounterAndMessage(International::GetText("Form_Main_loadtext_SaveConfig"));

	// All devices settup should be initialized in AllDevices
	devicesListViewEnableControl1->ResetComputeDevices(ComputeDeviceManager.Available.Devices);
	// set properties after
	devicesListViewEnableControl1->SaveToGeneralConfig=true;

	_loadingScreen->IncreaseLoadCounterAndMessage(International::GetText("Form_Main_loadtext_CheckLatestVersion"));

	_minerStatsCheck=new QTimer(this);
	connect(_minerStatsCheck, SIGNAL(timeout()), this, SLOT(MinerStatsCheck_Tick()));
	_minerStatsCheck->setInterval(ConfigManager.generalConfig->MinerAPIQueryInterval*1000);

//	_smaMinerCheck=new QTimer(this);
//	connect(_smaMinerCheck, SIGNAL(timeout()), this, SLOT(SMAMinerCheck_Tick()));
//	_smaMinerCheck->setInterval(ConfigManager.generalConfig->SwitchMinSecondsFixed*1000 + (qrand() % (ConfigManager.generalConfig->SwitchMinSecondsDynamic*1000)));
//#if WITH_AMD
//	if (ComputeDeviceManager.Group.ContainsAmdGpus()) {
//		_smaMinerCheck->setInterval((ConfigManager.generalConfig->SwitchMinSecondsAMD+ConfigManager.generalConfig->SwitchMinSecondsFixed)*1000 + (qrand() % (ConfigManager.generalConfig->SwitchMinSecondsDynamic*1000)));
//		}
//#endif

	_loadingScreen->IncreaseLoadCounterAndMessage(International::GetText("Form_Main_loadtext_GetNiceHashSMA"));
	// Init ws connection
	connect(&NiceHashStats, SIGNAL(BalanceUpdate(double)), this, SLOT(BalanceCallback()));
	connect(&NiceHashStats, SIGNAL(SmaUpdate()), this, SLOT(SmaCallback()));
	connect(&NiceHashStats, SIGNAL(VersionUpdate(QString)), this, SLOT(VersionUpdateCallback()));
	connect(&NiceHashStats, SIGNAL(ConnectionLost()), this, SLOT(ConnectionLostCallback()));
	connect(&NiceHashStats, SIGNAL(ConnectionEstablished()), this, SLOT(ConnectionEstablishedCallback()));
	connect(&NiceHashStats, SIGNAL(VersionBurn(QString)), this, SLOT(VersionBurnCallback()));
	connect(&NiceHashStats, SIGNAL(ExchangeUpdate()), this, SLOT(ExchangeCallback()));
	NiceHashStats.StartConnection(Links::NhmSocketAddress);

	// increase timeout
	if (Globals::IsFirstNetworkCheckTimeout) {
		while (!Helpers::WebRequestTestGoogle() && Globals::FirstNetworkCheckTimeoutTries>0) {
			--Globals::FirstNetworkCheckTimeoutTries;
			}
		}

	_loadingScreen->IncreaseLoadCounterAndMessage(International::GetText("Form_Main_loadtext_GetBTCRate"));

/*	_bitcoinExchangeCheck=new QTimer(this);
	connect(_bitcoinExchangeCheck, SIGNAL(timeout()), this, SLOT(BitcoinExchangeCheck_Tick()));
	_bitcoinExchangeCheck->start(1000*3601); // every 1h and 1s
	BitcoinExchangeCheck_Tick();*/

	_loadingScreen->IncreaseLoadCounterAndMessage(International::GetText("Form_Main_loadtext_SetEnvironmentVariable"));
	Helpers::SetDefaultEnvironmentVariables();

	_loadingScreen->IncreaseLoadCounterAndMessage(International::GetText("Form_Main_loadtext_SetWindowsErrorReporting"));

//	Helpers::DisableWindowsErrorReporting(ConfigManager.generalConfig->DisableWindowsErrorReporting);

	_loadingScreen->IncreaseLoadCounter();
#if WITH_NVIDIA
	if (ConfigManager.generalConfig->NVIDIAP0State) {
		_loadingScreen->SetInfoMsg(International::GetText("Form_Main_loadtext_NVIDIAP0State"));
		Helpers::SetNvidiaP0State();
		}
#endif

	_loadingScreen->FinishLoad();

//	bool runVCRed=!MinersExistanceChecker::IsMinersBinsInit() && !ConfigManager.generalConfig->DownloadInit;
	// standard miners check scope
	{
		// check if download needed
		if (!MinersExistanceChecker::IsMinersBinsInit() && !ConfigManager.generalConfig->DownloadInit) {
			Form_Loading* downloadUnzipForm=new Form_Loading(new MinersDownloader(MinersDownloadManager::StandardDlSetup), this);
			SetChildFormCenter(downloadUnzipForm);
			downloadUnzipForm->show();
			}
		// check if files are mising
		if (!MinersExistanceChecker::IsMinersBinsInit()) {
			QMessageBox::StandardButton result=QMessageBox::warning(this, International::GetText("Warning_with_Exclamation"), International::GetText("Form_Main_bins_folder_files_missing"), QMessageBox::Yes|QMessageBox::No);
			if (result==QMessageBox::Yes) {
				ConfigManager.generalConfig->DownloadInit=false;
				ConfigManager.GeneralConfigFileCommit();
				QProcess* pHandle=new QProcess;
				pHandle->setProgram(qApp->applicationFilePath());
				pHandle->startDetached();
				close();
				return;
				}
			}
		else if (!ConfigManager.generalConfig->DownloadInit) {
			// all good
			ConfigManager.generalConfig->DownloadInit=true;
			ConfigManager.GeneralConfigFileCommit();
			}
	}
	// 3rdparty miners check scope #2
	{
		// check if download needed
		if (ConfigManager.generalConfig->Use3rdPartyMiners==Enums::Use3rdPartyMiners::YES) {
			if (!MinersExistanceChecker::IsMiners3rdPartyBinsInit() && !ConfigManager.generalConfig->DownloadInit3rdParty) {
				Form_Loading* download3rdPartyUnzipForm=new Form_Loading(new MinersDownloader(MinersDownloadManager::ThirdPartyDlSetup), this);
				SetChildFormCenter(download3rdPartyUnzipForm);
				download3rdPartyUnzipForm->show();
				}
			// check if files are mising
			if (!MinersExistanceChecker::IsMiners3rdPartyBinsInit()) {
				QMessageBox::StandardButton result=QMessageBox::warning(this, International::GetText("Warning_with_Exclamation"), International::GetText("Form_Main_bins_folder_files_missing"), QMessageBox::Yes|QMessageBox::No);
				if (result==QMessageBox::Yes) {
					ConfigManager.generalConfig->DownloadInit3rdParty=false;
					ConfigManager.GeneralConfigFileCommit();
					QProcess* pHandle=new QProcess;
					pHandle->setProgram(qApp->applicationFilePath());
					pHandle->startDetached();
					close();
					return;
					}
				}
			else if (!ConfigManager.generalConfig->DownloadInit3rdParty) {
				// all good
				ConfigManager.generalConfig->DownloadInit3rdParty=true;
				ConfigManager.GeneralConfigFileCommit();
				}
			}
	}
/*
	if (runVCRed) {
		Helpers::InstallVcRedist();
		}
*/
	if (ConfigManager.generalConfig->AutoStartMining) {
		// well this is started manually as we want it to start at runtime
		_isManuallyStarted=true;
		if (StartMining(false)!=StartMiningReturnType::StartMining) {
			_isManuallyStarted=false;
			StopMining();
			}
		}
}

void Form_Main::SetChildFormCenter(QDialog* form) //done
{
	form->move(x()+(width()-form->width())/2, y()+(height()-form->height())/2);
}

void Form_Main::Form_Main_Shown()
{
	// general loading indicator
	int TotalLoadSteps=11;
	_loadingScreen=new Form_Loading(this, International::GetText("Form_Loading_label_LoadingText"), International::GetText("Form_Main_loadtext_CPU"), TotalLoadSteps, this);
	SetChildFormCenter(_loadingScreen);
	_loadingScreen->show();

	_startupTimer=new QTimer(this);
	connect(_startupTimer, SIGNAL(timeout()), this, SLOT(StartupTimer_Tick()));
	_startupTimer->start(200);
}
/*
void Form_Main::SMAMinerCheck_Tick()
{
	_smaMinerCheck->setInterval(ConfigManager.generalConfig->SwitchMinSecondsFixed*1000 + (qrand() % (ConfigManager.generalConfig->SwitchMinSecondsDynamic*1000)));
#if WITH_AMD
	if (ComputeDeviceManager.Group.ContainsAmdGpus()) {
		_smaMinerCheck->setInterval((ConfigManager.generalConfig->SwitchMinSecondsAMD+ConfigManager.generalConfig->SwitchMinSecondsFixed)*1000 + (qrand() % (ConfigManager.generalConfig->SwitchMinSecondsDynamic*1000)));
		}
#endif
#if (SWITCH_TESTING)
	_smaMinerCheck->setInterval(MiningDevice::SMAMinerCheckInterval);
#endif
	if (_isSmaUpdated) { // Don't bother checking for new profits unless SMA has changed
		_isSmaUpdated=false;
		MinersManager::SwitchMostProfitableGroupUpMethod(Globals::NiceHashData);
		}
}*/

void Form_Main::MinerStatsCheck_Tick()
{
	MinersManager::MinerStatsCheck();
}

void Form_Main::ComputeDevicesCheckTimer_Tick()
{
#if WITH_NVIDIA
	if (ComputeDeviceManager.Query.CheckVideoControllersCountMismath()) {
		// less GPUs than before, ACT!
		try {
			QProcess::execute(QDir::currentPath()+"/OnGPUsLost.sh");
			}
		catch (QException& ex) {
			Helpers::ConsolePrint("NICEHASH", QString("OnGPUsMismatch.sh error: ")+ex.what());
			}
		}
#endif
}

void Form_Main::InitFlowPanelStart()
{
	QListWidgetItem* i=nullptr;
	while (i=flowLayoutPanelRates->takeItem(0)) {
		delete i;
		}
	// add for every cdev a
	foreach (ComputeDevice* cdev, *ComputeDeviceManager.Available.Devices) {
		if (cdev->Enabled) {
			GroupProfitControl* newGroupProfitControl=new GroupProfitControl(flowLayoutPanelRates);
//			newGroupProfitControl->setHidden(true);
			QListWidgetItem* item=new QListWidgetItem(flowLayoutPanelRates);
			flowLayoutPanelRates->addItem(item);
			flowLayoutPanelRates->setItemWidget(item, newGroupProfitControl);
			flowLayoutPanelRates->setItemHidden(item, true);
			}
		}
}

void Form_Main::ClearRatesALL()
{
	HideNotProfitable();
	ClearRates(-1);
}

void Form_Main::ClearRates(int groupCount)
{
//	if (InvokeRequired) {
//		}
	if (_flowLayoutPanelVisibleCount!=groupCount) {
		_flowLayoutPanelVisibleCount=groupCount;
		// hide some Controls
		int hideIndex=0;
		for (int i=0; i<flowLayoutPanelRates->count(); i++) {
			flowLayoutPanelRates->setItemHidden(flowLayoutPanelRates->item(i), !(hideIndex<groupCount));
			++hideIndex;
			}
		}
	_flowLayoutPanelRatesIndex=0;
	int visibleGroupCount=1;
	if (groupCount>0) {
		visibleGroupCount+=groupCount;
		}
	int groupBox1Height=_emtpyGroupPanelHeight;
	if (flowLayoutPanelRates->count()) {
		QWidget* control=flowLayoutPanelRates->itemWidget(flowLayoutPanelRates->item(0));
		float panelHeight=control->height()*1.2f;
		groupBox1Height=(int)(visibleGroupCount*panelHeight);
		}
	groupBox1->resize(groupBox1->width(), groupBox1Height);
	flowLayoutPanelRates->resize(flowLayoutPanelRates->width(), groupBox1Height-6);
	statusStrip1->move(statusStrip1->x(), groupBox1->y()+groupBox1->height()+1);
	// set new height
	QSize s=size();
	s.setHeight(_mainFormHeight+groupBox1Height);
	resize(s);
}

void Form_Main::AddRateInfo(QString groupName, QString deviceStringInfo, ApiData* iAPIData, double paying, bool isApiGetException)
{
	QString apiGetExceptionString= isApiGetException
		? "**"
		: "";

	QString speedString= Helpers::FormatDualSpeedOutput(iAPIData->Speed, iAPIData->SecondarySpeed, iAPIData->AlgorithmID) + iAPIData->AlgorithmName + apiGetExceptionString;
	QString rateBTCString=FormatPayingOutput(paying);
	QString rateCurrencyString=QString::number(ExchangeRateApi::ConvertToActiveCurrency(paying*ExchangeRateApi::GetUsdExchangeRate()*_factorTimeUnit), 'f', 2)+QString(" %1/").arg(ExchangeRateApi::ActiveDisplayCurrency)+International::GetText(QMetaEnum::fromType<Enums::TimeUnitType>().valueToKey((int)ConfigManager.generalConfig->TimeUnit));

	try {
		((GroupProfitControl*)flowLayoutPanelRates->itemWidget(flowLayoutPanelRates->item(_flowLayoutPanelRatesIndex++)))->UpdateProfitStats(groupName, deviceStringInfo, speedString, rateBTCString, rateCurrencyString);
		}
	catch (...) {}

	UpdateGlobalRate();
}

void Form_Main::ShowNotProfitable(QString msg)
{
	if (ConfigManager.generalConfig->UseIFTTT) {
		if (!_isNotProfitable) {
			Ifttt::PostToIfttt("nicehash", msg);
			_isNotProfitable=true;
			}
		}

//	if (InvokeRequired) {
//		}
	label_NotProfitable->setVisible(true);
	label_NotProfitable->setText(msg);
	label_NotProfitable->repaint();
}

void Form_Main::HideNotProfitable()
{
	if (ConfigManager.generalConfig->UseIFTTT) {
		if (_isNotProfitable) {
			Ifttt::PostToIfttt("nicehash", "Mining is once again profitable and has resumed.");
			_isNotProfitable=false;
			}
		}

//	if (InvokeRequired) {
//		}
	label_NotProfitable->setVisible(false);
	label_NotProfitable->repaint();
}

void Form_Main::ForceMinerStatsUpdate()
{
	try {
		QMetaObject::invokeMethod(this, "MinerStatsCheck_Tick");
		}
	catch (std::exception& e) {
		Helpers::ConsolePrint("NiceHash", e.what());
		}
}

void Form_Main::UpdateGlobalRate()
{
	double totalRate=MinersManager::GetTotalRate();

	if (ConfigManager.generalConfig->AutoScaleBTCValues && totalRate<0.1) {
		toolStripStatusLabelBTCDayText->setText("mBTC/"+International::GetText(QMetaEnum::fromType<Enums::TimeUnitType>().valueToKey((int)ConfigManager.generalConfig->TimeUnit)));
		toolStripStatusLabelGlobalRateValue->setText(QString::number((totalRate*1000*_factorTimeUnit), 'f', 5));
		}
	else {
		toolStripStatusLabelBTCDayText->setText("BTC/"+International::GetText(QMetaEnum::fromType<Enums::TimeUnitType>().valueToKey((int)ConfigManager.generalConfig->TimeUnit)));
		toolStripStatusLabelGlobalRateValue->setText(QString::number(totalRate*_factorTimeUnit, 'f', 6));
		}
	toolStripStatusLabelBTCDayValue->setText(QString::number(ExchangeRateApi::ConvertToActiveCurrency(totalRate*_factorTimeUnit*ExchangeRateApi::GetUsdExchangeRate()), 'f', 2));
	toolStripStatusLabelBalanceText->setText(ExchangeRateApi::ActiveDisplayCurrency+"/"+International::GetText(QMetaEnum::fromType<Enums::TimeUnitType>().valueToKey((int)ConfigManager.generalConfig->TimeUnit))+"     "+International::GetText("Form_Main_balance")+":");
}

void Form_Main::BalanceCallback()
{
	Helpers::ConsolePrint("NICEHASH", "Balance update");
	double balance=NiceHashStats.Balance();
	if (balance>0) {
		if (ConfigManager.generalConfig->AutoScaleBTCValues && balance<0.1) {
			toolStripStatusLabelBalanceBTCCode->setText("mBTC");
			toolStripStatusLabelBalanceBTCValue->setText(QString::number(balance*1000, 'f', 5));
			}
		else {
			toolStripStatusLabelBalanceBTCCode->setText("BTC");
			toolStripStatusLabelBalanceBTCValue->setText(QString::number(balance, 'f', 6));
			}
		double amount=(balance*ExchangeRateApi::GetUsdExchangeRate());
		amount=ExchangeRateApi::ConvertToActiveCurrency(amount);
		toolStripStatusLabelBalanceDollarText->setText(QString::number(amount, 'f', 2));
		toolStripStatusLabelBalanceDollarValue->setText(QString("(%1)").arg(ExchangeRateApi::ActiveDisplayCurrency));
		}
}
/*void Form_Main::BitcoinExchangeCheck_Tick()
{
	Helpers::ConsolePrint("NICEHASH", "Bitcoin rate get");
	ExchangeRateApi.UpdateApi(textBoxWorkerName->text().trimmed());
	double br=ExchangeRateApi.GetUsdExchangeRate();
	QString currencyRate=International::GetText("BenchmarkRatioRateN_A");
	if (br>0) {
		Globals::BitcoinUsdRate=br;
		currencyRate=QString::number(ExchangeRateApi.ConvertToActiveCurrency(br), 'f', 2);
		}

//	toolTip1.
	Helpers::ConsolePrint("NICEHASH", "Current Bitcoin rate: "+QString::number(Globals::BitcoinUsdRate, 'f', 2));
}*/

void Form_Main::ExchangeCallback()
{
	UpdateExchange();
}

void Form_Main::UpdateExchange()
{
	double br=ExchangeRateApi::GetUsdExchangeRate();
	QString currencyRate=International::GetText("BenchmarkRatioRateN_A");
	if (br>0) {
		currencyRate=QString::number(ExchangeRateApi::ConvertToActiveCurrency(br), 'f', 2);
		}
	statusStrip1->setToolTip(QString("1 BTC = %1 %2").arg(currencyRate).arg(ExchangeRateApi::ActiveDisplayCurrency));

	Helpers::ConsolePrint("NICEHASH", "Current Bitcoin rate: "+QString::number(br, 'f', 2));
}

void Form_Main::SmaCallback()
{
	Helpers::ConsolePrint("NICEHASH", "SMA update");
//	_isSmaUpdated=true;
}

void Form_Main::VersionBurnCallback() // @todo
{
	StopMining();
	if (_benchmarkForm!=nullptr) {
		_benchmarkForm->StopBenchmark();
		QMessageBox::critical(this, International::GetText("Error_with_Exclamation"), "", QMessageBox::Ok);
		qApp->exit();
		}
}

void Form_Main::ConnectionLostCallback()
{
	if (!NHSmaData::HasData() && ConfigManager.generalConfig->ShowInternetConnectionWarning && _showWarningNiceHashData) {
		_showWarningNiceHashData=false;
		QMessageBox::StandardButton result=QMessageBox::critical(this, International::GetText("Form_Main_msgbox_NoInternetTitle"), International::GetText("Form_Main_msgbox_NoInternetMsg"), QMessageBox::Yes|QMessageBox::No);
		if (result==QMessageBox::Yes) {
			return;
			}
		qApp->exit();
		}
}

void Form_Main::ConnectionEstablishedCallback()
{
	// send credentials
	NiceHashStats.SetCredentials(textBoxBTCAddress->text().trimmed(), textBoxWorkerName->text().trimmed());
}

void Form_Main::VersionUpdateCallback()
{
	QString ver=NiceHashStats.Version();
	if (ver.isEmpty()) {
		return;
		}
	QVersionNumber programVersion=QVersionNumber::fromString(qApp->applicationVersion());
	QVersionNumber onlineVersion=QVersionNumber::fromString(ver);
	int ret=QVersionNumber::compare(programVersion, onlineVersion);
	if (ret<0 || (!ret && BetaAlphaPostfixString!="")) {
		SetVersionLabel(QString(International::GetText("Form_Main_new_version_released")).arg(ver));
		_visitUrlNew=Links::VisitUrlNew+ver;
		}
}

void Form_Main::SetVersionLabel(QString text) // @todo
{/*
	if (linkLabelNewVersion) {
		}
	else {
		}*/
	linkLabelNewVersion->setText(text);
}

bool Form_Main::VerifyMiningAddress(bool showError)
{
	if (!BitcoinAddress::ValidateBitcoinAddress(textBoxBTCAddress->text().trimmed()) && showError) {
		QMessageBox::StandardButton result=QMessageBox::critical(this, International::GetText("Error_with_Exclamation"), International::GetText("Form_Main_msgbox_InvalidBTCAddressMsg"), QMessageBox::Yes|QMessageBox::No);
		if (result==QMessageBox::Yes) {
			QDesktopServices::openUrl(QUrl(Links::NhmBtcWalletFaq));
			}
		textBoxBTCAddress->setFocus();
		return false;
		}
	if (!BitcoinAddress::ValidateWorkerName(textBoxWorkerName->text().trimmed()) && showError) {
		QMessageBox::critical(this, International::GetText("Error_with_Exclamation"), International::GetText("Form_Main_msgbox_InvalidWorkerNameMsg"), QMessageBox::Ok);
		textBoxWorkerName->setFocus();
		return false;
		}
	return true;
}

void Form_Main::LinkLabelCheckStats_LinkClicked() //done
{
	if (!VerifyMiningAddress(true)) {
		return;
		}
	QDesktopServices::openUrl(QUrl(Links::CheckStats+textBoxBTCAddress->text().trimmed()));
}

void Form_Main::LinkLabelChooseBTCWallet_LinkClicked() //done
{
	QDesktopServices::openUrl(QUrl(Links::NhmBtcWalletFaq));
}

void Form_Main::LinkLabelNewVersion_LinkClicked() //done
{
	QDesktopServices::openUrl(QUrl(Links::VisitUrlNew));
}

void Form_Main::Form1_FormClosing() //done
{
	MinersManager::StopAllMiners();
//	MessageBoxManager.Unregister();
}

void Form_Main::ButtonBenchmark_Click() //done
{
	ConfigManager.generalConfig->ServiceLocation=comboBoxLocation->currentIndex();

	_benchmarkForm=new Form_Benchmark(this);
	SetChildFormCenter(_benchmarkForm);
	_benchmarkForm->exec();
	bool startMining=_benchmarkForm->StartMining();
	delete _benchmarkForm;
	_benchmarkForm=nullptr;

	InitMainConfigGuiData();
	if (startMining) {
		ButtonStartMining_Click();
		}
}

void Form_Main::ButtonSettings_Click() //done
{
	Form_Settings settings(this);
	SetChildFormCenter(&settings);
	settings.exec();

	if (settings.IsChange() && settings.IsChangeSaved() && settings.IsRestartNeeded()) {
		QMessageBox::information(this, International::GetText("Form_Main_Restart_Required_Title"), International::GetText("Form_Main_Restart_Required_Msg"), QMessageBox::Ok);
		QProcess::startDetached(qApp->applicationFilePath());
		close();
		}
	else if (settings.IsChange() && settings.IsChangeSaved()) {
		InitLocalization();
		InitMainConfigGuiData();
		}
}

void Form_Main::ButtonStartMining_Click() //done
{
	_isManuallyStarted=true;
	if (StartMining(true)==StartMiningReturnType::ShowNoMining) {
		_isManuallyStarted=false;
		StopMining();
		QMessageBox::warning(this, International::GetText("Warning_with_Exclamation"), International::GetText("Form_Main_StartMiningReturnedFalse"), QMessageBox::Ok);
		}
}

void Form_Main::ButtonStopMining_Click() //done
{
	_isManuallyStarted=false;
	StopMining();
}

QString Form_Main::FormatPayingOutput(double paying)
{
	if (ConfigManager.generalConfig->AutoScaleBTCValues && paying<0.1) {
		return QString::number(paying*1000*_factorTimeUnit, 'f', 5)+" mBTC/"+International::GetText(QMetaEnum::fromType<Enums::TimeUnitType>().valueToKey((int)ConfigManager.generalConfig->TimeUnit));
		}
	return QString::number(paying*_factorTimeUnit, 'f', 6)+" BTC/"+International::GetText(QMetaEnum::fromType<Enums::TimeUnitType>().valueToKey((int)ConfigManager.generalConfig->TimeUnit));
}

void Form_Main::ButtonLogo_Click() //done
{
	QDesktopServices::openUrl(QUrl(Links::VisitUrl));
}

void Form_Main::ButtonHelp_Click() //done
{
	QDesktopServices::openUrl(QUrl(Links::NHQTM_Help));
}

void Form_Main::ToolStripStatusLabel10_Click()
{
	QDesktopServices::openUrl(QUrl(Links::NhmPayingFaq));
}

void Form_Main::TextBoxCheckBoxMain_Leave() //done
{
	if (VerifyMiningAddress(false)) {
		if (ConfigManager.generalConfig->BitcoinAddress!=textBoxBTCAddress->text().trimmed()
			|| ConfigManager.generalConfig->WorkerName!=textBoxWorkerName->text().trimmed()
			) {
			// Reset credentials
			NiceHashStats.SetCredentials(textBoxBTCAddress->text().trimmed(), textBoxWorkerName->text().trimmed());
			}
		// Commit to config.json
		ConfigManager.generalConfig->BitcoinAddress=textBoxBTCAddress->text().trimmed();
		ConfigManager.generalConfig->WorkerName=textBoxWorkerName->text().trimmed();
		ConfigManager.generalConfig->ServiceLocation=comboBoxLocation->currentIndex();
		ConfigManager.GeneralConfigFileCommit();
		}
}

// Minimize to system tray if MinimizeToTray is set to true
void Form_Main::Form1_Resize() //done
{
	Resources resources;
	notifyIcon1->setIcon(resources.logo);
	notifyIcon1->setToolTip(qApp->applicationName()+" v"+qApp->applicationVersion()+"\nDouble-click to restore..");

	if (ConfigManager.generalConfig->MinimizeToTray
		&& QSystemTrayIcon::isSystemTrayAvailable()
		&& windowState().testFlag(Qt::WindowMinimized)
		) {
		notifyIcon1->setVisible(true);
		hide();
		}
}

// Restore miner from the system tray
void Form_Main::NotifyIcon1_DoubleClick(QSystemTrayIcon::ActivationReason reason) //done
{
	if (reason==QSystemTrayIcon::ActivationReason::DoubleClick
		|| reason==QSystemTrayIcon::ActivationReason::Trigger // 2click not working
		) {
		show();
		setWindowState(Qt::WindowState::WindowActive); // normal state ?
		notifyIcon1->setVisible(false);
		}
}

Form_Main::StartMiningReturnType Form_Main::StartMining(bool showWarnings)
{
	if (textBoxBTCAddress->text()=="") {
		if (showWarnings) {
			QMessageBox::StandardButton result=QMessageBox::warning(this, International::GetText("Form_Main_DemoModeTitle"), International::GetText("Form_Main_DemoModeMsg"), QMessageBox::Yes|QMessageBox::No);
			if (result==QMessageBox::Yes) {
				_demoMode=true;
				labelDemoMode->show();
				labelDemoMode->setText(International::GetText("Form_Main_DemoModeLabel"));
				}
			else {
				return StartMiningReturnType::IgnoreMsg;
				}
			}
		else {
			return StartMiningReturnType::IgnoreMsg;
			}
		}
	else if (!VerifyMiningAddress(true)) {
		return StartMiningReturnType::IgnoreMsg;
		}

	bool hasData=NHSmaData::HasData();

	if (!showWarnings) {
		for (int i=0; i<10; i++) {
			if (hasData) {
				break;
				}
			QThread::sleep(1000);
			hasData=NHSmaData::HasData();
			Helpers::ConsolePrint("NICEHASH", QString("After %1s has data: %2").arg(i).arg(hasData));
			}
		}

	if (!hasData) {
		Helpers::ConsolePrint("NICEHASH", "No data received within timeout");
		if (showWarnings) {
			QMessageBox::critical(this, International::GetText("Error_with_Exclamation"), International::GetText("Form_Main_msgbox_NullNiceHashDataMsg"), QMessageBox::Ok);
			}
		return StartMiningReturnType::IgnoreMsg;
		}

	// Check if there are unbenchmarked algorithms
	bool isBenchInit=true;
	foreach (ComputeDevice* cdev, *ComputeDeviceManager.Available.Devices) {
		if (cdev->Enabled) {
			foreach (Algorithm* algo, *cdev->GetAlgorithmSettings()) {
				if (algo->Enabled==true) {
					if (!algo->BenchmarkSpeed()) {
						isBenchInit=false;
						break;
						}
					}
				}
			}
		}
	// Check if the user has run benchmark first
	if (!isBenchInit) {
		QMessageBox::StandardButton result=QMessageBox::No;
		if (showWarnings) {
			result=QMessageBox::warning(this, International::GetText("Warning_with_Exclamation"), International::GetText("EnabledUnbenchmarkedAlgorithmsWarning"), QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
			}
		if (result==QMessageBox::Yes) {
			_benchmarkForm=new Form_Benchmark(this, Enums::BenchmarkPerformanceType::Standard, true);
			SetChildFormCenter(_benchmarkForm);
			_benchmarkForm->exec();
			delete _benchmarkForm;
			_benchmarkForm=nullptr;
			InitMainConfigGuiData();
			}
		else if (result==QMessageBox::No) {
			// check devices without benchmarks
			foreach (ComputeDevice* cdev, *ComputeDeviceManager.Available.Devices) {
				if (cdev->Enabled) {
					bool enabled=false;
					foreach (Algorithm* algo, *cdev->GetAlgorithmSettings()) {
						if (algo->BenchmarkSpeed()>0) {
							enabled=true;
							break;
							}
						}
					cdev->Enabled=enabled;
					}
				}
			}
		else {
			return StartMiningReturnType::IgnoreMsg;
			}
		}

	textBoxBTCAddress->setEnabled(false);
	textBoxWorkerName->setEnabled(false);
	comboBoxLocation->setEnabled(false);
	buttonBenchmark->setEnabled(false);
	buttonStartMining->setEnabled(false);
	buttonSettings->setEnabled(false);
	devicesListViewEnableControl1->setIsMining(true);
	buttonStopMining->setEnabled(true);

	// Disable profitable notification on start
	_isNotProfitable=false;

	ConfigManager.generalConfig->BitcoinAddress=textBoxBTCAddress->text().trimmed();
	ConfigManager.generalConfig->WorkerName=textBoxWorkerName->text().trimmed();
	ConfigManager.generalConfig->ServiceLocation=comboBoxLocation->currentIndex();

	InitFlowPanelStart();
	ClearRatesALL();

	QString btcAddress= _demoMode
		? Globals::DemoUser
		: textBoxBTCAddress->text().trimmed();
	bool isMining=MinersManager::StartInitialize(this, Globals::MiningLocation[comboBoxLocation->currentIndex()], textBoxWorkerName->text().trimmed(), btcAddress);

	if (!_demoMode) {
		ConfigManager.GeneralConfigFileCommit();
		}

//	_isSmaUpdated=true;
//	_smaMinerCheck->setInterval(100);
//	_smaMinerCheck->start();
	_minerStatsCheck->start();

	if (ConfigManager.generalConfig->RunScriptOnCUDA_GPU_Lost) {
		_computeDevicesCheckTimer=new QTimer(this);
		connect(_computeDevicesCheckTimer, SIGNAL(timeout()), this, SLOT(ComputeDevicesCheckTimer_Tick()));
		_computeDevicesCheckTimer->setInterval(60000);
		_computeDevicesCheckTimer->start();
		}

	return isMining
		? StartMiningReturnType::StartMining
		: StartMiningReturnType::ShowNoMining;
}

void Form_Main::StopMining()
{
	_minerStatsCheck->stop();
//	_smaMinerCheck->stop();
	if (_computeDevicesCheckTimer!=nullptr) {
		_computeDevicesCheckTimer->stop();
		}

	// Disable IFTTT notification before label call
	_isNotProfitable=false;

	MinersManager::StopAllMiners();

	textBoxBTCAddress->setEnabled(true);
	textBoxWorkerName->setEnabled(true);
	comboBoxLocation->setEnabled(true);
	buttonBenchmark->setEnabled(true);
	buttonStartMining->setEnabled(true);
	buttonSettings->setEnabled(true);
	devicesListViewEnableControl1->setIsMining(false);
	buttonStopMining->setEnabled(false);

	if (_demoMode) {
		_demoMode=false;
		labelDemoMode->setVisible(false);
		}

	UpdateGlobalRate();
}

void Form_Main::showEvent(QShowEvent* ev)
{
	static bool next=false;
	ev->accept();
//	QMainWindow::showEvent(ev);
	if (next) {
		return;
		}
	next=true;
	Form_Main_Shown();
}

void Form_Main::changeEvent(QEvent* ev)
{
	ev->accept();
	if (ev->type()==QEvent::WindowStateChange && windowState().testFlag(Qt::WindowMinimized)) {
		Form1_Resize();
		}
}

void Form_Main::closeEvent(QCloseEvent* ev)
{
	Form1_FormClosing();
	ev->accept();
}
