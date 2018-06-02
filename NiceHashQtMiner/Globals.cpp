#include "Globals.h"
#include "Utils/BitcoinAddress.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Switching/Models.h"
#include "Switching/NHSmaData.h"


const QVector<QString> Globals::MiningLocation={"eu", "usa", "hk", "jp", "in", "br"};
QString Globals::DemoUser="1QATWNM7AePyHMGuQxBLTuq76avoDoWsFc";
int Globals::CurrentTosVer=3;
int Globals::ThreadsPerCpu=0;
bool Globals::IsFirstNetworkCheckTimeout=true;
int Globals::FirstNetworkCheckTimeoutTimeMs=500;
int Globals::FirstNetworkCheckTimeoutTries=10;


QString Globals::GetLocationURL(Enums::AlgorithmType algorithmType, QString miningLocation, Enums::NhmConectionType connectionType)
{
	NiceHashSma sma;
	if (!NHSmaData::TryGetSma(algorithmType, sma)) {
		return "";
		}

	QString name=sma.Name;
	int nPort=sma.Port;
	int sslPort=30000+nPort;

	// NHMConnection.NONE
	QString prefix="";
	int port=nPort;
	switch (connectionType) {
		case Enums::NhmConectionType::LOCKED:
			return miningLocation;
		case Enums::NhmConectionType::STRATUM_TCP:
			prefix="stratum+tcp://";
			break;
		case Enums::NhmConectionType::STRATUM_SSL:
			prefix="stratum+ssl://";
			port=sslPort;
			break;
		default:
			break;
		}

	return prefix+name+"."+miningLocation+".nicehash.com:"+QString::number(port);
}

QString Globals::GetBitcoinUser()
{
	return BitcoinAddress::ValidateBitcoinAddress(ConfigManager.generalConfig->BitcoinAddress.trimmed())
		? ConfigManager.generalConfig->BitcoinAddress.trimmed()
		: DemoUser;
}
