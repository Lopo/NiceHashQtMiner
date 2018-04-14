#ifndef INTERFACES_IMAINFORMRATESCOMUNICATION_H_
#define INTERFACES_IMAINFORMRATESCOMUNICATION_H_

#include <QString>

class ApiData;


class IMainFormRatesComunication
{
public:
	virtual void ClearRatesALL()=0;
	virtual void AddRateInfo(QString groupName, QString deviceStringInfo, ApiData* iAPIData, double paying, bool isApiGetException)=0;

	// The following four must use an invoker since they may be called from non-UI thread
	virtual void ShowNotProfitable(QString msg)=0;
	virtual void HideNotProfitable()=0;
	virtual void ForceMinerStatsUpdate()=0;
	virtual void ClearRates(int groupCount)=0;
};

#endif
