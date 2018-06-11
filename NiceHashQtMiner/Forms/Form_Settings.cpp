#include "Forms/Form_Settings.h"
#include "Forms/Components/BenchmarkLimitControl.h"
#include "Forms/Components/DevicesListViewEnableControl.h"
#include "Forms/Components/AlgorithmSettingsControl.h"
#include "Configs/ConfigManager.h"
#include "Devices/ComputeDeviceManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Miners/ethminer/MinerEtherum.h"
#include "Miners/Parsing/ExtraLaunchParametersParser.h"
#include "Devices/Algorithms/GroupAlgorithms.h"
#include "Utils/Links.h"
#include "Utils/ProfitabilityCalculator.h"
#include "Stats/NiceHashStats.h"
#include "Forms/Form_3rdParty_TOS.h"
#include "International.h"
#include "Globals.h"
#if WITH_AMD
#	include "Devices/AmdGpuDevice.h"
#endif
#include "Properties/Resources.h"
#include <cfloat>


void Form_Settings::InitializeComponent()
{
	Resources resources;

//	toolTip1=new QToolTip;

	tabControlGeneral=new QTabWidget(this);
	tabControlGeneral->setObjectName("tabControlGeneral");
	tabControlGeneral->setGeometry(12, 11, 612, 440);
	connect(tabControlGeneral, SIGNAL(currentChanged(int)), this, SLOT(TabControlGeneral_Selected(int)));

	tabPageGeneral=new QWidget(tabControlGeneral);
	tabPageGeneral->setObjectName("tabPageGeneral");
	tabPageGeneral->setGeometry(4, 22, 604, 414);
//	tabPageGeneral->setContentsMargins(2, 3, 2, 3);

	groupBox_Misc=new QGroupBox(tabPageGeneral);
	groupBox_Misc->setObjectName("groupBox_Misc");
	groupBox_Misc->setGeometry(358, 6, 244, 405);
	groupBox_Misc->setContentsMargins(2, 3, 2, 3);
	groupBox_Misc->setTitle("Misc:");
	groupBox_Misc->setStyleSheet("QGroupBox { border: 1px solid; margin-top: 0.4em;} QGroupBox::title { top: -8px; left: 10px;}");
#if WITH_NVIDIA
	pictureBox_RunScriptOnCUDA_GPU_Lost=new QLabel(groupBox_Misc);
	pictureBox_RunScriptOnCUDA_GPU_Lost->setObjectName("pictureBox_RunScriptOnCUDA_GPU_Lost");
	pictureBox_RunScriptOnCUDA_GPU_Lost->setGeometry(222, 383, 18, 18);
	pictureBox_RunScriptOnCUDA_GPU_Lost->setPixmap(resources.info_black_18);

	checkBox_RunScriptOnCUDA_GPU_Lost=new QCheckBox(groupBox_Misc);
	checkBox_RunScriptOnCUDA_GPU_Lost->setObjectName("checkBox_RunScriptOnCUDA_GPU_Lost");
	checkBox_RunScriptOnCUDA_GPU_Lost->setGeometry(6, 383, 191, 17);
	checkBox_RunScriptOnCUDA_GPU_Lost->setText("Run script when CUDA GPU is lost");
#endif
	pictureBox_ShowInternetConnectionWarning=new QLabel(groupBox_Misc);
	pictureBox_ShowInternetConnectionWarning->setObjectName("pictureBox_ShowInternetConnectionWarning");
	pictureBox_ShowInternetConnectionWarning->setGeometry(222, 361, 18, 18);
	pictureBox_ShowInternetConnectionWarning->setPixmap(resources.info_black_18);

	checkBox_ShowInternetConnectionWarning=new QCheckBox(groupBox_Misc);
	checkBox_ShowInternetConnectionWarning->setObjectName("checkBox_ShowInternetConnectionWarning");
	checkBox_ShowInternetConnectionWarning->setGeometry(6, 361, 192, 17);
	checkBox_ShowInternetConnectionWarning->setText("Show Internet Connection Warning");

//	checkBox_MinimizeMiningWindows
//	pictureBox_MinimizeMiningWindows

	pictureBox_RunAtStartup=new QLabel(groupBox_Misc);
	pictureBox_RunAtStartup->setObjectName("pictureBox_RunAtStartup");
	pictureBox_RunAtStartup->setGeometry(222, 339, 18, 18);
	pictureBox_RunAtStartup->setPixmap(resources.info_black_18);

	checkBox_RunAtStartup=new QCheckBox(groupBox_Misc);
	checkBox_RunAtStartup->setObjectName("checkBox_RunAtStartup");
	checkBox_RunAtStartup->setGeometry(6, 339, 120, 17);
	checkBox_RunAtStartup->setText("Start on logon");
	connect(checkBox_RunAtStartup, SIGNAL(stateChanged(int)), this, SLOT(CheckBox_RunAtStartup_CheckedChanged()));

	checkBox_AllowMultipleInstances=new QCheckBox(groupBox_Misc);
	checkBox_AllowMultipleInstances->setObjectName("checkBox_AllowMultipleInstances");
	checkBox_AllowMultipleInstances->setGeometry(6, 317, 139, 17);
	checkBox_AllowMultipleInstances->setText("Allow Multiple Instances");
	connect(checkBox_AllowMultipleInstances, SIGNAL(stateChanged(int)), this, SLOT(CheckBox_DisableDefaultOptimizations_CheckedChanged()));

	checkBox_DisableDefaultOptimizations=new QCheckBox(groupBox_Misc);
	checkBox_DisableDefaultOptimizations->setObjectName("checkBox_DisableDefaultOptimizations");
	checkBox_DisableDefaultOptimizations->setGeometry(6, 296, 163, 17);
	checkBox_DisableDefaultOptimizations->setText("Disable Default Optimizations");
	connect(checkBox_DisableDefaultOptimizations, SIGNAL(stateChanged(int)), this, SLOT(CheckBox_DisableDefaultOptimizations_CheckedChanged()));
#if WITH_AMD
	checkBox_AMD_DisableAMDTempControl=new QCheckBox(groupBox_Misc);
	checkBox_AMD_DisableAMDTempControl->setObjectName("checkBox_AMD_DisableAMDTempControl");
	checkBox_AMD_DisableAMDTempControl->setGeometry(6, 275, 145, 17);
	checkBox_AMD_DisableAMDTempControl->setText("DisableAMDTempControl");
	connect(checkBox_AMD_DisableAMDTempControl, SIGNAL(stateChanged(int)), this, SLOT(CheckBox_AMD_DisableAMDTempControl_CheckedChanged()));
#endif
	checkBox_AutoStartMining=new QCheckBox(groupBox_Misc);
	checkBox_AutoStartMining->setObjectName("checkBox_AutoStartMining");
	checkBox_AutoStartMining->setGeometry(6, 19, 102, 17);
	checkBox_AutoStartMining->setContentsMargins(2, 3, 2, 3);
	checkBox_AutoStartMining->setText("Autostart Mining");

//	checkBox_HideMiningWindows

	pictureBox_AllowMultipleInstances=new QLabel(groupBox_Misc);
	pictureBox_AllowMultipleInstances->setObjectName("pictureBox_AllowMultipleInstances");
	pictureBox_AllowMultipleInstances->setGeometry(222, 317, 18, 18);
	pictureBox_AllowMultipleInstances->setPixmap(resources.info_black_18);

	checkBox_MinimizeToTray=new QCheckBox(groupBox_Misc);
	checkBox_MinimizeToTray->setObjectName("checkBox_MinimizeToTray");
	checkBox_MinimizeToTray->setGeometry(6, 83, 100, 17);
	checkBox_MinimizeToTray->setText("MinimizeToTray");

	pictureBox_DisableDefaultOptimizations=new QLabel(groupBox_Misc);
	pictureBox_DisableDefaultOptimizations->setObjectName("pictureBox_DisableDefaultOptimizations");
	pictureBox_DisableDefaultOptimizations->setGeometry(222, 296, 18, 18);
	pictureBox_DisableDefaultOptimizations->setPixmap(resources.info_black_18);
#if WITH_AMD
	pictureBox_AMD_DisableAMDTempControl=new QLabel(groupBox_Misc);
	pictureBox_AMD_DisableAMDTempControl->setObjectName("pictureBox_AMD_DisableAMDTempControl");
	pictureBox_AMD_DisableAMDTempControl->setGeometry(222, 275, 18, 18);
	pictureBox_AMD_DisableAMDTempControl->setPixmap(resources.info_black_18);
#endif
#if WITH_NVIDIA
	pictureBox_NVIDIAP0State=new QLabel(groupBox_Misc);
	pictureBox_NVIDIAP0State->setObjectName("pictureBox_NVIDIAP0State");
	pictureBox_NVIDIAP0State->setGeometry(222, 253, 18, 18);
	pictureBox_NVIDIAP0State->setPixmap(resources.info_black_18);
#endif
//	pictureBox_DisableWindowsErrorReporting

	pictureBox_ShowDriverVersionWarning=new QLabel(groupBox_Misc);
	pictureBox_ShowDriverVersionWarning->setObjectName("pictureBox_ShowDriverVersionWarning");
	pictureBox_ShowDriverVersionWarning->setGeometry(222, 211, 18, 18);
	pictureBox_ShowDriverVersionWarning->setPixmap(resources.info_black_18);

	pictureBox_StartMiningWhenIdle=new QLabel(groupBox_Misc);
	pictureBox_StartMiningWhenIdle->setObjectName("pictureBox_StartMiningWhenIdle");
	pictureBox_StartMiningWhenIdle->setGeometry(222, 189, 18, 18);
	pictureBox_StartMiningWhenIdle->setPixmap(resources.info_black_18);

	pictureBox_AutoScaleBTCValues=new QLabel(groupBox_Misc);
	pictureBox_AutoScaleBTCValues->setObjectName("pictureBox_AutoScaleBTCValues");
	pictureBox_AutoScaleBTCValues->setGeometry(222, 168, 18, 18);
	pictureBox_AutoScaleBTCValues->setPixmap(resources.info_black_18);
#if WITH_AMD
	pictureBox_DisableDetectionAMD=new QLabel(groupBox_Misc);
	pictureBox_DisableDetectionAMD->setObjectName("pictureBox_DisableDetectionAMD");
	pictureBox_DisableDetectionAMD->setGeometry(222, 147, 18, 18);
	pictureBox_DisableDetectionAMD->setPixmap(resources.info_black_18);
#endif
	pictureBox_Use3rdPartyMiners=new QLabel(groupBox_Misc);
	pictureBox_Use3rdPartyMiners->setObjectName("pictureBox_Use3rdPartyMiners");
	pictureBox_Use3rdPartyMiners->setGeometry(222, 103, 18, 18);
	pictureBox_Use3rdPartyMiners->setPixmap(resources.info_black_18);
#if WITH_NVIDIA
	pictureBox_DisableDetectionNVIDIA=new QLabel(groupBox_Misc);
	pictureBox_DisableDetectionNVIDIA->setObjectName("pictureBox_DisableDetectionNVIDIA");
	pictureBox_DisableDetectionNVIDIA->setGeometry(222, 125, 18, 18);
	pictureBox_DisableDetectionNVIDIA->setPixmap(resources.info_black_18);
#endif
	pictureBox_AutoStartMining=new QLabel(groupBox_Misc);
	pictureBox_AutoStartMining->setObjectName("pictureBox_AutoStartMining");
	pictureBox_AutoStartMining->setGeometry(222, 19, 18, 18);
	pictureBox_AutoStartMining->setPixmap(resources.info_black_18);

	pictureBox_MinimizeToTray=new QLabel(groupBox_Misc);
	pictureBox_MinimizeToTray->setObjectName("pictureBox_MinimizeToTray");
	pictureBox_MinimizeToTray->setGeometry(222, 83, 18, 18);
	pictureBox_MinimizeToTray->setPixmap(resources.info_black_18);

//	pictureBox_HideMiningWindows

	checkBox_Use3rdPartyMiners=new QCheckBox(groupBox_Misc);
	checkBox_Use3rdPartyMiners->setObjectName("checkBox_Use3rdPartyMiners");
	checkBox_Use3rdPartyMiners->setGeometry(6, 104, 129, 17);
	checkBox_Use3rdPartyMiners->setText("Enable3rdPartyMiners");
	connect(checkBox_Use3rdPartyMiners, SIGNAL(stateChanged(int)), this, SLOT(CheckBox_Use3rdPartyMiners_CheckedChanged()));
#if WITH_NVIDIA
	checkBox_DisableDetectionNVIDIA=new QCheckBox(groupBox_Misc);
	checkBox_DisableDetectionNVIDIA->setObjectName("checkBox_DisableDetectionNVIDIA");
	checkBox_DisableDetectionNVIDIA->setGeometry(6, 125, 143, 17);
	checkBox_DisableDetectionNVIDIA->setText("DisableDetectionNVIDIA");
#endif
#if WITH_AMD
	checkBox_DisableDetectionAMD=new QCheckBox(groupBox_Misc);
	checkBox_DisableDetectionAMD->setObjectName("checkBox_DisableDetectionAMD");
	checkBox_DisableDetectionAMD->setGeometry(6, 147, 131, 17);
	checkBox_DisableDetectionAMD->setText("DisableDetectionAMD");
#endif
#if WITH_NVIDIA
	checkBox_NVIDIAP0State=new QCheckBox(groupBox_Misc);
	checkBox_NVIDIAP0State->setObjectName("checkBox_NVIDIAP0State");
	checkBox_NVIDIAP0State->setGeometry(6, 253, 100, 17);
	checkBox_NVIDIAP0State->setText("NVIDIAP0State");
#endif
	checkBox_AutoScaleBTCValues=new QCheckBox(groupBox_Misc);
	checkBox_AutoScaleBTCValues->setObjectName("checkBox_AutoScaleBTCValues");
	checkBox_AutoScaleBTCValues->setGeometry(6, 168, 128, 17);
	checkBox_AutoScaleBTCValues->setText("AutoScaleBTCValues");

//	checkBox_DisableWindowsErrorReporting

	checkBox_StartMiningWhenIdle=new QCheckBox(groupBox_Misc);
	checkBox_StartMiningWhenIdle->setObjectName("checkBox_StartMiningWhenIdle");
	checkBox_StartMiningWhenIdle->setGeometry(6, 189, 125, 17);
	checkBox_StartMiningWhenIdle->setText("StartMiningWhenIdle");

	checkBox_ShowDriverVersionWarning=new QCheckBox(groupBox_Misc);
	checkBox_ShowDriverVersionWarning->setObjectName("checkBox_ShowDriverVersionWarning");
	checkBox_ShowDriverVersionWarning->setGeometry(6, 211, 156, 17);
	checkBox_ShowDriverVersionWarning->setText("ShowDriverVersionWarning");

	groupBox_Logging=new QGroupBox(tabPageGeneral);
	groupBox_Logging->setObjectName("groupBox_Logging");
	groupBox_Logging->setGeometry(4, 339, 346, 70);
	groupBox_Logging->setTitle("Logging:");
	groupBox_Logging->setStyleSheet("QGroupBox { border: 1px solid; margin-top: 0.4em;} QGroupBox::title { top: -8px; left: 10px;}");

	label_LogMaxFileSize=new QLabel(groupBox_Logging);
	label_LogMaxFileSize->setObjectName("label_LogMaxFileSize");
	label_LogMaxFileSize->setGeometry(174, 19, 84, 13);
	label_LogMaxFileSize->setText("LogMaxFileSize:");

	textBox_LogMaxFileSize=new QSpinBox(groupBox_Logging);
	textBox_LogMaxFileSize->setObjectName("textBox_LogMaxFileSize");
	textBox_LogMaxFileSize->setGeometry(174, 41, 160, 20);
	textBox_LogMaxFileSize->setRange(0, INT_MAX);

	checkBox_LogToFile=new QCheckBox(groupBox_Logging);
	checkBox_LogToFile->setObjectName("checkBox_LogToFile");
	checkBox_LogToFile->setGeometry(6, 19, 72, 17);
	checkBox_LogToFile->setText("Log to file");

//	pictureBox_DebugConsole

	pictureBox_LogMaxFileSize=new QLabel(groupBox_Logging);
	pictureBox_LogMaxFileSize->setObjectName("pictureBox_LogMaxFileSize");
	pictureBox_LogMaxFileSize->setGeometry(318, 19, 18, 18);
	pictureBox_LogMaxFileSize->setPixmap(resources.info_black_18);

	pictureBox_LogToFile=new QLabel(groupBox_Logging);
	pictureBox_LogToFile->setObjectName("pictureBox_LogToFile");
	pictureBox_LogToFile->setGeometry(130, 19, 18, 18);
	pictureBox_LogToFile->setPixmap(resources.info_black_18);

//	checkBox_DebugConsole

	groupBox_Main=new QGroupBox(tabPageGeneral);
	groupBox_Main->setObjectName("groupBox_Main");
	groupBox_Main->setGeometry(6, 6, 346, 256);
	groupBox_Main->setTitle("Main:");
	groupBox_Main->setStyleSheet("QGroupBox { border: 1px solid; margin-top: 0.4em;} QGroupBox::title { top: -8px; left: 10px;}");

	pictureBox_ElectricityCost=new QLabel(groupBox_Main);
	pictureBox_ElectricityCost->setObjectName("pictureBox_ElectricityCost");
	pictureBox_ElectricityCost->setGeometry(130, 211, 18, 18);
	pictureBox_ElectricityCost->setPixmap(resources.info_black_18);

	textBox_ElectricityCost=new QDoubleSpinBox(groupBox_Main);
	textBox_ElectricityCost->setObjectName("textBox_ElectricityCost");
	textBox_ElectricityCost->setGeometry(10, 231, 138, 20);
	textBox_ElectricityCost->setRange(0, DBL_MAX);
	textBox_ElectricityCost->setDecimals(4);

	label_ElectricityCost=new QLabel(groupBox_Main);
	label_ElectricityCost->setObjectName("label_ElectricityCost");
	label_ElectricityCost->setGeometry(10, 211, 117, 13);
	label_ElectricityCost->setText("Electricity Cost (/KWh):");

	pictureBox_TimeUnit=new QLabel(groupBox_Main);
	pictureBox_TimeUnit->setObjectName("pictureBox_TimeUnit");
	pictureBox_TimeUnit->setGeometry(302, 102, 18, 18);
	pictureBox_TimeUnit->setPixmap(resources.info_black_18);

	label_TimeUnit=new QLabel(groupBox_Main);
	label_TimeUnit->setObjectName("label_TimeUnit");
	label_TimeUnit->setGeometry(160, 102, 52, 13);
	label_TimeUnit->setText("TimeUnit:");

	comboBox_TimeUnit=new QComboBox(groupBox_Main);
	comboBox_TimeUnit->setObjectName("comboBox_TimeUnit");
	comboBox_TimeUnit->setGeometry(160, 123, 160, 21);

	label_IFTTTAPIKey=new QLabel(groupBox_Main);
	label_IFTTTAPIKey->setObjectName("label_IFTTTAPIKey");
	label_IFTTTAPIKey->setGeometry(113, 170, 81, 13);
	label_IFTTTAPIKey->setText("IFTTT API Key:");

	textBox_IFTTTKey=new QLineEdit(groupBox_Main);
	textBox_IFTTTKey->setObjectName("textBox_IFTTTKey");
	textBox_IFTTTKey->setGeometry(110, 187, 210, 20);

	pictureBox_UseIFTTT=new QLabel(groupBox_Main);
	pictureBox_UseIFTTT->setObjectName("pictureBox_UseIFTTT");
	pictureBox_UseIFTTT->setGeometry(88, 184, 18, 18);
	pictureBox_UseIFTTT->setPixmap(resources.info_black_18);

	checkBox_UseIFTTT=new QCheckBox(groupBox_Main);
	checkBox_UseIFTTT->setObjectName("checkBox_UseIFTTT");
	checkBox_UseIFTTT->setGeometry(10, 168, 78, 17);
	checkBox_UseIFTTT->setText("Use IFTTT");

	checkBox_IdleWhenNoInternetAccess=new QCheckBox(groupBox_Main);
	checkBox_IdleWhenNoInternetAccess->setObjectName("checkBox_IdleWhenNoInternetAccess");
	checkBox_IdleWhenNoInternetAccess->setGeometry(10, 149, 205, 17);
	checkBox_IdleWhenNoInternetAccess->setText("Continue Mining If No Internet Access");
	connect(checkBox_IdleWhenNoInternetAccess, SIGNAL(stateChanged(int)), this, SLOT(GeneralCheckBoxes_CheckedChanged()));

	pictureBox_WorkerName=new QLabel(groupBox_Main);
	pictureBox_WorkerName->setObjectName("pictureBox_WorkerName");
	pictureBox_WorkerName->setGeometry(130, 59, 18, 18);
	pictureBox_WorkerName->setPixmap(resources.info_black_18);

	pictureBox_MinProfit=new QLabel(groupBox_Main);
	pictureBox_MinProfit->setObjectName("pictureBox_MinProfit");
	pictureBox_MinProfit->setGeometry(130, 103, 18, 18);
	pictureBox_MinProfit->setPixmap(resources.info_black_18);

	pictureBox_ServiceLocation=new QLabel(groupBox_Main);
	pictureBox_ServiceLocation->setObjectName("pictureBox_ServiceLocation");
	pictureBox_ServiceLocation->setGeometry(302, 59, 18, 18);
	pictureBox_ServiceLocation->setPixmap(resources.info_black_18);

	pictureBox_Info_BitcoinAddress=new QLabel(groupBox_Main);
	pictureBox_Info_BitcoinAddress->setObjectName("pictureBox_Info_BitcoinAddress");
	pictureBox_Info_BitcoinAddress->setGeometry(308, 16, 18, 18);
	pictureBox_Info_BitcoinAddress->setPixmap(resources.info_black_18);

	textBox_MinProfit=new QDoubleSpinBox(groupBox_Main);
	textBox_MinProfit->setObjectName("textBox_MinProfit");
	textBox_MinProfit->setGeometry(10, 123, 138, 20);
	textBox_MinProfit->setRange(0, DBL_MAX);

	pictureBox_IdleWhenNoInternetAccess=new QLabel(groupBox_Main);
	pictureBox_IdleWhenNoInternetAccess->setObjectName("pictureBox_IdleWhenNoInternetAccess");
	pictureBox_IdleWhenNoInternetAccess->setGeometry(302, 149, 18, 18);
	pictureBox_IdleWhenNoInternetAccess->setPixmap(resources.info_black_18);

	label_BitcoinAddress=new QLabel(groupBox_Main);
	label_BitcoinAddress->setObjectName("label_BitcoinAddress");
	label_BitcoinAddress->setGeometry(10, 16, 80, 13);
	label_BitcoinAddress->setText("BitcoinAddress:");

	label_MinProfit=new QLabel(groupBox_Main);
	label_MinProfit->setObjectName("label_MinProfit");
	label_MinProfit->setGeometry(10, 103, 115, 13);
	label_MinProfit->setText("Minimum Profit ($/day):");

	label_WorkerName=new QLabel(groupBox_Main);
	label_WorkerName->setObjectName("label_WorkerName");
	label_WorkerName->setGeometry(10, 59, 73, 13);
	label_WorkerName->setText("WorkerName:");

	label_ServiceLocation=new QLabel(groupBox_Main);
	label_ServiceLocation->setObjectName("label_ServiceLocation");
	label_ServiceLocation->setGeometry(160, 59, 87, 13);
	label_ServiceLocation->setText("ServiceLocation:");

	comboBox_ServiceLocation=new QComboBox(groupBox_Main);
	comboBox_ServiceLocation->setObjectName("comboBox_ServiceLocation");
	comboBox_ServiceLocation->setGeometry(160, 80, 160, 21);
	comboBox_ServiceLocation->insertItems(0, QStringList() << "Europe - Amsterdam" << "USA - San Jose" << "China - Hong Kong" << "Japan - Tokyo" << "India - Chennai" << "Brazil - Sao Paulo");

	textBox_BitcoinAddress=new QLineEdit(groupBox_Main);
	textBox_BitcoinAddress->setObjectName("textBox_BitcoinAddress");
	textBox_BitcoinAddress->setGeometry(10, 36, 316, 20);

	textBox_WorkerName=new QLineEdit(groupBox_Main);
	textBox_WorkerName->setObjectName("textBox_WorkerName");
	textBox_WorkerName->setGeometry(10, 80, 138, 20);

	groupBox_Localization=new QGroupBox(tabPageGeneral);
	groupBox_Localization->setObjectName("groupBox_Localization");
	groupBox_Localization->setGeometry(4, 268, 346, 70);
	groupBox_Localization->setTitle("Localization:");
	groupBox_Localization->setStyleSheet("QGroupBox { border: 1px solid; margin-top: 0.4em;} QGroupBox::title { top: -8px; left: 10px;}");

	label_Language=new QLabel(groupBox_Localization);
	label_Language->setObjectName("label_Language");
	label_Language->setGeometry(6, 16, 58, 13);
	label_Language->setText("Language:");

//	pictureBox5

	pictureBox_displayCurrency=new QLabel(groupBox_Localization);
	pictureBox_displayCurrency->setObjectName("pictureBox_displayCurrency");
	pictureBox_displayCurrency->setGeometry(314, 16, 18, 18);
	pictureBox_displayCurrency->setPixmap(resources.info_black_18);

	pictureBox_Language=new QLabel(groupBox_Localization);
	pictureBox_Language->setObjectName("pictureBox_Language");
	pictureBox_Language->setGeometry(178, 16, 18, 18);
	pictureBox_Language->setPixmap(resources.info_black_18);

	comboBox_Language=new QComboBox(groupBox_Localization);
	comboBox_Language->setObjectName("comboBox_Language");
	comboBox_Language->setGeometry(6, 36, 190, 21);

	currencyConverterCombobox=new QComboBox(groupBox_Localization);
	currencyConverterCombobox->setObjectName("currencyConverterCombobox");
	currencyConverterCombobox->setGeometry(212, 36, 122, 21);
	currencyConverterCombobox->insertItems(0, QStringList() << "AUD" << "BGN" << "BRL" << "CAD" << "CHF" << "CNY" << "CZK" << "DKK" << "EUR" << "GBP" << "HKD" << "HRK" << "HUF" << "IDR" << "ILS" << "INR" << "JPY" << "KRW" << "MXN" << "MYR" << "NOK" << "NZD" << "PHP" << "PLN" << "RON" << "RUB" << "SEK" << "SGD" << "THB" << "TRY" << "USD" << "ZAR");
	connect(currencyConverterCombobox, SIGNAL(currentIndexChanged(int)), SLOT(CurrencyConverterCombobox_SelectedIndexChanged(int)));

	label_displayCurrency=new QLabel(groupBox_Localization);
	label_displayCurrency->setObjectName("label_displayCurrency");
	label_displayCurrency->setGeometry(212, 16, 89, 13);
	label_displayCurrency->setText("Display Currency:");

	tabPageAdvanced=new QWidget(tabControlGeneral);
	tabPageAdvanced->setObjectName("tabPageAdvanced");
	tabPageAdvanced->setGeometry(4, 22, 604, 414);

	groupBox_Miners=new QGroupBox(tabPageAdvanced);
	groupBox_Miners->setObjectName("groupBox_Miners");
	groupBox_Miners->setGeometry(6, 6, 592, 192);
	groupBox_Miners->setTitle("Miners:");
	groupBox_Miners->setStyleSheet("QGroupBox { border: 1px solid; margin-top: 0.4em;} QGroupBox::title { top: -8px; left: 10px;}");

	pictureBox_MinerRestartDelayMS=new QLabel(groupBox_Miners);
	pictureBox_MinerRestartDelayMS->setObjectName("pictureBox_MinerRestartDelayMS");
	pictureBox_MinerRestartDelayMS->setGeometry(362, 15, 18, 18);
	pictureBox_MinerRestartDelayMS->setPixmap(resources.info_black_18);

	pictureBox_APIBindPortStart=new QLabel(groupBox_Miners);
	pictureBox_APIBindPortStart->setObjectName("pictureBox_APIBindPortStart");
	pictureBox_APIBindPortStart->setGeometry(555, 14, 18, 18);
	pictureBox_APIBindPortStart->setPixmap(resources.info_black_18);

	pictureBox_SwitchMaxSeconds=new QLabel(groupBox_Miners);
	pictureBox_SwitchMaxSeconds->setObjectName("pictureBox_SwitchMaxSeconds");
	pictureBox_SwitchMaxSeconds->setGeometry(362, 59, 18, 18);
	pictureBox_SwitchMaxSeconds->setPixmap(resources.info_black_18);

	pictureBox_SwitchProfitabilityThreshold=new QLabel(groupBox_Miners);
	pictureBox_SwitchProfitabilityThreshold->setObjectName("pictureBox_SwitchProfitabilityThreshold");
	pictureBox_SwitchProfitabilityThreshold->setGeometry(555, 60, 18, 18);
	pictureBox_SwitchProfitabilityThreshold->setPixmap(resources.info_black_18);

	pictureBox_ethminerDefaultBlockHeight=new QLabel(groupBox_Miners);
	pictureBox_ethminerDefaultBlockHeight->setObjectName("pictureBox_ethminerDefaultBlockHeight");
	pictureBox_ethminerDefaultBlockHeight->setGeometry(362, 102, 18, 18);
	pictureBox_ethminerDefaultBlockHeight->setPixmap(resources.info_black_18);

	pictureBox_DagGeneration=new QLabel(groupBox_Miners);
	pictureBox_DagGeneration->setObjectName("pictureBox_DagGeneration");
	pictureBox_DagGeneration->setGeometry(362, 146, 18, 18);
	pictureBox_DagGeneration->setPixmap(resources.info_black_18);

	pictureBox_CPU0_ForceCPUExtension=new QLabel(groupBox_Miners);
	pictureBox_CPU0_ForceCPUExtension->setObjectName("pictureBox_CPU0_ForceCPUExtension");
	pictureBox_CPU0_ForceCPUExtension->setGeometry(168, 146, 18, 18);
	pictureBox_CPU0_ForceCPUExtension->setPixmap(resources.info_black_18);
	pictureBox_CPU0_ForceCPUExtension->setVisible(false);

	pictureBox_MinerAPIQueryInterval=new QLabel(groupBox_Miners);
	pictureBox_MinerAPIQueryInterval->setObjectName("pictureBox_MinerAPIQueryInterval");
	pictureBox_MinerAPIQueryInterval->setGeometry(168, 102, 18, 18);
	pictureBox_MinerAPIQueryInterval->setPixmap(resources.info_black_18);

	pictureBox_SwitchMinSeconds=new QLabel(groupBox_Miners);
	pictureBox_SwitchMinSeconds->setObjectName("pictureBox_SwitchMinSeconds");
	pictureBox_SwitchMinSeconds->setGeometry(168, 59, 18, 18);
	pictureBox_SwitchMinSeconds->setPixmap(resources.info_black_18);

	pictureBox_MinIdleSeconds=new QLabel(groupBox_Miners);
	pictureBox_MinIdleSeconds->setObjectName("pictureBox_MinIdleSeconds");
	pictureBox_MinIdleSeconds->setGeometry(168, 15, 18, 18);
	pictureBox_MinIdleSeconds->setPixmap(resources.info_black_18);

	comboBox_DagLoadMode=new QComboBox(groupBox_Miners);
	comboBox_DagLoadMode->setObjectName("comboBox_DagLoadMode");
	comboBox_DagLoadMode->setGeometry(210, 165, 172, 21);
	comboBox_DagLoadMode->insertItems(0, QStringList() << "Automatic" << "SSE2" << "AVX" << "AVX2");

	label_DagGeneration=new QLabel(groupBox_Miners);
	label_DagGeneration->setObjectName("label_DagGeneration");
	label_DagGeneration->setGeometry(210, 149, 87, 13);
	label_DagGeneration->setText("Dag Load Mode:");

	comboBox_CPU0_ForceCPUExtension=new QComboBox(groupBox_Miners);
	comboBox_CPU0_ForceCPUExtension->setObjectName("comboBox_CPU0_ForceCPUExtension");
	comboBox_CPU0_ForceCPUExtension->setGeometry(10, 165, 178, 21);
	comboBox_CPU0_ForceCPUExtension->insertItems(0, QStringList() << "Automatic" << "AVX2_AES" << "AVX2" << "AVX_AES" << "AVX" << "AES");
	comboBox_CPU0_ForceCPUExtension->setVisible(false);
	connect(comboBox_CPU0_ForceCPUExtension, SIGNAL(currentIndexChanged(int)), this, SLOT(ComboBox_CPU0_ForceCPUExtension_SelectedIndexChanged(int)));

	label_CPU0_ForceCPUExtension=new QLabel(groupBox_Miners);
	label_CPU0_ForceCPUExtension->setObjectName("label_CPU0_ForceCPUExtension");
	label_CPU0_ForceCPUExtension->setGeometry(10, 149, 105, 13);
	label_CPU0_ForceCPUExtension->setText("ForceCPUExtension:");
	label_CPU0_ForceCPUExtension->setVisible(false);

	label_MinIdleSeconds=new QLabel(groupBox_Miners);
	label_MinIdleSeconds->setObjectName("label_MinIdleSeconds");
	label_MinIdleSeconds->setGeometry(10, 15, 86, 13);
	label_MinIdleSeconds->setText("MinIdleSeconds:");

	label_SwitchMaxSeconds=new QLabel(groupBox_Miners);
	label_SwitchMaxSeconds->setObjectName("label_SwitchMaxSeconds");
	label_SwitchMaxSeconds->setGeometry(210, 59, 86, 13);
	label_SwitchMaxSeconds->setText("SwitchMaxSecs:");

	label_MinerAPIQueryInterval=new QLabel(groupBox_Miners);
	label_MinerAPIQueryInterval->setObjectName("label_MinerAPIQueryInterval");
	label_MinerAPIQueryInterval->setGeometry(10, 100, 116, 13);
	label_MinerAPIQueryInterval->setText("MinerAPIQueryInterval:");

	label_MinerRestartDelayMS=new QLabel(groupBox_Miners);
	label_MinerRestartDelayMS->setObjectName("label_MinerRestartDelayMS");
	label_MinerRestartDelayMS->setGeometry(210, 15, 113, 13);
	label_MinerRestartDelayMS->setText("MinerRestartDelayMS:");

	textBox_SwitchMinSeconds=new QSpinBox(groupBox_Miners);
	textBox_SwitchMinSeconds->setObjectName("textBox_SwitchMinSeconds");
	textBox_SwitchMinSeconds->setGeometry(10, 78, 178, 20);
	textBox_SwitchMinSeconds->setRange(0, INT_MAX);

	label_APIBindPortStart=new QLabel(groupBox_Miners);
	label_APIBindPortStart->setObjectName("label_APIBindPortStart");
	label_APIBindPortStart->setGeometry(403, 14, 118, 13);
	label_APIBindPortStart->setText("API Bind port pool start:");

	textBox_SwitchProfitabilityThreshold=new QDoubleSpinBox(groupBox_Miners);
	textBox_SwitchProfitabilityThreshold->setObjectName("textBox_SwitchProfitabilityThreshold");
	textBox_SwitchProfitabilityThreshold->setGeometry(403, 78, 118, 20);
	textBox_SwitchProfitabilityThreshold->setRange(0, 1);
	textBox_SwitchProfitabilityThreshold->setSingleStep(0.01);
	textBox_SwitchProfitabilityThreshold->setDecimals(2);
	connect(textBox_SwitchProfitabilityThreshold, SIGNAL(editingFinished()), this, SLOT(GeneralTextBoxes_Leave()));

	textBox_ethminerDefaultBlockHeight=new QSpinBox(groupBox_Miners);
	textBox_ethminerDefaultBlockHeight->setObjectName("textBox_ethminerDefaultBlockHeight");
	textBox_ethminerDefaultBlockHeight->setGeometry(210, 121, 172, 20);
	textBox_ethminerDefaultBlockHeight->setRange(0, INT_MAX);

	label_SwitchProfitabilityThreshold=new QLabel(groupBox_Miners);
	label_SwitchProfitabilityThreshold->setObjectName("label_SwitchProfitabilityThreshold");
	label_SwitchProfitabilityThreshold->setGeometry(401, 58, 142, 13);
	label_SwitchProfitabilityThreshold->setText("ethminerDefaultBlockHeight:");

	label_ethminerDefaultBlockHeight=new QLabel(groupBox_Miners);
	label_ethminerDefaultBlockHeight->setObjectName("label_ethminerDefaultBlockHeight");
	label_ethminerDefaultBlockHeight->setGeometry(210, 100, 142, 13);
	label_ethminerDefaultBlockHeight->setText("ethminerDefaultBlockHeight:");

	textBox_APIBindPortStart=new QSpinBox(groupBox_Miners);
	textBox_APIBindPortStart->setObjectName("textBox_APIBindPortStart");
	textBox_APIBindPortStart->setGeometry(403, 32, 172, 20);
	textBox_APIBindPortStart->setRange(1025, 65535);

	label_SwitchMinSeconds=new QLabel(groupBox_Miners);
	label_SwitchMinSeconds->setObjectName("label_SwitchMinSeconds");
	label_SwitchMinSeconds->setGeometry(10, 59, 83, 13);
	label_SwitchMinSeconds->setText("SwitchMinSecs:");

	textBox_MinIdleSeconds=new QSpinBox(groupBox_Miners);
	textBox_MinIdleSeconds->setObjectName("textBox_MinIdleSeconds");
	textBox_MinIdleSeconds->setGeometry(10, 33, 178, 20);
	textBox_MinIdleSeconds->setRange(0, INT_MAX);

	textBox_SwitchMaxSeconds=new QSpinBox(groupBox_Miners);
	textBox_SwitchMaxSeconds->setObjectName("textBox_SwitchMaxSeconds");
	textBox_SwitchMaxSeconds->setGeometry(210, 78, 172, 20);
	textBox_SwitchMaxSeconds->setRange(0, INT_MAX);

	textBox_MinerRestartDelayMS=new QSpinBox(groupBox_Miners);
	textBox_MinerRestartDelayMS->setObjectName("textBox_MinerRestartDelayMS");
	textBox_MinerRestartDelayMS->setGeometry(210, 33, 172, 20);
	textBox_MinerRestartDelayMS->setRange(0, INT_MAX);

	textBox_MinerAPIQueryInterval=new QSpinBox(groupBox_Miners);
	textBox_MinerAPIQueryInterval->setObjectName("textBox_MinerAPIQueryInterval");
	textBox_MinerAPIQueryInterval->setGeometry(10, 121, 178, 20);
	textBox_MinerAPIQueryInterval->setRange(0, INT_MAX);

	groupBoxBenchmarkTimeLimits=new QGroupBox(tabPageAdvanced);
	groupBoxBenchmarkTimeLimits->setObjectName("groupBoxBenchmarkTimeLimits");
	groupBoxBenchmarkTimeLimits->setGeometry(6, 204, 592, 145);
	groupBoxBenchmarkTimeLimits->setTitle("Benchmark Time Limits:");
	groupBoxBenchmarkTimeLimits->setStyleSheet("QGroupBox { border: 1px solid; margin-top: 0.4em;} QGroupBox::title { top: -8px; left: 10px;}");
#if WITH_NVIDIA
	benchmarkLimitControlNVIDIA=new BenchmarkLimitControl(groupBoxBenchmarkTimeLimits);
	benchmarkLimitControlNVIDIA->setObjectName("benchmarkLimitControlNVIDIA");
	benchmarkLimitControlNVIDIA->setGeometry(10, 17, 178, 121);
	benchmarkLimitControlNVIDIA->setTimeLimits(nullptr);
#endif
	benchmarkLimitControlCPU=new BenchmarkLimitControl(groupBoxBenchmarkTimeLimits);
	benchmarkLimitControlCPU->setObjectName("benchmarkLimitControlCPU");
	benchmarkLimitControlCPU->setGeometry(210, 17, 178, 121);
	benchmarkLimitControlCPU->setTimeLimits(nullptr);
#if WITH_AMD
	benchmarkLimitControlAMD=new BenchmarkLimitControl(groupBoxBenchmarkTimeLimits);
	benchmarkLimitControlAMD->setObjectName("benchmarkLimitControlAMD");
	benchmarkLimitControlAMD->setGeometry(410, 17, 178, 121);
	benchmarkLimitControlAMD->setTimeLimits(nullptr);
#endif
	tabPageDevicesAlgos=new QWidget(tabControlGeneral);
	tabPageDevicesAlgos->setObjectName("tabPageDevicesAlgos");
	tabPageDevicesAlgos->setGeometry(4, 22, 604, 414);

	devicesListViewEnableControl1=new DevicesListViewEnableControl(tabPageDevicesAlgos);
	devicesListViewEnableControl1->setObjectName("devicesListViewEnableControl1");
	devicesListViewEnableControl1->setGeometry(6, 6, 352, 130);
	devicesListViewEnableControl1->BenchmarkCalculation=nullptr;
//	devicesListViewEnableControl1->setFirstColumnText("Enabled");
	devicesListViewEnableControl1->setIsInBenchmark(false);
	devicesListViewEnableControl1->setIsMining(false);
	devicesListViewEnableControl1->SaveToGeneralConfig=false;

	algorithmSettingsControl1=new AlgorithmSettingsControl(tabPageDevicesAlgos);
	algorithmSettingsControl1->setObjectName("algorithmSettingsControl1");
	algorithmSettingsControl1->setGeometry(362, 63, 238, 278);

	groupBoxAlgorithmSettings=new QGroupBox(tabPageDevicesAlgos);
	groupBoxAlgorithmSettings->setObjectName("groupBoxAlgorithmSettings");
	groupBoxAlgorithmSettings->setGeometry(6, 141, 350, 206);
	groupBoxAlgorithmSettings->setTitle("Algorithm settings for selected device:");
	groupBoxAlgorithmSettings->setStyleSheet("QGroupBox { border: 1px solid; margin-top: 0.4em;} QGroupBox::title { top: -8px; left: 10px;}");

	algorithmsListView1=new AlgorithmsListView(groupBoxAlgorithmSettings);
	algorithmsListView1->setObjectName("algorithmsListView1");
	algorithmsListView1->setGeometry(6, 15, 338, 191);
	algorithmsListView1->BenchmarkCalculation=nullptr;
	algorithmsListView1->ComunicationInterface=nullptr;
	algorithmsListView1->IsInBenchmark(false);

	buttonAllProfit=new QPushButton(tabPageDevicesAlgos);
	buttonAllProfit->setObjectName("buttonAllProfit");
	buttonAllProfit->setGeometry(364, 6, 226, 23);
	buttonAllProfit->setText("Check All profitability");
	connect(buttonAllProfit, SIGNAL(clicked()), this, SLOT(ButtonAllProfit_Click()));

	buttonSelectedProfit=new QPushButton(tabPageDevicesAlgos);
	buttonSelectedProfit->setObjectName("buttonSelectedProfit");
	buttonSelectedProfit->setGeometry(364, 35, 226, 23);
	buttonSelectedProfit->setText("Check Selected Profitability");
	connect(buttonSelectedProfit, SIGNAL(clicked()), this, SLOT(ButtonSelectedProfit_Click()));

	buttonSaveClose=new QPushButton(this);
	buttonSaveClose->setObjectName("buttonSaveClose");
	buttonSaveClose->setGeometry(348, 458, 134, 23);
	buttonSaveClose->setText("&Save and Close");
	connect(buttonSaveClose, SIGNAL(clicked()), this, SLOT(ButtonSaveClose_Click()));

	buttonDefaults=new QPushButton(this);
	buttonDefaults->setObjectName("buttonDefaults");
	buttonDefaults->setGeometry(270, 458, 74, 23);
	buttonDefaults->setText("&Defaults");
	connect(buttonDefaults, SIGNAL(clicked()), this, SLOT(ButtonDefaults_Click()));

	buttonCloseNoSave=new QPushButton(this);
	buttonCloseNoSave->setObjectName("buttonCloseNoSave");
	buttonCloseNoSave->setGeometry(486, 458, 134, 23);
	buttonCloseNoSave->setText("&Close without Saving");
	connect(buttonCloseNoSave, SIGNAL(clicked()), this, SLOT(ButtonCloseNoSave_Click()));

	tabControlGeneral->insertTab(0, tabPageGeneral, "General");
	tabControlGeneral->insertTab(1, tabPageAdvanced, "Advanced");
	tabControlGeneral->insertTab(2, tabPageDevicesAlgos, "Devices/Algorithms");

	setObjectName("Form_Settings");
	resize(634, 494);
	setWindowTitle("Settings");
//	connect(this, SIGNAL(closing()), this, SLOT(FormSettings_FormClosing()));
}

