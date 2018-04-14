#include "Miners/XmrStak/XmrStackCPUMiner.h"
#include "PInvoke/NiceHashProcess.h"
#include "Miners/Parsing/ExtraLaunchParametersParser.h"
#include "Utils/Helpers.h"
#include "PInvoke/CPUID.h"
#include "Globals.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Algorithm.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QList>


using namespace NiceHashQtMiner;


XmrStackCPUMinerConfig::XmrStackCPUMinerConfig(int numberOfthreads, QString poolAddr, QString wallet, int port)
	: XmrStakConfig(poolAddr, wallet, port)
{
	cpu_thread_num=numberOfthreads;
}

void XmrStackCPUMinerConfig::Inti_cpu_threads_conf(bool low_power_mode, bool no_prefetch, bool affine_to_cpu, bool isHyperThreading)
{
//	if (cpu_threads_conf!=nullptr) {delete cpu_threads_conf;}
	cpu_threads_conf=new QList<QJsonObject>;
	if (isHyperThreading) {
		for (int i_cpu=0; i_cpu<cpu_thread_num; ++i_cpu) {
			QJsonObject o;
			o.insert("low_power_mode", low_power_mode);
			o.insert("no_prefetch", no_prefetch);
			o.insert("affine_to_cpu", i_cpu*2);
			cpu_threads_conf->append(o);
			}
		}
	else {
		for (int i_cpu=0; i_cpu<cpu_thread_num; ++i_cpu) {
			if (affine_to_cpu) {
				QJsonObject o;
				o.insert("low_power_mode", low_power_mode);
				o.insert("no_prefetch", no_prefetch);
				o.insert("affine_to_cpu", i_cpu);
				cpu_threads_conf->append(o);
				}
			else {
				QJsonObject o;
				o.insert("low_power_mode", low_power_mode);
				o.insert("no_prefetch", no_prefetch);
				o.insert("affine_to_cpu", false);
				cpu_threads_conf->append(o);
				}
			}
		}
}

QString XmrStackCPUMinerConfig::toJsonString()
{
	QJsonObject obj;
	obj.insert("cpu_thread_num", cpu_thread_num);
	QJsonArray a;
	foreach (QJsonObject o, *cpu_threads_conf) {
		a.append(o);
		}
	obj.insert("cpu_threads_conf", a);
	obj.insert("use_slow_memory", use_slow_memory);
	obj.insert("nicehash_nonce", nicehash_nonce);
	obj.insert("aes_override", (aes_override==nullptr? QJsonValue(): *aes_override));
	obj.insert("call_timeout", call_timeout);
	obj.insert("retry_time", retry_time);
	obj.insert("giveup_limit", giveup_limit);
	obj.insert("verbose_level", verbose_level);
	obj.insert("h_print_time", h_print_time);
	obj.insert("daemon_mode", daemon_mode);
	obj.insert("output_file", output_file);
	obj.insert("httpd_port", httpd_port);
	obj.insert("http_login", http_login);
	obj.insert("http_pass", http_pass);
	obj.insert("prefer_ipv4", prefer_ipv4);
	obj.insert("tls_secure_algo", tls_secure_algo);
	obj.insert("print_motd", print_motd);
	obj.insert("flush_stdout", flush_stdout);
	obj.insert("currency", currency);

	QJsonObject pool;
	pool.insert("pool_address", pool_address);
	pool.insert("wallet_address", wallet_address);
	pool.insert("rig_id", "rig");
	pool.insert("pool_password", pool_password);
	pool.insert("use_nicehash", true);
	pool.insert("use_tls", use_tls);
	pool.insert("tls_fingerprint", tls_fingerprint);
	pool.insert("pool_weight", 1);

	obj.insert("pool_list", QJsonArray({pool}));
	return QJsonDocument(obj).toJson();
}


XmrStackCPUMiner::XmrStackCPUMiner()
	: XmrStak("XmrStackCPUMiner", 3600000)
{
}

void XmrStackCPUMiner::prepareConfigFile(QString pool, QString wallet)
{
	if (this->_MiningSetup->MiningPairs->count()>0) {
		try {
			bool IsHyperThreadingEnabled=this->_MiningSetup->MiningPairs->at(0)->CurrentExtraLaunchParameters.contains("enable_ht=true");
			int numTr=ExtraLaunchParametersParser::GetThreadsNumber(this->_MiningSetup->MiningPairs->at(0));
			if (IsHyperThreadingEnabled) {
				numTr/=2;
				}
			XmrStackCPUMinerConfig* config=new XmrStackCPUMinerConfig(numTr, pool, wallet, this->ApiPort());
			bool no_prefetch=ExtraLaunchParametersParser::GetNoPrefetch(_MiningSetup->MiningPairs->at(0));
			config->Inti_cpu_threads_conf(false, no_prefetch, false, IsHyperThreadingEnabled);
			config->currency=_MiningSetup->MinerName;
			QString writeStr=config->toJsonString();
			int start=writeStr.indexOf('{');
			int end=writeStr.lastIndexOf('}');
			writeStr=writeStr.mid(start+1, end-1);
			QFile f(WorkingDirectory()+GetConfigFileName());
			f.open(QIODevice::Text|QIODevice::WriteOnly);
			f.write(writeStr.toLatin1());
			f.close();
			}
		catch (...) {}
		}
}

NiceHashProcess* XmrStackCPUMiner::_Start()
{
	NiceHashProcess* P=XmrStak::_Start();

	ulong AffinityMask=_MiningSetup->MiningPairs->at(0)->Device->AffinityMask();
	if (AffinityMask!=0 && P!=nullptr) {
		CPUID::AdjustAffinity(P->pid(), AffinityMask);
		}

	return P;
}

QProcess* XmrStackCPUMiner::BenchmarkStartProcess(QStringList CommandLine)
{
	QProcess* BenchmarkHandle=Miner::BenchmarkStartProcess(CommandLine);

	ulong AffinityMask=_MiningSetup->MiningPairs->at(0)->Device->AffinityMask();
	if (AffinityMask!=0 && BenchmarkHandle!=nullptr) {
		CPUID::AdjustAffinity(BenchmarkHandle->pid(), AffinityMask);
		}

	return BenchmarkHandle;
}
