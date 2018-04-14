#include <QTcpSocket>
#include <QUdpSocket>

#include "MinersApiPortsManager.h"
#include "MinersSettingsManager.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"


QSet<int>* MinersApiPortsManager::UsedPorts=new QSet<int>;


bool MinersApiPortsManager::IsPortAvailable(int port)
{
	// check TCP
	QTcpSocket s;
	bool isAvailable=s.bind(port, QAbstractSocket::DontShareAddress);
	s.close();
	if (isAvailable) {
		QUdpSocket u;
		isAvailable=u.bind(port, QAbstractSocket::DontShareAddress);
		u.close();
		}
	return isAvailable;
}

int MinersApiPortsManager::GetAvailablePort()
{
	int port=ConfigManager.generalConfig->ApiBindPortPoolStart;
	int newPortEnd=port+3000;
	for ( ; port<newPortEnd; ++port) {
		if (!MinersSettingsManager::AllReservedPorts->contains(port) && IsPortAvailable(port) && !UsedPorts->contains(port)) {
			UsedPorts->insert(port);
			break;
			}
		}
	return port;
}

void MinersApiPortsManager::RemovePort(int port)
{
	UsedPorts->remove(port);
}
