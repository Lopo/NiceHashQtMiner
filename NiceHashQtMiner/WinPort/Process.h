#ifndef PROCESS_H
#define PROCESS_H

#include <sys/types.h>
#include <QString>
#include <QList>


class Process
{
public:
	static QList<pid_t>* GetProcessesByName(QString procName);
	static bool CheckProcessById(int processId);
};

#endif
