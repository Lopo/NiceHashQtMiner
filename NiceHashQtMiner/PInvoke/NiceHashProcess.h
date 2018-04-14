#ifndef NICEHASHPROCESS_H
#define NICEHASHPROCESS_H

#include <QProcess>


class NiceHashProcess
	: public QProcess
{
public:
	void Kill();
	void Close();
	bool Start();

	QFunctionPointer ExitEvent=nullptr;
	uint32_t ExitCode;
	qint64 Id;

private:
	QThread* tHandle=nullptr;
	bool bRunning;
	QProcess* pHandle=nullptr;

	void cThread();
};

#endif
