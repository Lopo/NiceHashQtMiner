#ifndef FORMS_FORM_SETTINGS_H_
#define FORMS_FORM_SETTINGS_H_

#include "config.h"
#include <QtWidgets>

class AlgorithmSettingsControl;
class AlgorithmsListView;
class BenchmarkLimitControl;
class DevicesListViewEnableControl;
class ComputeDevice;


class Form_Settings
	: public QDialog
{
	Q_OBJECT
	Q_PROPERTY(bool IsChange READ IsChange WRITE setIsChange)
	Q_PROPERTY(bool IsChangeSaved READ IsChangeSaved)
	Q_PROPERTY(bool IsRestartNeeded READ IsRestartNeeded)

public:
	inline bool IsChange() const { return _isChange; };
	inline bool IsChangeSaved() const { return _IsChangeSaved; };
	inline bool IsRestartNeeded() const { return _IsRestartNeeded; };

	Form_Settings(QWidget *parent=Q_NULLPTR);

private:
	QPushButton* buttonSaveClose=nullptr;
	QPushButton* buttonDefaults=nullptr;
	QPushButton* buttonCloseNoSave=nullptr;
	QTabWidget* tabControlGeneral=nullptr;
	QWidget* tabPageGeneral=nullptr;
	QLabel* label_displayCurrency=nullptr;
	QComboBox* currencyConverterCombobox=nullptr;
	QSpinBox* textBox_LogMaxFileSize=nullptr;
	QLineEdit* textBox_WorkerName=nullptr;
	QLineEdit* textBox_BitcoinAddress=nullptr;
	QCheckBox* checkBox_LogToFile=nullptr;
	QLabel* label_Language=nullptr;
	QComboBox* comboBox_Language=nullptr;
	QLabel* label_LogMaxFileSize=nullptr;
	QComboBox* comboBox_ServiceLocation=nullptr;
	QLabel* label_ServiceLocation=nullptr;
	QLabel* label_WorkerName=nullptr;
	QLabel* label_BitcoinAddress=nullptr;
//	QCheckBox* checkBox_DebugConsole=nullptr;
	QWidget* tabPageDevicesAlgos=nullptr;
	QToolTip* toolTip1=nullptr;
	QGroupBox* groupBox_Main=nullptr;
	QGroupBox* groupBox_Localization=nullptr;
	QGroupBox* groupBox_Logging=nullptr;
	QDoubleSpinBox* textBox_MinProfit=nullptr;
	QLabel* label_MinProfit=nullptr;
	QWidget* tabPageAdvanced=nullptr;
	QGroupBox* groupBox_Miners=nullptr;
	QLabel* label_MinIdleSeconds=nullptr;
	QLabel* label_SwitchMaxSeconds=nullptr;
	QLabel* label_MinerAPIQueryInterval=nullptr;
	QLabel* label_MinerRestartDelayMS=nullptr;
	QSpinBox* textBox_SwitchMinSeconds=nullptr;
	QLabel* label_APIBindPortStart=nullptr;
	QSpinBox* textBox_ethminerDefaultBlockHeight=nullptr;
	QLabel* label_ethminerDefaultBlockHeight=nullptr;
	QSpinBox* textBox_APIBindPortStart=nullptr;
	QLabel* label_SwitchMinSeconds=nullptr;
	QSpinBox* textBox_MinIdleSeconds=nullptr;
	QSpinBox* textBox_SwitchMaxSeconds=nullptr;
	QSpinBox* textBox_MinerRestartDelayMS=nullptr;
	QSpinBox* textBox_MinerAPIQueryInterval=nullptr;
	QGroupBox* groupBoxBenchmarkTimeLimits=nullptr;
	QGroupBox* groupBox_Misc=nullptr;
//	QCheckBox* checkBox_HideMiningWindows=nullptr;
	QCheckBox* checkBox_MinimizeToTray=nullptr;
#if WITH_NVIDIA
	QCheckBox* checkBox_DisableDetectionNVIDIA=nullptr;
#endif
#if WITH_AMD
	QCheckBox* checkBox_DisableDetectionAMD=nullptr;
#endif
#if WITH_NVIDIA
	QCheckBox* checkBox_NVIDIAP0State=nullptr;
#endif
	QCheckBox* checkBox_AutoScaleBTCValues=nullptr;
//	QCheckBox* checkBox_DisableWindowsErrorReporting=nullptr;
	QCheckBox* checkBox_StartMiningWhenIdle=nullptr;
	QCheckBox* checkBox_ShowDriverVersionWarning=nullptr;
	QComboBox* comboBox_CPU0_ForceCPUExtension=nullptr;
	QLabel* label_CPU0_ForceCPUExtension=nullptr;
	QComboBox* comboBox_DagLoadMode=nullptr;
	QLabel* label_DagGeneration=nullptr;
	AlgorithmSettingsControl* algorithmSettingsControl1=nullptr;
	QGroupBox* groupBoxAlgorithmSettings=nullptr;
	AlgorithmsListView* algorithmsListView1=nullptr;
	QPushButton* buttonAllProfit=nullptr;
	QPushButton* buttonSelectedProfit=nullptr;
#if WITH_NVIDIA
	BenchmarkLimitControl* benchmarkLimitControlNVIDIA=nullptr;
#endif
	BenchmarkLimitControl* benchmarkLimitControlCPU=nullptr;
#if WITH_AMD
	BenchmarkLimitControl* benchmarkLimitControlAMD=nullptr;
#endif
	QLabel* pictureBox_Info_BitcoinAddress=nullptr;
#if WITH_NVIDIA
	QLabel* pictureBox_NVIDIAP0State=nullptr;
#endif
//	QPicture* pictureBox_DisableWindowsErrorReporting=nullptr;
	QLabel* pictureBox_ShowDriverVersionWarning=nullptr;
	QLabel* pictureBox_StartMiningWhenIdle=nullptr;
	QLabel* pictureBox_AutoScaleBTCValues=nullptr;
#if WITH_AMD
	QLabel* pictureBox_DisableDetectionAMD=nullptr;
#endif
#if WITH_NVIDIA
	QLabel* pictureBox_DisableDetectionNVIDIA=nullptr;
#endif
	QLabel* pictureBox_MinimizeToTray=nullptr;
//	QPicture* pictureBox_HideMiningWindows=nullptr;
//	QPicture* pictureBox_DebugConsole=nullptr;
	QLabel* pictureBox_LogMaxFileSize=nullptr;
	QLabel* pictureBox_LogToFile=nullptr;
	QLabel* pictureBox_WorkerName=nullptr;
	QLabel* pictureBox_MinProfit=nullptr;
	QLabel* pictureBox_ServiceLocation=nullptr;
//	QPicture* pictureBox5=nullptr;
	QLabel* pictureBox_displayCurrency=nullptr;
	QLabel* pictureBox_Language=nullptr;
	DevicesListViewEnableControl* devicesListViewEnableControl1=nullptr;
	QLabel* pictureBox_MinIdleSeconds=nullptr;
	QLabel* pictureBox_SwitchMinSeconds=nullptr;
	QLabel* pictureBox_CPU0_ForceCPUExtension=nullptr;
	QLabel* pictureBox_MinerAPIQueryInterval=nullptr;
	QLabel* pictureBox_MinerRestartDelayMS=nullptr;
	QLabel* pictureBox_APIBindPortStart=nullptr;
	QLabel* pictureBox_SwitchMaxSeconds=nullptr;
	QLabel* pictureBox_ethminerDefaultBlockHeight=nullptr;
	QLabel* pictureBox_DagGeneration=nullptr;
	QCheckBox* checkBox_AutoStartMining=nullptr;
	QLabel* pictureBox_AutoStartMining=nullptr;
#if WITH_AMD
	QCheckBox* checkBox_AMD_DisableAMDTempControl=nullptr;
	QLabel* pictureBox_AMD_DisableAMDTempControl=nullptr;
#endif
	QCheckBox* checkBox_DisableDefaultOptimizations=nullptr;
	QLabel* pictureBox_DisableDefaultOptimizations=nullptr;
	QCheckBox* checkBox_IdleWhenNoInternetAccess=nullptr;
	QLabel* pictureBox_IdleWhenNoInternetAccess=nullptr;
	QLabel* pictureBox_Use3rdPartyMiners=nullptr;
	QCheckBox* checkBox_Use3rdPartyMiners=nullptr;
	QCheckBox* checkBox_AllowMultipleInstances=nullptr;
	QLabel* pictureBox_AllowMultipleInstances=nullptr;
	QLabel* pictureBox_SwitchProfitabilityThreshold=nullptr;
	QDoubleSpinBox* textBox_SwitchProfitabilityThreshold=nullptr;
	QLabel* label_SwitchProfitabilityThreshold=nullptr;
	QCheckBox* checkBox_RunAtStartup=nullptr;
	QLabel* pictureBox_RunAtStartup=nullptr;
//	QCheckBox* checkBox_MinimizeMiningWindows=nullptr;
//	QPicture* pictureBox_MinimizeMiningWindows=nullptr;
	QCheckBox* checkBox_UseIFTTT=nullptr;
	QLabel* pictureBox_UseIFTTT=nullptr;
	QLabel* label_IFTTTAPIKey=nullptr;
	QLineEdit* textBox_IFTTTKey=nullptr;
	QLabel* pictureBox_ShowInternetConnectionWarning=nullptr;
	QCheckBox* checkBox_ShowInternetConnectionWarning=nullptr;
	QLabel* pictureBox_RunScriptOnCUDA_GPU_Lost=nullptr;
	QCheckBox* checkBox_RunScriptOnCUDA_GPU_Lost=nullptr;
	QLabel* pictureBox_TimeUnit=nullptr;
	QLabel* label_TimeUnit=nullptr;
	QComboBox* comboBox_TimeUnit=nullptr;
	void InitializeComponent();

	bool _isInitFinished=false;
	bool _isChange=false;
	inline void setIsChange(const bool value) { _isChange= _isInitFinished && value;};
	bool _isCredChange=false;
	bool _IsChangeSaved=false;
	bool _IsRestartNeeded=false;

	// most likely we will have settings only per unique devices
	bool ShowUniqueDeviceList=true;

	ComputeDevice* _selectedComputeDevice=nullptr;

	bool _isStartupChanged=false;

	void InitializeToolTip();
	void InitializeFormTranslations();
	void InitializeGeneralTabTranslations();
	void InitializeGeneralTabCallbacks();
	void InitializeGeneralTabFieldValuesReferences();
	void InitializeGeneralTab();
	void InitializeDevicesTab();
	void InitializeDevicesCallbacks();

	bool IsInStartupRegistry();

private Q_SLOTS:
	void GeneralCheckBoxes_CheckedChanged();
#if WITH_AMD
	void CheckBox_AMD_DisableAMDTempControl_CheckedChanged();
#endif
	void CheckBox_DisableDefaultOptimizations_CheckedChanged();
	void CheckBox_RunAtStartup_CheckedChanged();
	void GeneralTextBoxes_Leave();
	void GeneralComboBoxes_Leave();
	void ComboBox_CPU0_ForceCPUExtension_SelectedIndexChanged(int);
	void DevicesListView1_ItemSelectionChanged(int);
	void ButtonSelectedProfit_Click();
	void ButtonAllProfit_Click();
//	void toolTip1_Popoup();
	void ButtonDefaults_Click();
	void ButtonSaveClose_Click();
	void ButtonCloseNoSave_Click();
//	void FormSettings_FormClosing();
	void CurrencyConverterCombobox_SelectedIndexChanged(int);
	void TabControlGeneral_Selected(int);
	void CheckBox_Use3rdPartyMiners_CheckedChanged();
//	void checkBox_HideMiningWindows_CheckChanged();
	void CheckBox_UseIFTTT_CheckChanged();

protected:
	void closeEvent(QCloseEvent*); // FormSettings_FormClosing
};

#endif
