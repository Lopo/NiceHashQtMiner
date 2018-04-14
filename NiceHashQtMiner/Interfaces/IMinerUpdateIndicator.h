#ifndef INTERFACES_IMINERUPDATEINDICATOR_H_
#define INTERFACES_IMINERUPDATEINDICATOR_H_


class IMinerUpdateIndicator
{
	virtual void SetMaxProgressValue(const int max)=0;
	virtual void SetProgressValueAndMsg(const int value, const QString& msg)=0;
	virtual void SetTitle(const QString& title)=0;
	virtual void FinishMsg(const bool success)=0;
};

#endif
