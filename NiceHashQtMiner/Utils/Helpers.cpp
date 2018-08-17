#include "Utils/Helpers.h"
#include "config.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Logger.h"
#include "Globals.h"
#include <QtNetwork>
#if LIBXSS_FOUND
#	include <X11/extensions/scrnsaver.h>
#endif
#ifdef HAVE_LIBCPUID
#	include <libcpuid/libcpuid.h>
#endif
#include <unistd.h>


extern QNetworkAccessManager* qnam;

bool Helpers::Is64BitOperatingSystem()
{
	return is64BitProcess || InternalCheckIsWow64();
}

bool Helpers::IsElevated()
{
	return !geteuid();
}

bool Helpers::InternalCheckIsWow64()
{
	return false;
}

void Helpers::ConsolePrint(QString grp, QString text)
{
#if WITH_DEBUG
#else
	if (grp=="DEBUG") {
		return;
		}
#endif
	// try will prevent an error if something tries to print an invalid character
	try {
//		std::cout << '[' << QDateTime::currentDateTime().toString("YYYY-MM-dd HH:mm:ss").toStdString() << "] [" << grp.toStdString() << "] " + QString.toStdString() << std::endl;

		if (ConfigManager.generalConfig->LogToFile && Logger.IsInit) {
			Logger.log->info("["+grp+"] "+text);
			}
		}
	catch (...) {} // // Not gonna recursively call here in case something is seriously wrong
}
/*
void Helpers::ConsolePrint(QString grp, QString text, ...)
{
	ConsolePrint(grp, QString.arg(text, ));
}
*/
void Helpers::ConsolePrint(QString grp, QString text, QString arg0)
{
	ConsolePrint(grp, text.arg(arg0));
}

void Helpers::ConsolePrint(QString grp, QString text, QString arg0, QString arg1)
{
	ConsolePrint(grp, text.arg(arg0, arg1));
}

void Helpers::ConsolePrint(QString grp, QString text, QString arg0, QString arg1, QString arg2)
{
	ConsolePrint(grp, text.arg(arg0, arg1, arg2));
}

uint32_t Helpers::GetIdleTime()
{
#if LIBXSS_FOUND
	Display *dpy=XOpenDisplay(nullptr);
	if (!dpy) {
		return 0;
		}
	XScreenSaverInfo *info=XScreenSaverAllocInfo();
	XScreenSaverQueryInfo(dpy, DefaultRootWindow(dpy), info);
	return info->idle;
#else
	return 0;
#endif
}
/*
void Helpers::DisableWindowsErrorReporting(bool en)
{
	ConsolePrint("NICEHASH", "Trying to enable/disable Windows error reporting");
}
*/
QString Helpers::FormatSpeedOutput(double speed, QString separator/*=" "*/)
{
	if (speed<1000) {
		return QString::number(speed, 'f', 3)+separator;
		}
	if (speed<100000) {
		return QString::number(speed*0.001, 'f', 3)+separator+"k";
		}
	if (speed<100000000) {
		return QString::number(speed*0.000001, 'f', 3)+separator+"M";
		}
	if (speed<100000000000) {
		return QString::number(speed*0.000000001, 'f', 3)+separator+"G";
		}
	return QString::number(speed*0.000000000001, 'f', 3)+separator+"T";
}

QString Helpers::FormatDualSpeedOutput(double primarySpeed, double secondarySpeed/*=0*/, Enums::AlgorithmType algo/*=Enums::AlgorithmType::NONE*/)
{
	QString ret;
	if (secondarySpeed>0) {
		ret=FormatSpeedOutput(primarySpeed, "")+"/"+FormatSpeedOutput(secondarySpeed, "")+" ";
		}
	else {
		ret=FormatSpeedOutput(primarySpeed);
		}

	QString unit= algo==Enums::AlgorithmType::Equihash? "Sol/s " : "H/s ";
	return ret+unit;
}

