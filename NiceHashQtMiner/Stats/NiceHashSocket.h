#ifndef STATS_NICEHASHSOCKET_H_
#define STATS_NICEHASHSOCKET_H_

#include <QObject>
#include <QtWebSockets/QWebSocket>
#include <QJsonObject>
#include <QJsonDocument>


class NiceHashSocket
	: public QObject
{
	Q_OBJECT
public:
	NiceHashSocket(QString address);

//	inline bool IsAlive() const { return _webSocket.state()==QAbstractSocket::SocketState::ConnectedState; };
	bool IsAlive();

	void StartConnection();

	bool SendData(QString data, bool recurs=false);

private:
	class NicehashLogin {
	public:
		QString method="login";
		QString version;
		int protocol=1;

		QString asJson() {
			QJsonObject o;
			o.insert("method", method);
			o.insert("version", version);
			o.insert("protocol", protocol);
			return QJsonDocument(o).toJson();
			};
		};

	QWebSocket* _webSocket=nullptr;

	bool _attemptingReconnect=false;
	bool _connectionAttempted=false;
	bool _connectionEstablished=false;
	QString _address;

	bool AttemptReconnect();

Q_SIGNALS:
	void ConnectionEstablished();
	void DataReceived(QString);
	void ConnectionLost();

private Q_SLOTS:
	void ConnectCallback();
	void ReceiveCallback(QString e);
	void ErrorCallback(QAbstractSocket::SocketError e);
	void CloseCallback();
};

#endif
