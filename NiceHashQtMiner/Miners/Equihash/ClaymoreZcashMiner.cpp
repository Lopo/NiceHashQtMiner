#include "Miners/Equihash/ClaymoreZcashMiner.h"


ClaymoreZcashMiner::ClaymoreZcashMiner()
	: ClaymoreBaseMiner("ClaymoreZcashMiner", 60*1000*5)
{
	IgnoreZero=true;
	LookForStart="zec - total speed:";
	DevFee=2.0;
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

	return QStringList() << "-mport" << "127.0.0.1:"+QString::number(ApiPort()) << "-benchmark" << "1" << "-logfile" << GetLogFileName() << GetDevicesCommandString();
}