QString Helpers::GetMotherboardID()
{
	return "";
}
/*
static inline void native_cpuid(unsigned int *eax, unsigned int *ebx,
                                unsigned int *ecx, unsigned int *edx)
{
        // ecx is often an input as well as an output
        asm volatile("cpuid"
            : "=a" (*eax),
              "=b" (*ebx),
              "=c" (*ecx),
              "=d" (*edx)
            : "0" (*eax), "2" (*ecx));
}*/
QString Helpers::GetCpuID()
{
	QString id="N/A";
	if (cpuid_present()) {
/*		struct cpu_raw_data_t raw;
		struct cpu_id_t data;
		if (cpuid_get_raw_data(&raw)<0) {
			return id;
			}
		if (cpu_identify(&raw, &data)<0) {
			return id;
			}
		printf("Found: %s CPU\n", data.vendor_str);
		printf("Processor model is `%s'\n", data.cpu_codename);
		printf("The full brand string is `%s'\n", data.brand_str);
		printf("The processor has %dK L1 cache and %dK L2 cache\n", data.l1_data_cache, data.l2_cache);
		printf("The processor has %d cores and %d logical processors\n", data.num_cores, data.num_logical_cpus);

		printf("Supported multimedia instruction sets:\n");
		printf("  MMX         : %s\n", data.flags[CPU_FEATURE_MMX] ? "present" : "absent");
		printf("  MMX-extended: %s\n", data.flags[CPU_FEATURE_MMXEXT] ? "present" : "absent");
		printf("  SSE         : %s\n", data.flags[CPU_FEATURE_SSE] ? "present" : "absent");
		printf("  SSE2        : %s\n", data.flags[CPU_FEATURE_SSE2] ? "present" : "absent");
		printf("  3DNow!      : %s\n", data.flags[CPU_FEATURE_3DNOW] ? "present" : "absent");
		printf("  serial #    : %s\n", data.flags[CPU_FEATURE_PN] ? "present" : "absent");

		printf("CPU clock is: %d MHz (according to your OS)\n", cpu_clock_by_os());
		printf("CPU clock is: %d MHz (tested)\n", cpu_clock_measure(200, 0));
*/		}
/*
	char PSN[30]; // ID: E3 06 05 00 FF FB EB BF
	int varEAX, varEBX, varECX, varEDX;
    char str[9];

    //%eax=1 gives most significant 32 bits in eax
    __asm__ __volatile__ ("cpuid"   : "=a" (varEAX), "=b" (varEBX), "=c" (varECX), "=d" (varEDX) : "a" (1));
    sprintf(str, "%08X", varEAX); //i.e. XXXX-XXXX-xxxx-xxxx-xxxx-xxxx
    sprintf(PSN, "%C%C%C%C-%C%C%C%C", str[0], str[1], str[2], str[3], str[4], str[5], str[6], str[7]);

    //%eax=3 gives least significant 64 bits in edx and ecx [if PN is enabled]
    __asm__ __volatile__ ("cpuid"   : "=a" (varEAX), "=b" (varEBX), "=c" (varECX), "=d" (varEDX) : "a" (3));
    sprintf(str, "%08X", varEDX); //i.e. xxxx-xxxx-XXXX-XXXX-xxxx-xxxx
    sprintf(PSN, "%s-%C%C%C%C-%C%C%C%C", PSN, str[0], str[1], str[2], str[3], str[4], str[5], str[6], str[7]);

    sprintf(str, "%08X", varECX); //i.e. xxxx-xxxx-xxxx-xxxx-XXXX-XXXX
    sprintf(PSN, "%s-%C%C%C%C-%C%C%C%C", PSN, str[0], str[1], str[2], str[3], str[4], str[5], str[6], str[7]);
*//*
	unsigned eax, ebx, ecx, edx;
	eax=3;
	native_cpuid(&eax, &ebx, &ecx, &edx);
	printf("serial number 0x%08x%08x\n", edx, ecx);
*/
	return id;
}

bool Helpers::WebRequestTestGoogle()
{
	QString url="http://www.google.com";
	try {
		QNetworkReply* reply;
		QEventLoop cLoop;
		QObject::connect(qnam, SIGNAL(finished(QNetworkReply*)), &cLoop, SLOT(quit()));
		QNetworkRequest myRequest=QNetworkRequest(url);
		QNetworkConfiguration conf=qnam->configuration();
		conf.setConnectTimeout(Globals::FirstNetworkCheckTimeoutTimeMs);
		qnam->setConfiguration(conf);
		reply=qnam->get(myRequest);
		cLoop.exec();
		reply->deleteLater();
		}
	catch (...) {
		return false;
		}
	return true;
}

