#include "CPUID.h"
#include "config.h"
#include "Utils/Helpers.h"
#include <QDir>
#include <QFile>
#include <QProcess>
#include <libcpuid/libcpuid.h>

using namespace NiceHashQtMiner;


//QLibrary* CPUID::lib=nullptr;

/*
bool CPUID::Init()
{
	if (lib==nullptr) {
		lib=new QLibrary("libcpuid.so");
		}
	if (!lib->isLoaded()) {
		Helpers::ConsolePrint("NICEHASH", "Unable to load libcpuid");
		return false;
		}
	return true;
}
*/
QString CPUID::GetCPUName()
{
	if (!cpuid_present()) {
		return QString("");
		}

	struct cpu_raw_data_t raw;
	struct cpu_id_t data;
	if (cpuid_get_raw_data(&raw)<0 || cpu_identify(&raw, &data)<0) {
		return QString("");
		}
	return QString(data.brand_str);
}

QString CPUID::GetCPUVendor()
{
	if (!cpuid_present()) {
		return QString("");
		}

	struct cpu_raw_data_t raw;
	struct cpu_id_t data;
	if (cpuid_get_raw_data(&raw)<0 || cpu_identify(&raw, &data)<0) {
		return QString("");
		}
	return QString(data.vendor_str);
}

int CPUID::SupportsSSE2()
{
	if (!cpuid_present()) {
		return 0;
		}
	struct cpu_raw_data_t raw;
	struct cpu_id_t data;
	if (cpuid_get_raw_data(&raw)<0 || cpu_identify(&raw, &data)<0) {
		return 0;
		}
	return data.flags[CPU_FEATURE_SSE];
}

int CPUID::SupportsAVX()
{
	if (!cpuid_present()) {
		return 0;
		}
	struct cpu_raw_data_t raw;
	struct cpu_id_t data;
	if (cpuid_get_raw_data(&raw)<0 || cpu_identify(&raw, &data)<0) {
		return 0;
		}
	return data.flags[CPU_FEATURE_AVX];
}

int CPUID::SupportsAVX2()
{
	if (!cpuid_present()) {
		return 0;
		}
	struct cpu_raw_data_t raw;
	struct cpu_id_t data;
	if (cpuid_get_raw_data(&raw)<0 || cpu_identify(&raw, &data)<0) {
		return 0;
		}
	return data.flags[CPU_FEATURE_AVX2];
}

int CPUID::SupportsAES()
{
	if (!cpuid_present()) {
		return 0;
		}
	struct cpu_raw_data_t raw;
	struct cpu_id_t data;
	if (cpuid_get_raw_data(&raw)<0 || cpu_identify(&raw, &data)<0) {
		return 0;
		}
	return data.flags[CPU_FEATURE_AES];
}

int CPUID::GetPhysicalProcessorCount()
{
	return QDir("/sys/devices/system/node").entryList(QDir::NoDotAndDotDot|QDir::Dirs).filter(QRegExp("node\\d+")).count();
}

int CPUID::GetVirtualCoresCount()
{
	if (!cpuid_present()) {
		return 0;
		}
	return cpuid_get_total_cpus();
}

int CPUID::GetNumberOfCores()
{
	struct cpu_raw_data_t raw;
	struct cpu_id_t data;
	if (cpuid_get_raw_data(&raw)<0 || cpu_identify(&raw, &data)<0) {
		return 0;
		}
	return data.num_cores;
}

bool CPUID::IsHyperThreadingEnabled()
{
	return GetVirtualCoresCount()>GetNumberOfCores();
}

ulong CPUID::CreateAffinityMask(int index, int percpu)
{
	ulong mask=0;
	ulong one=0x0000000000000001;
	for (int i=index*percpu; i<(index+1)*percpu; i++) {
		mask=mask|(one<<i);
		}
	return mask;
}

void CPUID::AdjustAffinity(int pid, ulong mask)
{
	QProcess* ProcessHandle=new QProcess;
	ProcessHandle->setProgram("setcpuaff");
	ProcessHandle->setArguments(QStringList() << QString::number(pid) << " " << QString::number(mask));
	ProcessHandle->start();
}
