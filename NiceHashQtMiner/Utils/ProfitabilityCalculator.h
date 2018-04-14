#ifndef UTILS_PROFITABILITYCALCULATOR_H_
#define UTILS_PROFITABILITYCALCULATOR_H_

#include "Enums.h"
#include <QMap>


/**
 * this class mirrors the web profitability, chech what https://www.nicehash.com/?p=calc is using for each algo
 */
class ProfitabilityCalculator
{
public:
	static double GetFormatedSpeed(double speed, Enums::AlgorithmType type);

private:
	static constexpr double kHs=1000;
	static constexpr double MHs=1000000;
	static constexpr double GHs=1000000000;
	static constexpr double THs=1000000000000;

	static QMap<Enums::AlgorithmType, double> Div; // divide factor to mirror web values
};

#endif
