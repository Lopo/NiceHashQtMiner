#ifndef MINERS_XMRSTAK_XMRSTAKCONFIG_H_
#define MINERS_XMRSTAK_XMRSTAKCONFIG_H_

#include <QString>


class XmrStakConfig
{
public:
	XmrStakConfig(QString poolAddr, QString wallet, int port);

	bool use_tls=false;
	bool tls_secure_algo=true;
	QString tls_fingerprint="";

	QString pool_address;
	QString wallet_address;
	QString pool_password="x";

	int call_timeout=10;
	int retry_time=10;
	int giveup_limit=10;

	int verbose_level=4;

	int h_print_time=60;

	bool daemon_mode=false;

	QString output_file="";

	int httpd_port=0;
	QString http_login="";
	QString http_pass="";

	bool prefer_ipv4=true;

	bool print_motd=false;
	bool flush_stdout=false;
};

#endif
