#include "Miners/Equihash/ClaymoreZcashMiner.h"


const QString ClaymoreZcashMiner::LookForStart="ZEC - Total Speed:";


ClaymoreZcashMiner::ClaymoreZcashMiner()
	: ClaymoreBaseMiner("ClaymoreZcashMiner", LookForStart, 60*1000*5)
{
	IgnoreZero=true;
}

double ClaymoreZcashMiner::DevFee()
{
	return 2.0;
}

void ClaymoreZcashMiner::Start(QString url, QString btcAdress, QString worker)
{
	QString username=GetUsername(btcAdress, worker);
	LastCommandLine=GetDevicesCommandString() << "-mport" << "127.0.0.1:-"+QString::number(ApiPort()) << "-zpool" << url
			<< "-zwal" << username << "-zpsw" << "x" << "-dbg" << "-1";
	ProcessHandle=_Start();
}

QStringList ClaymoreZcashMiner::BenchmarkCreateCommandLine(Algorithm* algorithm, int time)
{
	BenchmarkTimeWait=time/3; // 3 times faster than sgminer

	return QStringList() << "-mport" << "127.0.0.1:"+QString::number(ApiPort()) << "-benchmark" << "1" << GetDevicesCommandString();
}