Form_Settings::Form_Settings(QWidget *parent)
	: QDialog(parent)
{
	InitializeComponent();
	Resources resources;
	setWindowIcon(resources.logo);

	setIsChange(false);
	_IsChangeSaved=false;

	// backup settings
	ConfigManager.CreateBackup();

	// initialize form
	InitializeFormTranslations();

	// initialize tooltip
	InitializeToolTip();

	// initialize tabs
	InitializeGeneralTab();

	// initialization calls
	InitializeDevicesTab();
	// links algorithm list with algorithm settings control
	algorithmSettingsControl1->setEnabled(false);
	algorithmsListView1->ComunicationInterface=algorithmSettingsControl1;

	// set first device selected
	if (ComputeDeviceManager.Available.Devices->count()>0) {
		_selectedComputeDevice=ComputeDeviceManager.Available.Devices->first();
		algorithmsListView1->SetAlgorithms(_selectedComputeDevice, _selectedComputeDevice->Enabled);
		groupBoxAlgorithmSettings->setTitle(International::GetText("FormSettings_AlgorithmsSettings").arg(_selectedComputeDevice->Name));
		}

	// At the very end set to true
	_isInitFinished=true;
/*
	installEventFilter(this);
	comboBox_CPU0_ForceCPUExtension->setVisible(false);
	label_CPU0_ForceCPUExtension->setVisible(false);
	connect(checkBox_IdleWhenNoInternetAccess, SIGNAL(stateChanged(int)), this, SLOT(GeneralCheckBoxes_CheckedChanged()));
*/
}

