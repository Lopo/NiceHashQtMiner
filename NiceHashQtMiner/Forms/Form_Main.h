#ifndef FORMS_FORM_MAIN_H_
#define FORMS_FORM_MAIN_H_

//#include "Interfaces/IAfterInitializationCaller.h"
#include "Interfaces/IMainFormRatesComunication.h"
#include "Utils/Links.h"
#include "Form_Loading.h"
#include <QtWidgets>

class APIData;
class ClickableLabel;
class NHMCashRegisterBitcoin;
class DevicesListViewEnableControl;
class Form_Loading;
class Form_Benchmark;


class Form_Main
	: public QMainWindow, Form_Loading::IAfterInitializationCaller, IMainFormRatesComunication
{
	Q_OBJECT

public:
	Form_Main(QWidget* parent=Q_NULLPTR);
	virtual ~Form_Main() {};

	enum class StartMiningReturnType {
		StartMining,
		ShowNoMining,
		IgnoreMsg
		};
	Q_ENUM(StartMiningReturnType)

	void ClearRatesALL();
	void ClearRates(int groupCount);
	void ShowNotProfitable(QString msg);
	void HideNotProfitable();
	void ForceMinerStatsUpdate();
	void AfterLoadComplete();
	void AddRateInfo(QString groupName, QString deviceStringInfo, ApiData* iAPIData, double paying, bool isApiGetException);
	void closeEvent(QCloseEvent*);

private:
	QPushButton* buttonStartMining=nullptr;
	QLabel* labelServiceLocation=nullptr;
	QLabel* labelBitcoinAddress=nullptr;
	QStatusBar* statusStrip1=nullptr;
	ClickableLabel* linkLabelCheckStats=nullptr;
	QLabel* labelWorkerName=nullptr;
	QLabel* toolStripStatusLabelGlobalRateValue=nullptr;
	QLabel* toolStripStatusLabelBalanceText=nullptr;
	QLabel* toolStripStatusLabelBalanceBTCValue=nullptr;
	QLabel* toolStripStatusLabelBalanceBTCCode=nullptr;
	QLabel* toolStripStatusLabelGlobalRateText=nullptr;
	QPushButton* buttonStopMining=nullptr;
	QPushButton* buttonBenchmark=nullptr;
	QLabel* toolStripStatusLabelBTCDayText=nullptr;
	QLabel* toolStripStatusLabelBTCDayValue=nullptr;
	QLabel* toolStripStatusLabelBalanceDollarText=nullptr;
	QLabel* toolStripStatusLabelBalanceDollarValue=nullptr;
	QPushButton* buttonSettings=nullptr;
	QPushButton* buttonLogo=nullptr;
	NHMCashRegisterBitcoin* toolStripStatusLabel10=nullptr;
	QSystemTrayIcon* notifyIcon1=nullptr;
	QLineEdit* textBoxBTCAddress=nullptr;
	QComboBox* comboBoxLocation=nullptr;
	QLineEdit* textBoxWorkerName=nullptr;
	ClickableLabel* linkLabelChooseBTCWallet=nullptr;
	QLabel* labelDemoMode=nullptr;
	QListWidget* flowLayoutPanelRates=nullptr;
	QGroupBox* groupBox1=nullptr;
	QLabel* label_NotProfitable=nullptr;
	DevicesListViewEnableControl* devicesListViewEnableControl1=nullptr;
	QPushButton* buttonHelp=nullptr;
	ClickableLabel* linkLabelNewVersion=nullptr;
	QToolTip* toolTip1=nullptr;

protected:
	void showEvent(QShowEvent* ev);
	void changeEvent(QEvent* ev);

private Q_SLOTS:
	void TextBoxCheckBoxMain_Leave(); //done
	void LinkLabelCheckStats_LinkClicked(); //done
	void LinkLabelChooseBTCWallet_LinkClicked(); //done
	void LinkLabelNewVersion_LinkClicked(); //done
	void ButtonBenchmark_Click(); //done
	void ButtonSettings_Click(); //done
	void ButtonStartMining_Click(); //done
	void ButtonStopMining_Click(); //done
	void ButtonLogo_Click(); //done
	void ButtonHelp_Click(); //done
	void ToolStripStatusLabel10_Click(); //done

	void StartupTimer_Tick();
	void SmaCallback();
	void VersionBurnCallback();
	void ExchangeCallback();
	void ConnectionLostCallback();
	void ConnectionEstablishedCallback();
	void IdleCheck_Tick();
	void BalanceCallback();
	void VersionUpdateCallback();
//	void SetVersionLabelCallback();
//	void BitcoinExchangeCheck_Tick();
	void MinerStatsCheck_Tick();
//	void SMAMinerCheck_Tick(); // @deprecated Deprecated in favour of AlgorithmSwitchingManager timer
	void ComputeDevicesCheckTimer_Tick();
	void NotifyIcon1_DoubleClick(QSystemTrayIcon::ActivationReason reason); //done

private:
	void InitializeComponent();
	void InitLocalization();

	QString _visitUrlNew=Links::VisitUrlNew;

	QTimer* _minerStatsCheck=nullptr;
//	QTimer* _smaMinerCheck=nullptr;
//	QTimer* _bitcoinExchangeCheck=nullptr;
	QTimer* _startupTimer=nullptr;
	QTimer* _idleCheck=nullptr;
	QTimer* _computeDevicesCheckTimer=nullptr;

	bool _showWarningNiceHashData=false;
	bool _demoMode=false;

//	Randdom R;

	Form_Loading* _loadingScreen=nullptr;
	Form_Benchmark* _benchmarkForm=nullptr;

	int _flowLayoutPanelVisibleCount=0;
	int _flowLayoutPanelRatesIndex=0;

	const QString BetaAlphaPostfixString="";

	bool _isDeviceDetectionInitialized=false;

	bool _isManuallyStarted=false;
	bool _isNotProfitable=false;

//	bool _isSmaUpdated=false;

	double _factorTimeUnit=1.0;

	int _mainFormHeight=0;
	int _emtpyGroupPanelHeight=0;

	void InitMainConfigGuiData();
	void SetChildFormCenter(QDialog* form); //done
	void Form_Main_Shown();
	void InitFlowPanelStart();
	void UpdateGlobalRate();
	void UpdateExchange();
	bool VerifyMiningAddress(bool showError);
	void Form1_FormClosing(); //done
	QString FormatPayingOutput(double paying);
	void Form1_Resize(); //done
	StartMiningReturnType StartMining(bool showWarnings);
	void StopMining();
	void SetVersionLabel(QString text);
};

#endif
