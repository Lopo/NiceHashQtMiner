#include "Miners/ethminer/MinerEtherumOCL.h"
#include "Devices/ComputeDeviceManager.h"
#include "Utils/Helpers.h"
#include "Miners/Parsing/ExtraLaunchParametersParser.h"


QList<MinerEtherum*>* MinerEtherumOCL::MinerEtherumOclList=new QList<MinerEtherum*>;


MinerEtherumOCL::MinerEtherumOCL()
	: MinerEtherum("MinerEtherumOCL", "AMD OpenCL")
{
	_gpuPlatformNumber=ComputeDeviceManager.Avaliable.AmdOpenCLPlatformNum;
	MinerEtherumOclList->append(this);
}

MinerEtherumOCL::~MinerEtherumOCL()
{
	// remove from list
	MinerEtherumOclList->removeAll(this);
}

void MinerEtherumOCL::Start(QString url, QString btcAdress, QString worker)
{
	Helpers::ConsolePrint(MinerTag(), "Starting MinerEtherumOCL, checking existing MinerEtherumOCL to stop");
	MinerEtherum::Start(url, btcAdress, worker, *MinerEtherumOclList);
}

QStringList MinerEtherumOCL::GetStartCommandStringPart(QString url, QString username)
{
	QStringList ret;
	ret << "--opencl" << "--opencl-platform" << QString::number(_gpuPlatformNumber)
			<< ExtraLaunchParametersParser::ParseForMiningSetup(MiningSetup_, Enums::DeviceType::AMD)
			<< "-S" << url.mid(14)
			<< "-O" << username+":x"
			<< "-SP" << QString::number(2)
			<< "--api-port" << QString::number(ApiPort())
			<< "--opencl-devices";
	return ret;
}

QStringList MinerEtherumOCL::GetBenchmarkCommandStringPart(Algorithm* algorithm)
{
	QStringList ret;
	ret << "--opencl" << "--opencl-platform" << QString::number(_gpuPlatformNumber)
			<< ExtraLaunchParametersParser::ParseForMiningSetup(MiningSetup_, Enums::DeviceType::AMD)
			<< "--benchmark-warmup" << QString::number(40) << "--benchmark-trial" << QString::number(20)
			<< "--opencl-devices";
	return ret;	
}
