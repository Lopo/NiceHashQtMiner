//#include <QProcess>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <unistd.h>
#include <thread>
#include <sched.h>
#include "CPUID.h"
#include "Utils/Helpers.h"
#include "libcpuid/libcpuid.h"

using namespace NiceHashQtMiner;


QLibrary* CPUID::lib=nullptr;

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

/*
 * Allocates a new set for ncpus and returns size in bytes and size in bits
 */
static cpu_set_t *cpuset_alloc(int ncpus, size_t *setsize, size_t *nbits)
{
	cpu_set_t *set = CPU_ALLOC(ncpus);

	if (!set)
		return NULL;
	if (setsize)
		*setsize = CPU_ALLOC_SIZE(ncpus);
	if (nbits)
		*nbits = 8*CPU_ALLOC_SIZE(ncpus);
	return set;
}

static const char *nexttoken(const char *q,  int sep)
{
	if (q)
		q = strchr(q, sep);
	if (q)
		q++;
	return q;
}
/*
 * Parses string with list of CPU ranges.
 * Returns 0 on success.
 * Returns 1 on error.
 * Returns 2 if fail is set and a cpu number passed in the list doesn't fit
 * into the cpu_set. If fail is not set cpu numbers that do not fit are
 * ignored and 0 is returned instead.
 */
static int cpulist_parse(const QString str, cpu_set_t *set, size_t setsize, int fail)
{
	size_t max = setsize*8;
	const char *p, *q;
	int r = 0;

	q = str.toStdString().c_str();
	CPU_ZERO_S(setsize, set);

	while (p = q, q = nexttoken(q, ','), p) {
		unsigned int a;	/* beginning of range */
		unsigned int b;	/* end of range */
		unsigned int s;	/* stride */
		const char *c1, *c2;
		char c;

		if ((r = sscanf(p, "%u%c", &a, &c)) < 1)
			return 1;
		b = a;
		s = 1;

		c1 = nexttoken(p, '-');
		c2 = nexttoken(p, ',');
		if (c1 != NULL && (c2 == NULL || c1 < c2)) {
			if ((r = sscanf(c1, "%u%c", &b, &c)) < 1)
				return 1;
			c1 = nexttoken(c1, ':');
			if (c1 != NULL && (c2 == NULL || c1 < c2)) {
				if ((r = sscanf(c1, "%u%c", &s, &c)) < 1)
					return 1;
				if (s == 0)
					return 1;
			}
		}

		if (!(a <= b))
			return 1;
		while (a <= b) {
			if (fail && (a >= max))
				return 2;
			CPU_SET_S(a, setsize, set);
			a += s;
		}
	}

	if (r == 2)
		return 1;
	return 0;
}

static cpu_set_t * path_cpuparse(int maxcpus, int islist, const char *path)
{
	cpu_set_t *set;
	size_t setsize;
//	char buf[len];

	QFile fd(path);
	fd.open(QIODevice::ReadOnly|QIODevice::Text);
	QString buf(fd.readLine(maxcpus*7).trimmed());
	fd.close();

	set=cpuset_alloc(maxcpus, &setsize, NULL);
	if (!set) {
		Helpers::ConsolePrint("NICEHASH", "failed to callocate cpu set");
		return nullptr;
		}

	if (islist) {
		if (cpulist_parse(buf, set, setsize, 0)) {
			Helpers::ConsolePrint("NICEHASH", "failed to parse CPU list "+buf);
			return nullptr;
			}
		}
/*	else {
		if (cpumask_parse(buf, set, setsize)) {
			errx(EXIT_FAILURE, _("failed to parse CPU mask %s"), buf);
			}
		}*/
	return set;
}

int CPUID::GetVirtualCoresCount()
{
//	return std::thread::hardware_concurrency();
//	return cpuid_get_total_cpus();
	int coreCount;

	int maxcpus=2048;
	size_t setsize=CPU_ALLOC_SIZE(maxcpus);
	coreCount=CPU_COUNT_S(setsize, path_cpuparse(maxcpus, true, "/sys/devices/system/cpu/present"));
	return coreCount;
}

int CPUID::GetNumberOfCores()
{
	return 4;
	int coreCount=0;
//	foreach (item, )
	return coreCount;
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