void Form_Settings::InitializeToolTip()
{
	// Setup Tooltips
	comboBox_Language->setToolTip(International::GetText("Form_Settings_ToolTip_Language"));
	label_Language->setToolTip(International::GetText("Form_Settings_ToolTip_Language"));
	pictureBox_Language->setToolTip(International::GetText("Form_Settings_ToolTip_Language"));

	textBox_BitcoinAddress->setToolTip(International::GetText("Form_Settings_ToolTip_BitcoinAddress"));
	label_BitcoinAddress->setToolTip(International::GetText("Form_Settings_ToolTip_BitcoinAddress"));
	pictureBox_Info_BitcoinAddress->setToolTip(International::GetText("Form_Settings_ToolTip_BitcoinAddress"));

	textBox_WorkerName->setToolTip(International::GetText("Form_Settings_ToolTip_WorkerName"));
	label_WorkerName->setToolTip(International::GetText("Form_Settings_ToolTip_WorkerName"));
	pictureBox_WorkerName->setToolTip(International::GetText("Form_Settings_ToolTip_WorkerName"));

	comboBox_ServiceLocation->setToolTip(International::GetText("Form_Settings_ToolTip_ServiceLocation"));
	label_ServiceLocation->setToolTip(International::GetText("Form_Settings_ToolTip_ServiceLocation"));
	pictureBox_ServiceLocation->setToolTip(International::GetText("Form_Settings_ToolTip_ServiceLocation"));

	comboBox_TimeUnit->setToolTip(International::GetText("Form_Settings_ToolTip_TimeUnit"));
	label_TimeUnit->setToolTip(International::GetText("Form_Settings_ToolTip_TimeUnit"));
	pictureBox_TimeUnit->setToolTip(International::GetText("Form_Settings_ToolTip_TimeUnit"));

	checkBox_MinimizeToTray->setToolTip(International::GetText("Form_Settings_ToolTip_checkBox_MinimizeToTray"));
	pictureBox_MinimizeToTray->setToolTip(International::GetText("Form_Settings_ToolTip_checkBox_MinimizeToTray"));

	checkBox_Use3rdPartyMiners->setToolTip(International::GetText("Form_Settings_General_3rdparty_ToolTip"));
	pictureBox_Use3rdPartyMiners->setToolTip(International::GetText("Form_Settings_General_3rdparty_ToolTip"));

	checkBox_AllowMultipleInstances->setToolTip(International::GetText("Form_Settings_General_AllowMultipleInstances_ToolTip"));
	pictureBox_AllowMultipleInstances->setToolTip(International::GetText("Form_Settings_General_AllowMultipleInstances_ToolTip"));

	label_MinProfit->setToolTip(International::GetText("Form_Settings_ToolTip_MinimumProfit"));
	pictureBox_MinProfit->setToolTip(International::GetText("Form_Settings_ToolTip_MinimumProfit"));
	textBox_MinProfit->setToolTip(International::GetText("Form_Settings_ToolTip_MinimumProfit"));

	textBox_SwitchMaxSeconds->setToolTip(International::GetText("Form_Settings_ToolTip_SwitchMaxSeconds"));
	label_SwitchMaxSeconds->setToolTip(International::GetText("Form_Settings_ToolTip_SwitchMaxSeconds"));
	pictureBox_SwitchMaxSeconds->setToolTip(International::GetText("Form_Settings_ToolTip_SwitchMaxSeconds"));

	textBox_SwitchMinSeconds->setToolTip(International::GetText("Form_Settings_ToolTip_SwitchMinSeconds"));
	label_SwitchMinSeconds->setToolTip(International::GetText("Form_Settings_ToolTip_SwitchMinSeconds"));
	pictureBox_SwitchMinSeconds->setToolTip(International::GetText("Form_Settings_ToolTip_SwitchMinSeconds"));

	textBox_MinerAPIQueryInterval->setToolTip(International::GetText("Form_Settings_ToolTip_MinerAPIQueryInterval"));
	label_MinerAPIQueryInterval->setToolTip(International::GetText("Form_Settings_ToolTip_MinerAPIQueryInterval"));
	pictureBox_MinerAPIQueryInterval->setToolTip(International::GetText("Form_Settings_ToolTip_MinerAPIQueryInterval"));

	textBox_MinerRestartDelayMS->setToolTip(International::GetText("Form_Settings_ToolTip_MinerRestartDelayMS"));
	label_MinerRestartDelayMS->setToolTip(International::GetText("Form_Settings_ToolTip_MinerRestartDelayMS"));
	pictureBox_MinerRestartDelayMS->setToolTip(International::GetText("Form_Settings_ToolTip_MinerRestartDelayMS"));

	textBox_APIBindPortStart->setToolTip(International::GetText("Form_Settings_ToolTip_APIBindPortStart"));
	label_APIBindPortStart->setToolTip(International::GetText("Form_Settings_ToolTip_APIBindPortStart"));
	pictureBox_APIBindPortStart->setToolTip(International::GetText("Form_Settings_ToolTip_APIBindPortStart"));

	comboBox_DagLoadMode->setToolTip(International::GetText("Form_Settings_ToolTip_DagGeneration"));
	label_DagGeneration->setToolTip(International::GetText("Form_Settings_ToolTip_DagGeneration"));
	pictureBox_DagGeneration->setToolTip(International::GetText("Form_Settings_ToolTip_DagGeneration"));

	benchmarkLimitControlCPU->SetToolTip(/*toolTip1, */"CPUs");
#if WITH_NVIDIA
	benchmarkLimitControlNVIDIA->SetToolTip(/*toolTip1, */"NVIDIA GPUs");
#endif
#if WITH_AMD
	benchmarkLimitControlAMD->SetToolTip(/*toolTip1, */"AMD GPUs");
#endif

#if WITH_NVIDIA
	checkBox_DisableDetectionNVIDIA->setToolTip(International::GetText("Form_Settings_ToolTip_checkBox_DisableDetection").arg("NVIDIA"));
	pictureBox_DisableDetectionNVIDIA->setToolTip(International::GetText("Form_Settings_ToolTip_checkBox_DisableDetection").arg("NVIDIA"));
#endif
#if WITH_AMD
	checkBox_DisableDetectionAMD->setToolTip(International::GetText("Form_Settings_ToolTip_checkBox_DisableDetection").arg("AMD"));
	pictureBox_DisableDetectionAMD->setToolTip(International::GetText("Form_Settings_ToolTip_checkBox_DisableDetection").arg("AMD"));
#endif

	checkBox_AutoScaleBTCValues->setToolTip(International::GetText("Form_Settings_ToolTip_checkBox_AutoScaleBTCValues"));
	pictureBox_AutoScaleBTCValues->setToolTip(International::GetText("Form_Settings_ToolTip_checkBox_AutoScaleBTCValues"));

	checkBox_StartMiningWhenIdle->setToolTip(International::GetText("Form_Settings_ToolTip_checkBox_StartMiningWhenIdle"));
	pictureBox_StartMiningWhenIdle->setToolTip(International::GetText("Form_Settings_ToolTip_checkBox_StartMiningWhenIdle"));

	textBox_MinIdleSeconds->setToolTip(International::GetText("Form_Settings_ToolTip_MinIdleSeconds"));
	label_MinIdleSeconds->setToolTip(International::GetText("Form_Settings_ToolTip_MinIdleSeconds"));
	pictureBox_MinIdleSeconds->setToolTip(International::GetText("Form_Settings_ToolTip_MinIdleSeconds"));

	checkBox_LogToFile->setToolTip(International::GetText("Form_Settings_ToolTip_checkBox_LogToFile"));
	pictureBox_LogToFile->setToolTip(International::GetText("Form_Settings_ToolTip_checkBox_LogToFile"));


	textBox_LogMaxFileSize->setToolTip(International::GetText("Form_Settings_ToolTip_LogMaxFileSize"));
	label_LogMaxFileSize->setToolTip(International::GetText("Form_Settings_ToolTip_LogMaxFileSize"));
	pictureBox_LogMaxFileSize->setToolTip(International::GetText("Form_Settings_ToolTip_LogMaxFileSize"));

	checkBox_ShowDriverVersionWarning->setToolTip(International::GetText("Form_Settings_ToolTip_checkBox_ShowDriverVersionWarning"));
	pictureBox_ShowDriverVersionWarning->setToolTip(International::GetText("Form_Settings_ToolTip_checkBox_ShowDriverVersionWarning"));

	checkBox_ShowInternetConnectionWarning->setToolTip(International::GetText("Form_Settings_ToolTip_checkBox_ShowInternetConnectionWarning"));
	pictureBox_ShowInternetConnectionWarning->setToolTip(International::GetText("Form_Settings_ToolTip_checkBox_ShowInternetConnectionWarning"));
#if WITH_NVIDIA
	checkBox_NVIDIAP0State->setToolTip(International::GetText("Form_Settings_ToolTip_checkBox_NVIDIAP0State"));
	pictureBox_NVIDIAP0State->setToolTip(International::GetText("Form_Settings_ToolTip_checkBox_NVIDIAP0State"));

	checkBox_RunScriptOnCUDA_GPU_Lost->setToolTip(International::GetText("Form_Settings_ToolTip_checkBox_RunScriptOnCUDA_GPU_Lost"));
	pictureBox_RunScriptOnCUDA_GPU_Lost->setToolTip(International::GetText("Form_Settings_ToolTip_checkBox_RunScriptOnCUDA_GPU_Lost"));
#endif
	checkBox_RunAtStartup->setToolTip(International::GetText("Form_Settings_ToolTip_checkBox_RunAtStartup"));
	pictureBox_RunAtStartup->setToolTip(International::GetText("Form_Settings_ToolTip_checkBox_RunAtStartup"));


	checkBox_AutoStartMining->setToolTip(International::GetText("Form_Settings_ToolTip_checkBox_AutoStartMining"));
	pictureBox_AutoStartMining->setToolTip(International::GetText("Form_Settings_ToolTip_checkBox_AutoStartMining"));


	textBox_ethminerDefaultBlockHeight->setToolTip(International::GetText("Form_Settings_ToolTip_ethminerDefaultBlockHeight"));
	label_ethminerDefaultBlockHeight->setToolTip(International::GetText("Form_Settings_ToolTip_ethminerDefaultBlockHeight"));
	pictureBox_ethminerDefaultBlockHeight->setToolTip(International::GetText("Form_Settings_ToolTip_ethminerDefaultBlockHeight"));

	label_displayCurrency->setToolTip(International::GetText("Form_Settings_ToolTip_DisplayCurrency"));
	pictureBox_displayCurrency->setToolTip(International::GetText("Form_Settings_ToolTip_DisplayCurrency"));
	currencyConverterCombobox->setToolTip(International::GetText("Form_Settings_ToolTip_DisplayCurrency"));

	// Setup Tooltips CPU
	comboBox_CPU0_ForceCPUExtension->setToolTip(International::GetText("Form_Settings_ToolTip_CPU_ForceCPUExtension"));
	label_CPU0_ForceCPUExtension->setToolTip(International::GetText("Form_Settings_ToolTip_CPU_ForceCPUExtension"));
	pictureBox_CPU0_ForceCPUExtension->setToolTip(International::GetText("Form_Settings_ToolTip_CPU_ForceCPUExtension"));
#if WITH_AMD
	// amd disable temp control
	checkBox_AMD_DisableAMDTempControl->setToolTip(International::GetText("Form_Settings_ToolTip_DisableAMDTempControl"));
	pictureBox_AMD_DisableAMDTempControl->setToolTip(International::GetText("Form_Settings_ToolTip_DisableAMDTempControl"));
#endif
	// disable default optimizations
	checkBox_DisableDefaultOptimizations->setToolTip(International::GetText("Form_Settings_ToolTip_DisableDefaultOptimizations"));
	pictureBox_DisableDefaultOptimizations->setToolTip(International::GetText("Form_Settings_ToolTip_DisableDefaultOptimizations"));

	// internet connection mining check
	checkBox_IdleWhenNoInternetAccess->setToolTip(International::GetText("Form_Settings_ToolTip_ContinueMiningIfNoInternetAccess"));
	pictureBox_IdleWhenNoInternetAccess->setToolTip(International::GetText("Form_Settings_ToolTip_ContinueMiningIfNoInternetAccess"));

	// IFTTT notification check
	checkBox_UseIFTTT->setToolTip(International::GetText("Form_Settings_ToolTip_UseIFTTT"));
	pictureBox_UseIFTTT->setToolTip(International::GetText("Form_Settings_ToolTip_UseIFTTT"));

	pictureBox_SwitchProfitabilityThreshold->setToolTip(International::GetText("Form_Settings_ToolTip_SwitchProfitabilityThreshold"));
	label_SwitchProfitabilityThreshold->setToolTip(International::GetText("Form_Settings_ToolTip_SwitchProfitabilityThreshold"));

	// Electricity cost
	label_ElectricityCost->setToolTip(International::GetText("Form_Settings_ToolTip_ElectricityCost"));
	textBox_ElectricityCost->setToolTip(International::GetText("Form_Settings_ToolTip_ElectricityCost"));
	pictureBox_ElectricityCost->setToolTip(International::GetText("Form_Settings_ToolTip_ElectricityCost"));

	setWindowTitle(International::GetText("Form_Settings_Title"));

	algorithmSettingsControl1->InitLocale(/*toolTip1*/);
}

