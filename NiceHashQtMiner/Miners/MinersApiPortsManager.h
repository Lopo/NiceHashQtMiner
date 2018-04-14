#ifndef MINERS_MINERSAPIPORTSMANAGER_H_
#define MINERS_MINERSAPIPORTSMANAGER_H_

#include <QObject>


class MinersApiPortsManager
	: public QObject
{
	Q_OBJECT
private:
	static QSet<int>* UsedPorts;

public:
	static bool IsPortAvailable(int port);
	static int GetAvailablePort();
	static void RemovePort(int port);
};

#endif
