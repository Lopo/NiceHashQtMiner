#include "Miners/XmrStak/XmrStakConfig.h"


XmrStakConfig::XmrStakConfig(QString poolAddr, QString wallet, int port)
{
	pool_address=poolAddr;
	wallet_address=wallet;
	httpd_port=port;
}