void Form_Settings::InitializeFormTranslations()
{
	buttonDefaults->setText(International::GetText("Form_Settings_buttonDefaultsText"));
	buttonSaveClose->setText(International::GetText("Form_Settings_buttonSaveText"));
	buttonCloseNoSave->setText(International::GetText("Form_Settings_buttonCloseNoSaveText"));
}

void Form_Settings::InitializeGeneralTabTranslations()
{
//	checkBox_DebugConsole
	checkBox_AutoStartMining->setText(International::GetText("Form_Settings_General_AutoStartMining"));
//	checkBox_HideMiningWindows
	checkBox_MinimizeToTray->setText(International::GetText("Form_Settings_General_MinimizeToTray"));
#if WITH_NVIDIA
	checkBox_DisableDetectionNVIDIA->setText(International::GetText("Form_Settings_General_DisableDetection").arg("NVIDIA"));
#endif
#if WITH_AMD
	checkBox_DisableDetectionAMD->setText(International::GetText("Form_Settings_General_DisableDetection").arg("AMD"));
#endif
	checkBox_AutoScaleBTCValues->setText(International::GetText("Form_Settings_General_AutoScaleBTCValues"));
	checkBox_StartMiningWhenIdle->setText(International::GetText("Form_Settings_General_StartMiningWhenIdle"));
	checkBox_ShowDriverVersionWarning->setText(International::GetText("Form_Settings_General_ShowDriverVersionWarning"));
//	checkBox_DisableWindowsErrorReporting
	checkBox_ShowInternetConnectionWarning->setText(International::GetText("Form_Settings_General_ShowInternetConnectionWarning"));
	checkBox_Use3rdPartyMiners->setText(International::GetText("Form_Settings_General_3rdparty_Text"));
#if WITH_NVIDIA
	checkBox_NVIDIAP0State->setText(International::GetText("Form_Settings_General_NVIDIAP0State"));
#endif
	checkBox_LogToFile->setText(International::GetText("Form_Settings_General_LogToFile"));
#if WITH_AMD
	checkBox_AMD_DisableAMDTempControl->setText(International::GetText("Form_Settings_General_DisableAMDTempControl"));
#endif
	checkBox_AllowMultipleInstances->setText(International::GetText("Form_Settings_General_AllowMultipleInstances_Text"));
	checkBox_RunAtStartup->setText(International::GetText("Form_Settings_General_RunAtStartup"));
//	checkBox_MinimizeMiningWindows
	checkBox_UseIFTTT->setText(International::GetText("Form_Settings_General_UseIFTTT"));
#if WITH_NVIDIA
	checkBox_RunScriptOnCUDA_GPU_Lost->setText(International::GetText("Form_Settings_General_RunScriptOnCUDA_GPU_Lost"));
#endif

	label_Language->setText(International::GetText("Form_Settings_General_Language")+":");
	label_BitcoinAddress->setText(International::GetText("BitcoinAddress")+":");
	label_WorkerName->setText(International::GetText("WorkerName")+":");
	label_ServiceLocation->setText(International::GetText("Service_Location")+":");
	{
		int i=0;
		foreach (QString loc, Globals::MiningLocation) {
			comboBox_ServiceLocation->setItemText(i++, International::GetText("LocationName_"+loc));
			}
	}
	label_MinIdleSeconds->setText(International::GetText("Form_Settings_General_MinIdleSeconds")+":");
	label_MinerRestartDelayMS->setText(International::GetText("Form_Settings_General_MinerRestartDelayMS")+":");
	label_MinerAPIQueryInterval->setText(International::GetText("Form_Settings_General_MinerAPIQueryInterval")+":");
	label_LogMaxFileSize->setText(International::GetText("Form_Settings_General_LogMaxFileSize")+":");

	label_SwitchMaxSeconds->setText(International::GetText("Form_Settings_General_SwitchMaxSeconds")+":");
	label_SwitchMinSeconds->setText(International::GetText("Form_Settings_General_SwitchMinSeconds")+":");

	label_ethminerDefaultBlockHeight->setText(International::GetText("Form_Settings_General_ethminerDefaultBlockHeight")+":");
	label_DagGeneration->setText(International::GetText("Form_Settings_DagGeneration")+":");
	label_APIBindPortStart->setText(International::GetText("Form_Settings_APIBindPortStart")+":");

	label_MinProfit->setText(International::GetText("Form_Settings_General_MinimumProfit")+":");

	label_displayCurrency->setText(International::GetText("Form_Settings_DisplayCurrency"));

	label_IFTTTAPIKey->setText(International::GetText("Form_Settings_IFTTTAPIKey"));

	label_ElectricityCost->setText(International::GetText("Form_Settings_ElectricityCost"));

	// Benchmark time limits
	// internationalization change
	groupBoxBenchmarkTimeLimits->setTitle(International::GetText("Form_Settings_General_BenchmarkTimeLimits_Title")+":");
	benchmarkLimitControlCPU->setGroupName(International::GetText("Form_Settings_General_BenchmarkTimeLimitsCPU_Group")+":");
#if WITH_NVIDIA
	benchmarkLimitControlNVIDIA->setGroupName(International::GetText("Form_Settings_General_BenchmarkTimeLimitsNVIDIA_Group")+":");
#endif
#if WITH_AMD
	benchmarkLimitControlAMD->setGroupName(International::GetText("Form_Settings_General_BenchmarkTimeLimitsAMD_Group")+":");
#endif
	// moved from constructor because of editor
	benchmarkLimitControlCPU->InitLocale();
#if WITH_NVIDIA
	benchmarkLimitControlNVIDIA->InitLocale();
#endif
#if WITH_AMD
	benchmarkLimitControlAMD->InitLocale();
#endif

	// device enabled listview translation
	devicesListViewEnableControl1->InitLocale();
	algorithmsListView1->InitLocale();

	// Setup Tooltips CPU
	label_CPU0_ForceCPUExtension->setText(International::GetText("Form_Settings_General_CPU_ForceCPUExtension")+":");
	// new translations
	tabControlGeneral->setTabText(0, International::GetText("FormSettings_Tab_General"));
	tabControlGeneral->setTabText(1, International::GetText("FormSettings_Tab_Advanced"));
	tabControlGeneral->setTabText(2, International::GetText("FormSettings_Tab_Devices_Algorithms"));
	groupBox_Main->setTitle(International::GetText("FormSettings_Tab_General_Group_Main"));
	groupBox_Localization->setTitle(International::GetText("FormSettings_Tab_General_Group_Localization"));
	groupBox_Logging->setTitle(International::GetText("FormSettings_Tab_General_Group_Logging"));
	groupBox_Misc->setTitle(International::GetText("FormSettings_Tab_General_Group_Misc"));
	// advanced
	groupBox_Miners->setTitle(International::GetText("FormSettings_Tab_Advanced_Group_Miners"));
	groupBoxBenchmarkTimeLimits->setTitle(International::GetText("FormSettings_Tab_Advanced_Group_BenchmarkTimeLimits"));

	buttonAllProfit->setText(International::GetText("FormSettings_Tab_Devices_Algorithms_Check_ALLProfitability"));
	buttonSelectedProfit->setText(International::GetText("FormSettings_Tab_Devices_Algorithms_Check_SingleProfitability"));

	checkBox_DisableDefaultOptimizations->setText(International::GetText("Form_Settings_Text_DisableDefaultOptimizations"));
	checkBox_IdleWhenNoInternetAccess->setText(International::GetText("Form_Settings_Text_ContinueMiningIfNoInternetAccess"));

	label_SwitchProfitabilityThreshold->setText(International::GetText("Form_Settings_General_SwitchProfitabilityThreshold"));
}

