#include "Miners/Equihash/Ewbf144.h"


Ewbf144::Ewbf144()
	: Ewbf("ewbf144")
{ }

QStringList Ewbf144::GetStartCommand(QString url, QString btcAddress, QString worker)
{
	QStringList command=Ewbf::GetStartCommand(url, btcAddress, worker);
	command.append({"--algo", "144_5", "--pers", "auto"});
	return command;
}
