#include "IFTTT.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Utils/Helpers.h"
#include <QException>


void Ifttt::PostToIfttt(QString action, QString msg)
{
	try {
		QString key=ConfigManager.generalConfig->IFTTTKey;
		QString worker=ConfigManager.generalConfig->WorkerName;
		QString minProfit=QString::number(ConfigManager.generalConfig->MinimumProfit, 'f', 2).replace(',', '.');

		
		}
	catch (QException ex) {
		Helpers::ConsolePrint("NICEHASH", ex.what());
		}
}