void Form_Settings::InitializeGeneralTabCallbacks()
{
	{ // Add EventHandler for all the general tab's checkboxes
		connect(checkBox_AutoScaleBTCValues, SIGNAL(stateChanged(int)), this, SLOT(GeneralCheckBoxes_CheckedChanged()));
#if WITH_AMD
		connect(checkBox_DisableDetectionAMD, SIGNAL(stateChanged(int)), this, SLOT(GeneralCheckBoxes_CheckedChanged()));
#endif
#if WITH_NVIDIA
		connect(checkBox_DisableDetectionNVIDIA, SIGNAL(stateChanged(int)), this, SLOT(GeneralCheckBoxes_CheckedChanged()));
#endif
		connect(checkBox_MinimizeToTray, SIGNAL(stateChanged(int)), this, SLOT(GeneralCheckBoxes_CheckedChanged()));
//		connect(checkBox_HideMiningWindows, SIGNAL(stateChanged(int)), this, SLOT(CheckBox_HideMiningWindows_CheckChanged()));
//		connect(checkBox_DebugConsole, SIGNAL(stateChanged(int)), this, SLOT(GeneralCheckBoxes_CheckedChanged()));
		connect(checkBox_ShowDriverVersionWarning, SIGNAL(stateChanged(int)), this, SLOT(GeneralCheckBoxes_CheckedChanged()));
//		connect(checkBox_DisableWindowsErrorReporting, SIGNAL(stateChanged(int)), this, SLOT(GeneralCheckBoxes_CheckedChanged()));
		connect(checkBox_ShowInternetConnectionWarning, SIGNAL(stateChanged(int)), this, SLOT(GeneralCheckBoxes_CheckedChanged()));
		connect(checkBox_StartMiningWhenIdle, SIGNAL(stateChanged(int)), this, SLOT(GeneralCheckBoxes_CheckedChanged()));
#if WITH_NVIDIA
		connect(checkBox_NVIDIAP0State, SIGNAL(stateChanged(int)), this, SLOT(GeneralCheckBoxes_CheckedChanged()));
#endif
		connect(checkBox_LogToFile, SIGNAL(stateChanged(int)), this, SLOT(GeneralCheckBoxes_CheckedChanged()));
		connect(checkBox_AutoStartMining, SIGNAL(stateChanged(int)), this, SLOT(GeneralCheckBoxes_CheckedChanged()));
		connect(checkBox_AllowMultipleInstances, SIGNAL(stateChanged(int)), this, SLOT(GeneralCheckBoxes_CheckedChanged()));
//		connect(checkBox_MinimizeMiningWindows, SIGNAL(stateChanged(int)), this, SLOT(GeneralCheckBoxes_CheckedChanged()));
		connect(checkBox_UseIFTTT, SIGNAL(stateChanged(int)), this, SLOT(CheckBox_UseIFTTT_CheckChanged()));
#if WITH_NVIDIA
		connect(checkBox_RunScriptOnCUDA_GPU_Lost, SIGNAL(stateChanged(int)), this, SLOT(GeneralCheckBoxes_CheckedChanged()));
#endif
	}
	{ // Add EventHandler for all the general tab's textboxes
		connect(textBox_BitcoinAddress, SIGNAL(returnPressed()), this, SLOT(GeneralTextBoxes_Leave())); connect(textBox_BitcoinAddress, SIGNAL(editingFinished()), this, SLOT(GeneralTextBoxes_Leave()));
		connect(textBox_WorkerName, SIGNAL(returnPressed()), this, SLOT(GeneralTextBoxes_Leave())); connect(textBox_WorkerName, SIGNAL(editingFinished()), this, SLOT(GeneralTextBoxes_Leave()));
		connect(textBox_IFTTTKey, SIGNAL(returnPressed()), this, SLOT(GeneralTextBoxes_Leave())); connect(textBox_IFTTTKey, SIGNAL(editingFinished()), this, SLOT(GeneralTextBoxes_Leave()));
		// these are ints only
		connect(textBox_SwitchMaxSeconds, SIGNAL(valueChanged(int)), this, SLOT(GeneralTextBoxes_Leave())); connect(textBox_SwitchMaxSeconds, SIGNAL(editingFinished()), this, SLOT(GeneralTextBoxes_Leave()));
		connect(textBox_SwitchMinSeconds, SIGNAL(valueChanged(int)), this, SLOT(GeneralTextBoxes_Leave())); connect(textBox_SwitchMinSeconds, SIGNAL(editingFinished()), this, SLOT(GeneralTextBoxes_Leave()));
		connect(textBox_MinerAPIQueryInterval, SIGNAL(valueChanged(int)), this, SLOT(GeneralTextBoxes_Leave())); connect(textBox_MinerAPIQueryInterval, SIGNAL(editingFinished()), this, SLOT(GeneralTextBoxes_Leave()));
		connect(textBox_MinerRestartDelayMS, SIGNAL(valueChanged(int)), this, SLOT(GeneralTextBoxes_Leave())); connect(textBox_MinerRestartDelayMS, SIGNAL(editingFinished()), this, SLOT(GeneralTextBoxes_Leave()));
		connect(textBox_MinIdleSeconds, SIGNAL(valueChanged(int)), this, SLOT(GeneralTextBoxes_Leave())); connect(textBox_MinIdleSeconds, SIGNAL(editingFinished()), this, SLOT(GeneralTextBoxes_Leave()));
		connect(textBox_LogMaxFileSize, SIGNAL(valueChanged(int)), this, SLOT(GeneralTextBoxes_Leave())); connect(textBox_LogMaxFileSize, SIGNAL(editingFinished()), this, SLOT(GeneralTextBoxes_Leave()));
		connect(textBox_ethminerDefaultBlockHeight, SIGNAL(valueChanged(int)), this, SLOT(GeneralTextBoxes_Leave())); connect(textBox_ethminerDefaultBlockHeight, SIGNAL(editingFinished()), this, SLOT(GeneralTextBoxes_Leave()));
		connect(textBox_APIBindPortStart, SIGNAL(valueChanged(int)), this, SLOT(GeneralTextBoxes_Leave())); connect(textBox_APIBindPortStart, SIGNAL(editingFinished()), this, SLOT(GeneralTextBoxes_Leave()));
		connect(textBox_MinProfit, SIGNAL(valueChanged(double)), this, SLOT(GeneralTextBoxes_Leave())); connect(textBox_MinProfit, SIGNAL(editingFinished()), this, SLOT(GeneralTextBoxes_Leave()));
		connect(textBox_ElectricityCost, SIGNAL(valueChanged(double)), this, SLOT(GeneralTextBoxes_Leave())); connect(textBox_ElectricityCost, SIGNAL(editingFinished()), this, SLOT(GeneralTextBoxes_Leave()));
	}/*
	{ // set int only keypress
		connect(textBox_SwitchMaxSeconds, SIGNAL(keyPressEvent()), this, SLOT(textBoxIntsOnly_KeyPress()));
		connect(textBox_SwitchMinSeconds, SIGNAL(keyPress()), this, SLOT(textBoxIntsOnly_KeyPress()));
		connect(textBox_MinerAPIQueryInterval, SIGNAL(keyPress()), this, SLOT(textBoxIntsOnly_KeyPress()));
		connect(textBox_MinerRestartDelayMS, SIGNAL(keyPress()), this, SLOT(textBoxIntsOnly_KeyPress()));
		connect(textBox_MinIdleSeconds, SIGNAL(keyPress()), this, SLOT(textBoxIntsOnly_KeyPress()));
		connect(textBox_LogMaxFileSize, SIGNAL(keyPressEvent()), this, SLOT(textBoxIntsOnly_KeyPress()));
		connect(textBox_ethminerDefaultBlockHeight, SIGNAL(keyPress()), this, SLOT(textBoxIntsOnly_KeyPress()));
		connect(textBox_APIBindPortStart, SIGNAL(keyPress()), this, SLOT(textBoxIntsOnly_KeyPress()));
		// set double only keypress
		connect(textBox_MinProfit, SIGNAL(keyPress()), this, SLOT(textBoxDoubleOnly_KeyPress()));
		connect(textBox_ElectricityCost, SIGNAL(keyPress()), this, SLOT(textBoxDoubleOnly_KeyPress()));
	}*/
	{ // Add EventHandler for all the general tab's textboxes
		connect(comboBox_Language, SIGNAL(currentIndexChanged(int)), this, SLOT(GeneralComboBoxes_Leave()));
		connect(comboBox_ServiceLocation, SIGNAL(currentIndexChanged(int)), this, SLOT(GeneralComboBoxes_Leave()));
		connect(comboBox_TimeUnit, SIGNAL(currentIndexChanged(int)), this, SLOT(GeneralComboBoxes_Leave()));
		connect(comboBox_DagLoadMode, SIGNAL(currentIndexChanged(int)), this, SLOT(GeneralComboBoxes_Leave()));
	}

	// CPU exceptions
	comboBox_CPU0_ForceCPUExtension->setCurrentIndex((int)ConfigManager.generalConfig->ForceCPUExtension); // @todo check meta
	connect(comboBox_CPU0_ForceCPUExtension, SIGNAL(currentIndexChanged(int)), this, SLOT(ComboBox_CPU0_ForceCPUExtension_SelectedIndexChanged(int)));
	// fill dag dropdown
	comboBox_DagLoadMode->clear();
	for (int i=0; i<(int)Enums::DagGenerationType::END; ++i) {
		comboBox_DagLoadMode->addItem(MinerEtherum::GetDagGenerationString((Enums::DagGenerationType)i));
		}
	// set selected
	comboBox_DagLoadMode->setCurrentIndex((int)ConfigManager.generalConfig->EthminerDagGenerationType);
}

