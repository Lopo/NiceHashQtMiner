#ifndef MINERS_XMRSTAK_CONFIGS_XMRSTAKCONFIG_H_
#define MINERS_XMRSTAK_CONFIGS_XMRSTAKCONFIG_H_

#include "Miners/XmrStak/Configs/IasJsonObject.h"
#include <QString>


class XmrStakConfig
	: public IasJsonObject
{
public:
	void SetBenchmarkOptions(QString logFile);

	int call_timeout=10;
	int retry_time=10;
	int giveup_limit=10;

	int verbose_level=4;
	bool print_motd=true;

	int h_print_time=60;

	Qt::CheckState aes_override=Qt::PartiallyChecked;

	QString use_slow_memory="warn";

	bool tls_secure_algo=true;

	bool daemon_mode=false;

	bool flush_stdout=false;

	QString output_file="";

	int httpd_port=0;

	QString http_login="";
	QString http_pass="";

	bool prefer_ipv4=true;

	QJsonObject asJsonObject() override;
	static XmrStakConfig* fromJson(QString json);
};

#endif
