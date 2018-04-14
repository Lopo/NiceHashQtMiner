#ifndef FORM_FORM_LOADING_H_
#define FORM_FORM_LOADING_H_

#include <QtWidgets>

#include "Interfaces/IMessageNotifier.h"
#include "Interfaces/IMinerUpdateIndicator.h"

class MinersDownloader;


class Form_Loading
	: public QDialog, public IMessageNotifier, public IMinerUpdateIndicator
{
	Q_OBJECT
public:
	virtual ~Form_Loading() {};

	class IAfterInitializationCaller {
	public:
		virtual void AfterLoadComplete()=0;
		};

	Form_Loading(IAfterInitializationCaller* initCaller, const QString& loadFormTitle, const QString& startInfoMsg, const int totalLoadSteps, QWidget* parent=Q_NULLPTR);
	Form_Loading(MinersDownloader* minersDownloader, QWidget* parent=Q_NULLPTR);

	void IncreaseLoadCounterAndMessage(const QString& infoMsg);

	void SetProgressMaxValue(const int maxValue);
	void SetInfoMsg(const QString& infoMsg);

	void IncreaseLoadCounter();
	void FinishLoad();
	void SetValueAndMsg(const int setValue, const QString& infoMsg);
	void SetMessage(const QString& infoMsg);
	void SetMessageAndIncrementStep(const QString& infoMsg);
	void SetMaxProgressValue(const int max);
	void SetProgressValueAndMsg(const int value, const QString& msg);
	void SetTitle(const QString& title);
	void FinishMsg(const bool ok);
	void Form_Loading_Shown();

protected:
	void showEvent(QShowEvent* ev);

private:
	QProgressBar* progressBar1=nullptr;
	QLabel* LoadText=nullptr;
	QLabel* label_LoadingText=nullptr;

	void InitializeComponent();

	int LoadCounter=0;
	int TotalLoadSteps=12;
	IAfterInitializationCaller* AfterInitCaller;
	MinersDownloader* _minersDownloader=nullptr;
};

#endif