void Form_Settings::InitializeGeneralTabFieldValuesReferences()
{
	{ // Checkboxes set checked value
//	checkBox_DebugConsole->setChecked(ConfigManager.generalConfig->DebugConsole);
	checkBox_AutoStartMining->setChecked(ConfigManager.generalConfig->AutoStartMining);
//	checkBox_HideMiningWindows->setChecked(ConfigManager.generalConfig->HideMiningWindows);
	checkBox_MinimizeToTray->setChecked(ConfigManager.generalConfig->MinimizeToTray);
#if WITH_NVIDIA
	checkBox_DisableDetectionNVIDIA->setChecked(ConfigManager.generalConfig->DeviceDetection->DisableDetectionNVIDIA);
#endif
#if WITH_AMD
	checkBox_DisableDetectionAMD->setChecked(ConfigManager.generalConfig->DeviceDetection->DisableDetectionAMD);
#endif
	checkBox_AutoScaleBTCValues->setChecked(ConfigManager.generalConfig->AutoScaleBTCValues);
	checkBox_StartMiningWhenIdle->setChecked(ConfigManager.generalConfig->StartMiningWhenIdle);
	checkBox_ShowDriverVersionWarning->setChecked(ConfigManager.generalConfig->ShowDriverVersionWarning);
//	checkBox_DisableWindowsErrorReporting->setChecked(ConfigManager.generalConfig->DisableWindowsErrorReporting);
	checkBox_ShowInternetConnectionWarning->setChecked(ConfigManager.generalConfig->ShowInternetConnectionWarning);
#if WITH_NVIDIA
	checkBox_NVIDIAP0State->setChecked(ConfigManager.generalConfig->NVIDIAP0State);
#endif
	checkBox_LogToFile->setChecked(ConfigManager.generalConfig->LogToFile);
#if WITH_AMD
	checkBox_AMD_DisableAMDTempControl->setChecked(ConfigManager.generalConfig->DisableAMDTempControl);
#endif
	checkBox_DisableDefaultOptimizations->setChecked(ConfigManager.generalConfig->DisableDefaultOptimizations);
	checkBox_IdleWhenNoInternetAccess->setChecked(ConfigManager.generalConfig->IdleWhenNoInternetAccess);
	checkBox_Use3rdPartyMiners->setChecked(ConfigManager.generalConfig->Use3rdPartyMiners==Enums::Use3rdPartyMiners::YES);
	checkBox_AllowMultipleInstances->setChecked(ConfigManager.generalConfig->AllowMultipleInstances);
	checkBox_RunAtStartup->setChecked(IsInStartupRegistry());
//	checkBox_MinimizeMiningWindows->setChecked(ConfigManager.generalConfig->MinimizeMiningWindows);
//	checkBox_MinimizeMiningWindows->setEnabled(ConfigManager.generalConfig->HideMiningWindows);
	checkBox_UseIFTTT->setChecked(ConfigManager.generalConfig->UseIFTTT);
#if WITH_NVIDIA
	checkBox_RunScriptOnCUDA_GPU_Lost->setChecked(ConfigManager.generalConfig->RunScriptOnCUDA_GPU_Lost);
#endif
	}

	{ // Textboxes
		textBox_BitcoinAddress->setText(ConfigManager.generalConfig->BitcoinAddress);
		textBox_WorkerName->setText(ConfigManager.generalConfig->WorkerName);
		textBox_IFTTTKey->setText(ConfigManager.generalConfig->IFTTTKey);
		textBox_IFTTTKey->setEnabled(ConfigManager.generalConfig->UseIFTTT);
		textBox_SwitchMaxSeconds->setValue(ConfigManager.generalConfig->SwitchSmaTimeChangeSeconds.Upper);
		textBox_SwitchMinSeconds->setValue(ConfigManager.generalConfig->SwitchSmaTimeChangeSeconds.Lower);
		textBox_MinerAPIQueryInterval->setValue(ConfigManager.generalConfig->MinerAPIQueryInterval);
		textBox_MinerRestartDelayMS->setValue(ConfigManager.generalConfig->MinerRestartDelayMS);
		textBox_MinIdleSeconds->setValue(ConfigManager.generalConfig->MinIdleSeconds);
		textBox_LogMaxFileSize->setValue(ConfigManager.generalConfig->LogMaxFileSize);
		textBox_ethminerDefaultBlockHeight->setValue(ConfigManager.generalConfig->ethminerDefaultBlockHeight);
		textBox_APIBindPortStart->setValue(ConfigManager.generalConfig->ApiBindPortPoolStart);
		textBox_MinProfit->setValue(ConfigManager.generalConfig->MinimumProfit);
		textBox_SwitchProfitabilityThreshold->setValue(ConfigManager.generalConfig->SwitchProfitabilityThreshold);
		textBox_ElectricityCost->setValue(ConfigManager.generalConfig->KwhPrice);
	}

	{ // set custom control references
		benchmarkLimitControlCPU->setTimeLimits(new QVector<int>(ConfigManager.generalConfig->BenchmarkTimeLimits->CPU()));
#if WITH_NVIDIA
		benchmarkLimitControlNVIDIA->setTimeLimits(new QVector<int>(ConfigManager.generalConfig->BenchmarkTimeLimits->NVIDIA()));
#endif
#if WITH_AMD
		benchmarkLimitControlAMD->setTimeLimits(new QVector<int>(ConfigManager.generalConfig->BenchmarkTimeLimits->AMD()));
#endif

		// here we want all devices
		devicesListViewEnableControl1->SetComputeDevices(ComputeDeviceManager.Available.Devices);
		devicesListViewEnableControl1->SetAlgorithmsListView(algorithmsListView1);
		devicesListViewEnableControl1->IsSettingsCopyEnabled=true;
	}

	{ // Add language selections list
		QMap<Enums::LanguageType, QString>* lang=International::GetAvailableLanguages();
		comboBox_Language->clear();
		for (int i=0; i<lang->count(); i++) {
			comboBox_Language->addItem(lang->value((Enums::LanguageType)i));
			}
	}

	QMetaEnum e=QMetaEnum::fromType<Enums::TimeUnitType>();

	{ // Add time unit selection list
		QMap<Enums::TimeUnitType, QString>* timeunits=new QMap<Enums::TimeUnitType, QString>;
		for (int i=0; i<e.keyCount(); i++) {
			timeunits->insert((Enums::TimeUnitType)i, International::GetText(e.valueToKey(i)));
			comboBox_TimeUnit->insertItem(i, timeunits->value((Enums::TimeUnitType)i));
			}
	}

	{ // ComboBox
		comboBox_Language->setCurrentIndex((int)ConfigManager.generalConfig->Language);
		comboBox_ServiceLocation->setCurrentIndex(ConfigManager.generalConfig->ServiceLocation);
		comboBox_TimeUnit->setCurrentText(International::GetText(e.valueToKey((int)ConfigManager.generalConfig->TimeUnit)));
		currencyConverterCombobox->setCurrentText(ConfigManager.generalConfig->DisplayCurrency);
	}
}

