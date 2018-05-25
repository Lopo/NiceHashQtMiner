#ifndef GLOBALS_H_
#define GLOBALS_H_

#include "Enums.h"

class NiceHashSma;

class Globals
{
public:
	// constants
	static const QVector<QString> MiningLocation;

	static QString DemoUser;

	static int CurrentTosVer; // change this if TOS changes

	// variables
	static int ThreadsPerCpu;

	static bool IsFirstNetworkCheckTimeout;

	static int FirstNetworkCheckTimeoutTimeMs;
	static int FirstNetworkCheckTimeoutTries;

	static QString GetLocationURL(Enums::AlgorithmType algorithmType, QString miningLocation, Enums::NhmConectionType connectionType);
	static QString GetBitcoinUser();
};

#endif
