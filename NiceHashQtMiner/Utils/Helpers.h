#ifndef UTILS_HELPERS_H_
#define UTILS_HELPERS_H_

#include "PInvoke/PInvokeHelpers.h"
#include "Enums.h"


class Helpers
	: public PInvokeHelpers
{
public:
	static bool Is64BitOperatingSystem();
	static bool IsElevated();

	static bool InternalCheckIsWow64();
	static void ConsolePrint(QString grp, QString text);
//	static void ConsolePrint(QString grp, QString text, ...);
	static void ConsolePrint(QString grp, QString text, QString arg0);
	static void ConsolePrint(QString grp, QString text, QString arg0, QString arg1);
	static void ConsolePrint(QString grp, QString text, QString arg0, QString arg1, QString arg2);
	static uint32_t GetIdleTime();
//	static void DisableWindowsErrorReporting(bool en);
	static QString FormatSpeedOutput(double speed, QString separator=" ");
	static QString FormatDualSpeedOutput(double primarySpeed, double secondarySpeed=0, Enums::AlgorithmType algo=Enums::AlgorithmType::NONE);
	static QString GetMotherboardID();
	static QString GetCpuID();
	static bool WebRequestTestGoogle();
	static bool IsConnectedToInternet();
	static int ParseInt(QString text);
	static long ParseLong(QString text);
	static double ParseDouble(QString text);
//	static bool isWMIEnabled();
//	static void InstallVcRedist();
	static void SetDefaultEnvironmentVariables();
	static void SetNvidiaP0State();
	static Enums::AlgorithmType DualAlgoFromAlgos(Enums::AlgorithmType primary, Enums::AlgorithmType secondary);

private:
	static const bool is64BitProcess=sizeof(int*)==8;
};

#endif