void Form_Settings::InitializeGeneralTab()
{
	InitializeGeneralTabTranslations();
	InitializeGeneralTabCallbacks();
	InitializeGeneralTabFieldValuesReferences();
}

void Form_Settings::InitializeDevicesTab()
{
	InitializeDevicesCallbacks();
}

void Form_Settings::InitializeDevicesCallbacks()
{
//	devicesListViewEnableControl1->SetDeviceSelectionChangedCallback((void (*)(int))&Form_Settings::DevicesListView1_ItemSelectionChanged);
	connect(devicesListViewEnableControl1, SIGNAL(ItemSelectionChanged(int)), this, SLOT(DevicesListView1_ItemSelectionChanged(int)));
}

void Form_Settings::GeneralCheckBoxes_CheckedChanged()
{
	if (!_isInitFinished) {
		return;
		}
	// indicate there has been a change
	setIsChange(true);
//	ConfigManager.generalConfig->DebugConsole=checkBox_DebugConsole->isChecked();
	ConfigManager.generalConfig->AutoStartMining=checkBox_AutoStartMining->isChecked();
//	ConfigManager.generalConfig->HideMiningWindows=checkBox_HideMiningWindows->isChecked();
	ConfigManager.generalConfig->MinimizeToTray=checkBox_MinimizeToTray->isChecked();
#if WITH_NVIDIA
	ConfigManager.generalConfig->DeviceDetection->DisableDetectionNVIDIA=checkBox_DisableDetectionNVIDIA->isChecked();
#endif
#if WITH_AMD
	ConfigManager.generalConfig->DeviceDetection->DisableDetectionAMD=checkBox_DisableDetectionAMD->isChecked();
#endif
	ConfigManager.generalConfig->AutoScaleBTCValues=checkBox_AutoScaleBTCValues->isChecked();
	ConfigManager.generalConfig->StartMiningWhenIdle=checkBox_StartMiningWhenIdle->isChecked();
	ConfigManager.generalConfig->ShowDriverVersionWarning=checkBox_ShowDriverVersionWarning->isChecked();
//	ConfigManager.generalConfig->DisableWindowsErrorReporting=checkBox_DisableWindowsErrorReporting->isChecked();
	ConfigManager.generalConfig->ShowInternetConnectionWarning=checkBox_ShowInternetConnectionWarning->isChecked();
#if WITH_NVIDIA
	ConfigManager.generalConfig->NVIDIAP0State=checkBox_NVIDIAP0State->isChecked();
#endif
	ConfigManager.generalConfig->LogToFile=checkBox_LogToFile->isChecked();
	ConfigManager.generalConfig->IdleWhenNoInternetAccess=checkBox_IdleWhenNoInternetAccess->isChecked();
	ConfigManager.generalConfig->UseIFTTT=checkBox_UseIFTTT->isChecked();
	ConfigManager.generalConfig->AllowMultipleInstances=checkBox_AllowMultipleInstances->isChecked();
//	ConfigManager.generalConfig->MinimizeMiningWindows=checkBox_MinimizeMiningWindows->isChecked();
#if WITH_NVIDIA
	ConfigManager.generalConfig->RunScriptOnCUDA_GPU_Lost=checkBox_RunScriptOnCUDA_GPU_Lost->isChecked();
#endif
}
#if WITH_AMD
void Form_Settings::CheckBox_AMD_DisableAMDTempControl_CheckedChanged()
{
	if (!_isInitFinished) {
		return;
		}

	// indicate there has been a change
	setIsChange(true);
	ConfigManager.generalConfig->DisableAMDTempControl=checkBox_AMD_DisableAMDTempControl->isChecked();
	foreach (ComputeDevice* cDev, *ComputeDeviceManager.Available.Devices) {
		if (cDev->DeviceType==Enums::DeviceType::AMD) {
			foreach (Algorithm* algorithm, *cDev->GetAlgorithmSettings()) {
				if (algorithm->NiceHashID!=Enums::AlgorithmType::DaggerHashimoto) {
					algorithm->ExtraLaunchParameters << AmdGpuDevice::TemperatureParam;
					algorithm->ExtraLaunchParameters=ExtraLaunchParametersParser::ParseForMiningPair(new MiningPair(cDev, algorithm), algorithm->NiceHashID, Enums::DeviceType::AMD, false);
					}
				}
			}
		}
}
#endif

void Form_Settings::CheckBox_DisableDefaultOptimizations_CheckedChanged()
{
	if (!_isInitFinished) {
		return;
		}

	// indicate there has been a change
	setIsChange(true);
	ConfigManager.generalConfig->DisableDefaultOptimizations=checkBox_DisableDefaultOptimizations->isChecked();
	if (ConfigManager.generalConfig->DisableDefaultOptimizations) {
		foreach (ComputeDevice* cDev, *ComputeDeviceManager.Available.Devices) {
			foreach (Algorithm* algorithm, *cDev->GetAlgorithmSettings()) {
				algorithm->ExtraLaunchParameters.clear();
#if WITH_AMD
				if (cDev->DeviceType==Enums::DeviceType::AMD && algorithm->NiceHashID!=Enums::AlgorithmType::DaggerHashimoto) {
					algorithm->ExtraLaunchParameters << AmdGpuDevice::TemperatureParam;
					algorithm->ExtraLaunchParameters=ExtraLaunchParametersParser::ParseForMiningPair(new MiningPair(cDev, algorithm), algorithm->NiceHashID, cDev->DeviceType, false);
					}
#endif
				}
			}
		}
	else {
		foreach (ComputeDevice* cDev, *ComputeDeviceManager.Available.Devices) {
			if (cDev->DeviceType==Enums::DeviceType::CPU) { // cpu has no defaults
				continue;
				}
			QList<Algorithm*>* deviceDefaultsAlgoSettings=GroupAlgorithms::CreateForDeviceList(cDev);
			foreach (Algorithm* defaultAlgoSettings, *deviceDefaultsAlgoSettings) {
				Algorithm* toSetAlgo=cDev->GetAlgorithm(defaultAlgoSettings);
				if (toSetAlgo!=nullptr) {
					toSetAlgo->ExtraLaunchParameters=defaultAlgoSettings->ExtraLaunchParameters;
					toSetAlgo->ExtraLaunchParameters=ExtraLaunchParametersParser::ParseForMiningPair(new MiningPair(cDev, toSetAlgo), toSetAlgo->NiceHashID, cDev->DeviceType, false);
					}
				}
			}
		}
}

void Form_Settings::CheckBox_RunAtStartup_CheckedChanged()
{
	_isStartupChanged=true;
}