bool Helpers::IsConnectedToInternet()
{
	return true;
/*	bool returnValue=false;
	try {
//		returnValue=InternetGetConnectedState(&Desc, 0);
		}
	catch (...) {
		returnValue=false;
		}
	return returnValue;*/
}

int Helpers::ParseInt(QString text)
{
	bool ok;
	int tmpVal=text.toInt(&ok);
	if (ok) {
		return tmpVal;
		}
	return 0;
}

long Helpers::ParseLong(QString text)
{
	bool ok;
	long tmpVal=text.toLong(&ok);
	if (ok) {
		return tmpVal;
		}
	return 0;
}

double Helpers::ParseDouble(QString text)
{
	try {
		bool ok;
		double tmpVal=text.replace(',', '.').toDouble(&ok);
		if (ok) {
			return tmpVal;
			}
		return 0;
		}
	catch (...) {
		return 0;
		}
}

void Helpers::SetDefaultEnvironmentVariables()
{
	ConsolePrint("NICEHASH", "Setting environment variables");

	QMap<QString, QString> envNameValues={
		{"GPU_MAX_ALLOC_PERCENT", "100"},
		{"GPU_USE_SYNC_OBJECTS", "1"},
		{"GPU_SINGLE_ALLOC_PERCENT", "100"},
		{"GPU_MAX_HEAP_SIZE", "100"},
//		{"GPU_FORCE_64BIT_PTR", "1"} // cuases problems with lots of miners
		};
	auto i=envNameValues.constBegin();
	QProcessEnvironment env=QProcessEnvironment::systemEnvironment();
	while (i!=envNameValues.constEnd()) {
		if (!env.contains(i.key()) || env.value(i.key())!=i.value()) {
			int ret=setenv(i.key().toStdString().c_str(), i.value().toStdString().c_str(), 1);
			if (ret) {
				ConsolePrint("NICEHASH", strerror(errno));
				}
			}
		++i;
		}
}

void Helpers::SetNvidiaP0State()
{
		try {
/*			ProcessStartInfo psi=new ProcessStartInfo;
			psi.FileName="nvidiasetp0state";
			psi.Verb="runas";
			psi.UseShellExecute=true;
			psi.CreateNoWindow=true;
			QProcess p=QProcess.start(psi);
			p.WaitForExit();
			if (p.exitCode()!=0) {
				ConsolePrint("NICEHASH", "nvidiasetp0state returned error code: "+QString::number(p.exicode));*/
/*			QProcess* psi=new QProcess(this);
			psi->setProgram("./nvidiasetp0state");
			psi->start();
			psi->waitForFinished();*/
			int exitcode=QProcess::execute("./nvidiasetp0state");
			if (exitcode) {
				ConsolePrint("NICEHASH", "nvidiasetp0state returned error code: "+QString::number(exitcode));
				}
			else {
				ConsolePrint("NICEHASH", "nvidiasetp0state all OK");
				}
			}
		catch (std::exception& ex) {
			ConsolePrint("NICEHASH", QString("nvidiasetp0state error: ")+ex.what());
			}
}

Enums::AlgorithmType Helpers::DualAlgoFromAlgos(Enums::AlgorithmType primary, Enums::AlgorithmType secondary)
{
	if (primary==Enums::AlgorithmType::DaggerHashimoto) {
		switch (secondary) {
			case Enums::AlgorithmType::Decred:
				return Enums::AlgorithmType::DaggerDecred;
			case Enums::AlgorithmType::Lbry:
				return Enums::AlgorithmType::DaggerLbry;
			case Enums::AlgorithmType::Pascal:
				return Enums::AlgorithmType::DaggerPascal;
			case Enums::AlgorithmType::Sia:
				return Enums::AlgorithmType::DaggerSia;
			case Enums::AlgorithmType::Blake2s:
				return Enums::AlgorithmType::DaggerBlake2s;
			case Enums::AlgorithmType::Keccak:
				return Enums::AlgorithmType::DaggerKeccak;
			default: // make compiler happy
				return primary;
			}
		}
	return primary;
}
