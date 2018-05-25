#include <qt/QtCore/qjsondocument.h>

#include "Miners/XmrStak/Configs/XmrStakConfig.h"


void XmrStakConfig::SetBenchmarkOptions(QString logFile)
{
	output_file=logFile;
	h_print_time=1;
	daemon_mode=true;
	flush_stdout=true;
	verbose_level=4;
}

QJsonObject XmrStakConfig::asJsonObject()
{
	QJsonObject o;
	o.insert("call_timeout", call_timeout);
	o.insert("retry_time", retry_time);
	o.insert("giveup_limit", giveup_limit);
	o.insert("verbose_level", verbose_level);
	o.insert("print_motd", print_motd);
	o.insert("h_print_time", h_print_time);
	o.insert("aes_override", aes_override==Qt::PartiallyChecked? QJsonValue() : QJsonValue((aes_override==Qt::Checked)));
	o.insert("use_slow_memory", use_slow_memory);
	o.insert("tls_secure_algo", tls_secure_algo);
	o.insert("daemon_mode", daemon_mode);
	o.insert("flush_stdout", flush_stdout);
	o.insert("output_file", output_file);
	o.insert("httpd_port", httpd_port);
	o.insert("http_login", http_login);
	o.insert("http_pass", http_pass);
	o.insert("prefer_ipv4", prefer_ipv4);
	return o;
}

XmrStakConfig* XmrStakConfig::fromJson(QString json)
{
	QJsonObject o=QJsonDocument::fromJson(json.toLatin1()).object();
	XmrStakConfig* ret=new XmrStakConfig;
	ret->call_timeout=o.value("call_timeout").toInt();
	ret->retry_time=o.value("retry_time").toInt();
	ret->giveup_limit=o.value("giveup_limit").toInt();
	ret->verbose_level=o.value("verbose_level").toInt();
	ret->print_motd=o.value("print_motd").toBool();
	ret->h_print_time=o.value("h_print_time").toInt();
	ret->aes_override=o.value("aes_override").isNull()? Qt::PartiallyChecked : (o.value("aes_override").toBool()? Qt::Checked : Qt::Unchecked);
	ret->use_slow_memory=o.value("use_slow_memory").toString();
	ret->tls_secure_algo=o.value("tls_secure_algo").toBool();
	ret->daemon_mode=o.value("daemon_mode").toBool();
	ret->flush_stdout=o.value("flush_stdout").toBool();
	ret->output_file=o.value("output_file").toString();
	ret->httpd_port=o.value("httpd_port").toInt();
	ret->http_login=o.value("http_login").toString();
	ret->http_pass=o.value("http_pass").toString();
	ret->prefer_ipv4=o.value("prefer_ipv4").toBool();
	return ret;
}