bool Form_Settings::IsInStartupRegistry()
{
	QString autostartPath=QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).at(0)+QLatin1String("/autostart");
	if (!QDir(autostartPath).exists()) {
		return false;
		}
	QFile autorunFile(autostartPath+QLatin1String("/NiceHashQtMiner.desktop"));
	if (!autorunFile.exists()) {
		return false;
		}
	autorunFile.open(QIODevice::ReadOnly|QIODevice::Text);
	QString fc=QString(autorunFile.readAll());
	autorunFile.close();
	return fc.split('\n').contains("Exec="+qApp->applicationFilePath());
}

void Form_Settings::GeneralTextBoxes_Leave()
{
	if (!_isInitFinished) {
		return;
		}
	setIsChange(true);
	if (ConfigManager.generalConfig->BitcoinAddress!=textBox_BitcoinAddress->text().trimmed()) {
		_isCredChange=true;
		}
	ConfigManager.generalConfig->BitcoinAddress=textBox_BitcoinAddress->text().trimmed();
	if (ConfigManager.generalConfig->WorkerName!=textBox_WorkerName->text().trimmed()) {
		_isCredChange=true;
		}
	ConfigManager.generalConfig->WorkerName=textBox_WorkerName->text().trimmed();

	ConfigManager.generalConfig->SwitchSmaTimeChangeSeconds.Upper=textBox_SwitchMaxSeconds->value();
	ConfigManager.generalConfig->SwitchSmaTimeChangeSeconds.Lower=textBox_SwitchMinSeconds->value();

	ConfigManager.generalConfig->MinerAPIQueryInterval=textBox_MinerAPIQueryInterval->value();
	ConfigManager.generalConfig->MinerRestartDelayMS=textBox_MinerRestartDelayMS->value();
	ConfigManager.generalConfig->MinIdleSeconds=textBox_MinIdleSeconds->value();
	ConfigManager.generalConfig->LogMaxFileSize=textBox_LogMaxFileSize->value();
	ConfigManager.generalConfig->ethminerDefaultBlockHeight=textBox_ethminerDefaultBlockHeight->value();
	ConfigManager.generalConfig->ApiBindPortPoolStart=textBox_APIBindPortStart->value();
	// min profit
	ConfigManager.generalConfig->MinimumProfit=textBox_MinProfit->value();
	ConfigManager.generalConfig->SwitchProfitabilityThreshold=textBox_SwitchProfitabilityThreshold->value();

	ConfigManager.generalConfig->IFTTTKey=textBox_IFTTTKey->text().trimmed();

	ConfigManager.generalConfig->KwhPrice=textBox_ElectricityCost->value();

	// Fix bounds
	ConfigManager.generalConfig->FixSettingBounds();
	// update strings
	textBox_MinProfit->setValue(ConfigManager.generalConfig->MinimumProfit);
	textBox_SwitchProfitabilityThreshold->setValue(ConfigManager.generalConfig->SwitchProfitabilityThreshold);
	textBox_SwitchMaxSeconds->setValue(ConfigManager.generalConfig->SwitchSmaTimeChangeSeconds.Upper);
	textBox_SwitchMinSeconds->setValue(ConfigManager.generalConfig->SwitchSmaTimeChangeSeconds.Lower);
	textBox_MinerAPIQueryInterval->setValue(ConfigManager.generalConfig->MinerAPIQueryInterval);
	textBox_MinerRestartDelayMS->setValue(ConfigManager.generalConfig->MinerRestartDelayMS);
	textBox_MinIdleSeconds->setValue(ConfigManager.generalConfig->MinIdleSeconds);
	textBox_LogMaxFileSize->setValue(ConfigManager.generalConfig->LogMaxFileSize);
	textBox_ethminerDefaultBlockHeight->setValue(ConfigManager.generalConfig->ethminerDefaultBlockHeight);
	textBox_APIBindPortStart->setValue(ConfigManager.generalConfig->ApiBindPortPoolStart);
	textBox_ElectricityCost->setValue(ConfigManager.generalConfig->KwhPrice);
}

void Form_Settings::GeneralComboBoxes_Leave()
{
	if (!_isInitFinished) {
		return;
		}
	setIsChange(true);
	ConfigManager.generalConfig->Language=(Enums::LanguageType)comboBox_Language->currentIndex();
	ConfigManager.generalConfig->ServiceLocation=comboBox_ServiceLocation->currentIndex();
	ConfigManager.generalConfig->TimeUnit=(Enums::TimeUnitType)comboBox_TimeUnit->currentIndex();
	ConfigManager.generalConfig->EthminerDagGenerationType=(Enums::DagGenerationType)comboBox_DagLoadMode->currentIndex();
}

void Form_Settings::ComboBox_CPU0_ForceCPUExtension_SelectedIndexChanged(int index)
{
	ConfigManager.generalConfig->ForceCPUExtension=(Enums::CPUExtensionType)index;
}

void Form_Settings::DevicesListView1_ItemSelectionChanged(int row)
{
	algorithmSettingsControl1->Deselect();
	// show algorithms
	_selectedComputeDevice=ComputeDeviceManager.Available.GetCurrentlySelectedComputeDevice(row, ShowUniqueDeviceList);
	algorithmsListView1->SetAlgorithms(_selectedComputeDevice, _selectedComputeDevice->Enabled);
	groupBoxAlgorithmSettings->setTitle(International::GetText("FormSettings_AlgorithmsSettings").arg(_selectedComputeDevice->Name));
}

void Form_Settings::ButtonSelectedProfit_Click()
{
	if (_selectedComputeDevice==nullptr) {
		QMessageBox::warning(this, International::GetText("Warning_with_Exclamation"), International::GetText("FormSettings_ButtonProfitSingle"), QMessageBox::Ok);
		return;
		}

	QString url=Links::NhmProfitCheck+_selectedComputeDevice->Name;
	foreach (Algorithm* algorithm, *_selectedComputeDevice->GetAlgorithmSettingsFastest()) {
		int id=(int)algorithm->NiceHashID;
		url+="&speed"+QString::number(id)+"="+QString::number(ProfitabilityCalculator::GetFormatedSpeed(algorithm->BenchmarkSpeed(), algorithm->NiceHashID), 'f', 2);
		}
	url+="&nhqmver="+qApp->applicationVersion(); // Add version info
	url+="&cost=1&power=1"; // Set default power and cost to 1
	QDesktopServices::openUrl(url);
}

void Form_Settings::ButtonAllProfit_Click()
{
	QString url=Links::NhmProfitCheck+"CUSTOM";
	QMap<Enums::AlgorithmType, double> total;
	foreach (ComputeDevice* curCDev, *ComputeDeviceManager.Available.Devices) {
		foreach (Algorithm* algorithm, *curCDev->GetAlgorithmSettingsFastest()) {
			if (total.contains(algorithm->NiceHashID)) {
				total[algorithm->NiceHashID]+=algorithm->BenchmarkSpeed();
				}
			else {
				total[algorithm->NiceHashID]=algorithm->BenchmarkSpeed();
				}
			}
		}
	foreach (Enums::AlgorithmType algorithm, total.keys()) {
		int id=(int)algorithm;
		url+="&speed"+QString::number(id)+"="+QString::number(ProfitabilityCalculator::GetFormatedSpeed(total.value(algorithm), algorithm), 'f', 2);
		}
	url+="&nhqmver="+qApp->applicationVersion(); // Add version info
	url+="&cost=1&power=1"; // Set default power and cost to 1
	QDesktopServices::openUrl(url);
}
/*
void Form_Settings::toolTip1_Popoup()
{}
*/
void Form_Settings::ButtonDefaults_Click()
{
	QMessageBox::StandardButton result=QMessageBox::warning(this, International::GetText("Form_Settings_buttonDefaultsTitle"), International::GetText("Form_Settings_buttonDefaultsMsg"), QMessageBox::Yes|QMessageBox::No);
	if (result==QMessageBox::Yes) {
		setIsChange(true);
		_IsChangeSaved=true;
		ConfigManager.generalConfig->SetDefaults();

		International::Initialize(ConfigManager.generalConfig->Language);
		InitializeGeneralTabFieldValuesReferences();
		InitializeGeneralTabTranslations();
		}
}

void Form_Settings::ButtonSaveClose_Click() //done
{
	QMessageBox::information(this, International::GetText("Form_Settings_buttonSaveTitle"), International::GetText("Form_Settings_buttonSaveMsg"), QMessageBox::Ok);
	setIsChange(true);
	_IsChangeSaved=true;

	if (_isCredChange) {
		NiceHashStats::SetCredentials(ConfigManager.generalConfig->BitcoinAddress.trimmed(), ConfigManager.generalConfig->WorkerName.trimmed());
		}

	close();
}

void Form_Settings::ButtonCloseNoSave_Click() //done
{
	_IsChangeSaved=false;
	close();
}

void Form_Settings::closeEvent(QCloseEvent* e) // FormSettings_FormClosing
{
	if (_isChange && !_IsChangeSaved) {
		QMessageBox::StandardButton result=QMessageBox::warning(this, International::GetText("Form_Settings_buttonCloseNoSaveTitle"), International::GetText("Form_Settings_buttonCloseNoSaveMsg"), QMessageBox::Yes|QMessageBox::No);
		if (result==QMessageBox::No) {
			e->ignore();
			return;
			}
		}

	// check restart parameters change
	_IsRestartNeeded=ConfigManager.IsRestartNeeded();

	if (_IsChangeSaved) {
		ConfigManager.GeneralConfigFileCommit();
		ConfigManager.CommitBenchmarks();
		International::Initialize(ConfigManager.generalConfig->Language);

		if (_isStartupChanged) {
			try {
				QString autostartPath=QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).at(0)+QLatin1String("/autostart");
				QDir autorunDir(autostartPath);
				QFile autorunFile(autostartPath+QLatin1String("/NiceHashQtMiner.desktop"));
				if (checkBox_RunAtStartup->isChecked()) {
					if (!autorunDir.exists()) {
						autorunDir.mkpath(autostartPath);
						}
					if (!autorunFile.exists()) {
						if (autorunFile.open(QFile::WriteOnly)) {
							QStringList autorunContent={
								"[Desktop Entry]",
								"Type=Application",
								"Exec="+qApp->applicationFilePath(),
								"Hidden=false",
								"NoDisplay=false",
								"Name[en_US]=NiceHashQtMiner",
								"Name=NiceHashQtMiner",
								"Comment=NiceHash QtMiner"
								};
							QTextStream outStream(&autorunFile);
							outStream << autorunContent.join('\n');
							autorunFile.setPermissions(QFileDevice::ExeUser|QFileDevice::ExeOwner|QFileDevice::ExeOther|QFileDevice::ExeGroup|QFileDevice::WriteUser|QFileDevice::ReadUser);
							autorunFile.close();
							}
						}
					}
				else {
					if (autorunFile.exists()) {
						autorunFile.open(QIODevice::ReadOnly|QIODevice::Text);
						QString fc=QString(autorunFile.readAll());
						autorunFile.close();
						if (fc.split('\n').contains("Exec="+qApp->applicationFilePath())) {
							autorunFile.remove();
							}
						}
					}
				}
			catch (QException& er) {
				Helpers::ConsolePrint("REGISTRY", er.what());
				}
			}
		}
	else {
		ConfigManager.RestoreBackup();
		}
}

void Form_Settings::CurrencyConverterCombobox_SelectedIndexChanged(int index)
{
	ConfigManager.generalConfig->DisplayCurrency=currencyConverterCombobox->currentText();
}

void Form_Settings::TabControlGeneral_Selected(int)
{
	// set first device selected {
	if (ComputeDeviceManager.Available.Devices->count()>0) {
		algorithmSettingsControl1->Deselect();
		}
}

void Form_Settings::CheckBox_Use3rdPartyMiners_CheckedChanged()
{
	if (!_isInitFinished) {
		return;
		}
	if (checkBox_Use3rdPartyMiners->isChecked()) {
		// Show TOS
		Form_3rdParty_TOS* tos=new Form_3rdParty_TOS(this);
		tos->exec();
		delete tos;
		checkBox_Use3rdPartyMiners->setChecked(ConfigManager.generalConfig->Use3rdPartyMiners==Enums::Use3rdPartyMiners::YES);
		}
	else {
		ConfigManager.generalConfig->Use3rdPartyMiners=Enums::Use3rdPartyMiners::NO;
		}
}
/*
void Form_Settings::checkBox_HideMiningWindows_CheckChanged()
{
	if (!_isInitFinished) {
		return;
		}
	setIsChange(true);
	ConfigManager.generalConfig->HideMiningWindows=checkBox_HideMiningWindows->isChecked();
	checkBox_MinimizeMiningWindows->setEnabled(!checkBox_HideMiningWindows->isChecked());
}
*/
void Form_Settings::CheckBox_UseIFTTT_CheckChanged()
{
	if (!_isInitFinished) {
		return;
		}
	setIsChange(true);

	ConfigManager.generalConfig->UseIFTTT=checkBox_UseIFTTT->isChecked();

	textBox_IFTTTKey->setEnabled(checkBox_UseIFTTT->isChecked());
}
/*
void Form_Settings::on_tabControlGeneral_currentChanged(int index)
{
	if (index==1) {
		tabControlGeneral_Selected();
		return;
		}
}
*/