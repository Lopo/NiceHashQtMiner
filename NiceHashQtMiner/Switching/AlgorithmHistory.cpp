#include "Switching/AlgorithmHistory.h"


/**
 * Initialize new empty instance
 *
 * @param maxHistory Maximum history to keep
 */
AlgorithmHistory::AlgorithmHistory(int maxHistory)
{
	_maxHistory=maxHistory;
}

/**
 * Append profit and remove oldest if at maximum history
 *
 * @param profit
 */
void AlgorithmHistory::Add(double profit)
{
	_history.append(profit);
	if (_history.count()>_maxHistory) {
		_history.removeFirst();
		}
}

/**
 * Count the number of times the algorithm has been above a profit
 * Count starts from the most recent profit and must be consecutive
 *
 * @param profit Profit to check if over
 * @return Number of consecutive times over from most recent profit
 */
int AlgorithmHistory::CountOverProfit(double profit)
{
	int count=0;

	for (int i=_history.count()-1; i>=0; i--) {
		if (_history.at(i)>profit) {
			count++;
			}
		else {
			break;
			}
		}

	return count;
}
