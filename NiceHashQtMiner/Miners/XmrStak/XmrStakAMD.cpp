#include "Miners/XmrStak/XmrStakAMD.h"
#include "Miners/Parsing/ExtraLaunchParametersParser.h"
#include "Devices/ComputeDevice/ComputeDevice.h"
#include "Utils/Helpers.h"
#include <QtConcurrent/QtConcurrentRun>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QIODevice>
#include <QFile>


XmrStakGPUSettings::XmrStakGPUSettings(int index, int intensity, int worksize/*=8*/, bool affine_to_cpu/*=false*/)
{
	this->index=index;
	this->intensity=intensity;
	this->worksize=worksize;
	this->affine_to_cpu=affine_to_cpu;
}

QJsonObject XmrStakGPUSettings::toJsonObject()
{
	QJsonObject ret;
	ret.insert("index", index);
	ret.insert("intensity", intensity);
	ret.insert("worksize", worksize);
	ret.insert("affine_to_cpu", affine_to_cpu);
	return ret;
}

XmrStakAMDConfig::XmrStakAMDConfig(QString poolAddr, QString wallet, int port)
	: XmrStakConfig(poolAddr, wallet, port)
{ }

void XmrStakAMDConfig::Initialize_gpu_threads_conf(QList<XmrStakGPUSettings*>* gpuSettings)
{
	gpu_threads_conf=new QList<QJsonObject>;
	foreach (XmrStakGPUSettings* settings, *gpuSettings) {
		gpu_threads_conf->append(settings->toJsonObject());
		}
	gpu_thread_num=gpuSettings->count();
}

QString XmrStakAMDConfig::toJsonString()
{
	QJsonObject o;
	o.insert("gpu_thread_num", gpu_thread_num);
	if (gpu_threads_conf!=nullptr) {
		QJsonArray gtc;
		foreach (QJsonObject c, *gpu_threads_conf) {
			gtc.append(c);
			}
		o.insert("gpu_threads_conf", gtc);
		}
	o.insert("platform_index", platform_index);
	return QString(QJsonDocument(o).toJson(QJsonDocument::JsonFormat::Indented));
}

XmrStakAMD::XmrStakAMD()
	: XmrStak("XmrStakAMD", 5*60*1000)
{
	_ConectionType=Enums::NhmConectionType::NONE;
//	IsNeverHideMiningWindow=true;
}

void XmrStakAMD::prepareConfigFile(QString pool, QString wallet)
{
	try {
		XmrStakAMDConfig* config=new XmrStakAMDConfig(pool, wallet, ApiPort());
		QList<XmrStakGPUSettings*>* gpuConfigs=new QList<XmrStakGPUSettings*>;
		foreach (MiningPair* pair, *_MiningSetup->MiningPairs) {
			QList<int>* intensities=ExtraLaunchParametersParser::GetIntensityStak(pair);
			if (intensities->count()<=0) {
				intensities->append(1000);
				}
			foreach (int intensity, *intensities) {
				gpuConfigs->append(new XmrStakGPUSettings(pair->Device->ID, intensity));
				}
			}
		config->Initialize_gpu_threads_conf(gpuConfigs);
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

ApiData* XmrStakAMD::GetSummaryAsync()
{
	ApiData* ad=new ApiData(_MiningSetup->CurrentAlgorithmType);

	QString DataToSend=GetHttpRequestNhmAgentStrin("h");

	QString* resp=QtConcurrent::run(this, static_cast<QString*(XmrStakAMD::*)(int, QString, bool, bool)>(&Miner::GetApiDataAsync), ApiPort(), DataToSend, false, true).result();
	if (resp==nullptr) {
		Helpers::ConsolePrint(MinerTag(), ProcessTag()+" summary is null");
		CurrentMinerReadStatus=Enums::MinerApiReadStatus::NONE;
		return nullptr;
		}
	const QString Totals="Totals:";
	const QString Highest="Highest:";
	int start_i=resp->indexOf(Highest);
	int end_i=resp->indexOf(Highest);
	if (start_i>-1 && end_i>-1) {
		QString sub_resp=resp->mid(start_i, end_i-start_i)
			.remove(Totals)
			.remove(Highest)
			.remove(QRegExp("<.*?>")); // Remove HTML tags
		foreach (QString s, sub_resp.split(' ', QString::SplitBehavior::SkipEmptyParts)) {
			bool ok=false;
			double speed=s.toDouble(&ok);
			if (ok) {
				CurrentMinerReadStatus=Enums::MinerApiReadStatus::GOT_READ;
				ad->Speed=speed;
				break;
				}
			}
		}
	// check if speed zero
	if (!ad->Speed) {
		CurrentMinerReadStatus=Enums::MinerApiReadStatus::READ_SPEED_ZERO;
		}

	return ad;
}
