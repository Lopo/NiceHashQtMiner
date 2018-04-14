#ifndef SWITCHING_ALGORITHMHISTORY_H_
#define SWITCHING_ALGORITHMHISTORY_H_

#include <QList>


/**
 * Represents a limited history of profitability
 */
class AlgorithmHistory
{
public:
	AlgorithmHistory(int maxHistory);
	void Add(double profit);
	int CountOverProfit(double profit);

private:
	int _maxHistory=0;
	QList<double> _history;
};

#endif
