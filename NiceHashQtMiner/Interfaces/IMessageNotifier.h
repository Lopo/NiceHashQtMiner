#ifndef INTERFACES_IMESSAGENOTIFIER_H_
#define INTERFACES_IMESSAGENOTIFIER_H_


class IMessageNotifier
{
public:
	virtual void SetMessage(const QString& infoMsg)=0;
	virtual void SetMessageAndIncrementStep(const QString& infoMsg)=0;
};

#endif
