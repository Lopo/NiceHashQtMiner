#include "Forms/Form_Loading.h"
#include "Utils/MinersDownloader.h"
#include <QThread>


void Form_Loading::InitializeComponent()
{
	progressBar1=new QProgressBar(this);
	progressBar1->setObjectName("progressBar1");
	progressBar1->setGeometry(12, 25, 286, 23);
	progressBar1->setTextVisible(false);

	LoadText=new QLabel(this);
	LoadText->setObjectName("LoadText");
	LoadText->setGeometry(9, 51, 283, 13);
	LoadText->setText("                                                                                            ");

	label_LoadingText=new QLabel(this);
	label_LoadingText->setObjectName("label_LoadingText");
	label_LoadingText->setGeometry(84, 9, 136, 13);
	QFont fl;
	fl.setBold(true);
	label_LoadingText->setFont(fl);
	label_LoadingText->setText("Loading, please wait...");

	setWindowModality(Qt::ApplicationModal);
	resize(310, 76);
	setContextMenuPolicy(Qt::NoContextMenu);
	setModal(true);
	setObjectName("Form_Loading");
	setWindowTitle("Form_Loading");
	setWindowFlag(Qt::SplashScreen); // borderless
//	connect(this, SIGNAL(), this, SLOT(Form_Loading_Shown()));
}

Form_Loading::Form_Loading(IAfterInitializationCaller* initCaller, const QString& loadFormTitle, const QString& startInfoMsg, const int totalLoadSteps, QWidget* parent/*=Q_NULLPTR*/)
	: QDialog(parent)
{
	InitializeComponent();

	label_LoadingText->setText(loadFormTitle);
	label_LoadingText->move((width()-label_LoadingText->width())/2, label_LoadingText->y());

	AfterInitCaller=initCaller;

	TotalLoadSteps=totalLoadSteps;
	progressBar1->setMaximum(TotalLoadSteps);
	progressBar1->setValue(0);

	SetInfoMsg(startInfoMsg);
}

Form_Loading::Form_Loading(MinersDownloader* minersDownloader, QWidget* parent/*=Q_NULLPTR*/)
	: QDialog(parent)
{
	InitializeComponent();
	label_LoadingText->move((width()-label_LoadingText->width())/2, label_LoadingText->y());
	_minersDownloader=minersDownloader;
}

void Form_Loading::IncreaseLoadCounterAndMessage(const QString& infoMsg)
{
	SetInfoMsg(infoMsg);
	IncreaseLoadCounter();
}

void Form_Loading::SetProgressMaxValue(const int maxValue)
{
	progressBar1->setMaximum(maxValue);
}

void Form_Loading::SetInfoMsg(const QString& infoMsg)
{
	LoadText->setText(infoMsg);
}

void Form_Loading::IncreaseLoadCounter()
{
	LoadCounter++;
	progressBar1->setValue(LoadCounter);
	update();
	if (LoadCounter>=TotalLoadSteps) {
		AfterInitCaller->AfterLoadComplete();
//		close(); // crashing - fix!
//		dispose();
		}
}

void Form_Loading::FinishLoad()
{
	while (LoadCounter<TotalLoadSteps) {
		IncreaseLoadCounter();
		}
}

void Form_Loading::SetValueAndMsg(const int setValue, const QString& infoMsg)
{
	SetInfoMsg(infoMsg);
	progressBar1->setValue(setValue);
	update();
	if (progressBar1->value()>=progressBar1->maximum()) {
		close();
		}
}

void Form_Loading::SetMessage(const QString& infoMsg)
{
	SetInfoMsg(infoMsg);
}

void Form_Loading::SetMessageAndIncrementStep(const QString& infoMsg)
{
	IncreaseLoadCounterAndMessage(infoMsg);
}

void Form_Loading::SetMaxProgressValue(const int max)
{
	progressBar1->setMaximum(max);
	progressBar1->setValue(0);
}

void Form_Loading::SetProgressValueAndMsg(const int value, const QString& msg)
{
	if (value<=progressBar1->maximum()) {
		progressBar1->setValue(value);
		LoadText->setText(msg);
		progressBar1->update(); // invalidate()
		LoadText->update(); // invalidate()
		}
}

void Form_Loading::SetTitle(const QString& title)
{
	label_LoadingText->setText(title);
}

void Form_Loading::FinishMsg(const bool ok)
{
	if (ok) {
		label_LoadingText->setText("Init Finished!");
		}
	else {
		label_LoadingText->setText("Init Failed!");
		}
	QThread::msleep(1000);
	close();
}

void Form_Loading::Form_Loading_Shown()
{
	if (_minersDownloader!=nullptr) {
		_minersDownloader->Start(this);
		}
}

void Form_Loading::showEvent(QShowEvent* ev)
{
	static bool next=false;
	ev->accept();
	if (next) {
		return;
		}
	next=true;
	Form_Loading_Shown();
}
