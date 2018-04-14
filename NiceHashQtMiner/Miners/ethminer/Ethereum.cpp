#include "Miners/ethminer/Ethereum.h"
#include "Stats/NiceHashStats.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"


QString Ethereum::CurrentBlockNum="";


void Ethereum::GetCurrentBlock(QString worker)
{
	QString ret=NiceHashStats::GetNiceHashApiData("https://etherchain.org/api/blocks/count", worker);
	if (ret==nullptr) {
		Helpers::ConsolePrint(worker, QString("Failed to obtain current block, using default %1.").arg(ConfigManager.generalConfig->ethminerDefaultBlockHeight));
		}
	else {
		ret=ret.mid(ret.lastIndexOf("count")+7);
		CurrentBlockNum=ret.mid(0, ret.length()-3);
		}
}
