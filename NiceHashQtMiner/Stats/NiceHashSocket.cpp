#include "Stats/NiceHashSocket.h"
#include "Utils/Helpers.h"
#include "Stats/NiceHashStats.h"
#include "Switching/NHSmaData.h"
#include <QApplication>
#include <QException>
#include <QMetaEnum>
#include <QThread>

#include <iostream>


bool NiceHashSocket::IsAlive()
{
	return _webSocket->isValid();
}

NiceHashSocket::NiceHashSocket(QString address)
{
	_address=address;
}

void NiceHashSocket::StartConnection()
{
	_connectionAttempted=true;
	try {
		if (_webSocket==nullptr) {
			_webSocket=new QWebSocket(_address);
			}
		else {
			_webSocket->close();
			}
		connect(_webSocket, SIGNAL(connected()), this, SLOT(ConnectCallback()));
		connect(_webSocket, SIGNAL(textMessageReceived(QString)), this, SLOT(ReceiveCallback(QString)));
		connect(_webSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(ErrorCallback(QAbstractSocket::SocketError)));
		connect(_webSocket, SIGNAL(disconnected()), this, SLOT(CloseCallback()));
		_webSocket->open(QUrl(_address));
		_connectionEstablished=true;
		}
	catch (QException e) {
		Helpers::ConsolePrint("SOCKET", e.what());
		}
}

void NiceHashSocket::ConnectCallback()
{
	try {
		NHSmaData::InitializeIfNeeded();
		// send login
		NicehashLogin login;
		login.version="NHQM/"+qApp->applicationVersion();
		SendData(login.asJson());

		emit ConnectionEstablished();
		}
	catch (QException er) {
		Helpers::ConsolePrint("SOCKET", er.what());
		}
}

void NiceHashSocket::ReceiveCallback(QString e)
{
	emit DataReceived(e);
}

void NiceHashSocket::ErrorCallback(QAbstractSocket::SocketError e)
{
	Helpers::ConsolePrint("SOCKET", QString(QMetaEnum::fromType<QAbstractSocket::SocketError>().valueToKey(e))+": "+_webSocket->errorString());
}

void NiceHashSocket::CloseCallback()
{
	Helpers::ConsolePrint("SOCKET", QString("Connection closed code").arg(_webSocket->closeCode()).arg(_webSocket->closeReason()));
	AttemptReconnect();
}

// Don't call SendData on UI threads, since it will block the thread for a bit if a reconnect is needed
bool NiceHashSocket::SendData(QString data, bool recurs/*=false*/)
{
	try {
		if (_webSocket!=nullptr && IsAlive()) {
			// Make sure connection is open
			// Verify valid JSON and method
			QJsonObject dataJson=QJsonDocument::fromJson(data.toLatin1()).object();
			QString method=dataJson.value("method").toString();
			if (method=="credentials.set" || method=="devices.status" || method=="login") {
				Helpers::ConsolePrint("SOCKET", "Sending data: "+data);
				_webSocket->sendTextMessage(data);
				return true;
				}
			}
		else if (_webSocket!=nullptr) {
			if (AttemptReconnect() && !recurs) {
				// Reconnect was successful, send data again (safety to prevent recursion overload)
				SendData(data, true);
				}
			else {
				Helpers::ConsolePrint("SOCKET", "Socket connection unsuccessfull, will try again on next device update (1min)");
				}
			}
		else {
			if (!_connectionAttempted) {
				Helpers::ConsolePrint("SOCKET", "Data sending attempted before socket initialization");
				}
			else {
				Helpers::ConsolePrint("SOCKET", "webSocket not created, retrying");
				StartConnection();
				}
			}
		}
	catch (QException er) {
		Helpers::ConsolePrint("SOCKET", er.what());
		}
	return false;
}

bool NiceHashSocket::AttemptReconnect()
{
	if (_attemptingReconnect) {
		return false;
		}
	if (IsAlive()) {
		// no reconnect needed
		return true;
		}
	_attemptingReconnect=true;
	int sleep=_connectionEstablished? 10+(qrand()%(20+1)) : 0;
	Helpers::ConsolePrint("SOCKET", QString("Attempting reconnect in %1 seconds").arg(sleep));
	// More retries on first attempt
	int retries=_connectionEstablished? 5 : 25;
	if (_connectionEstablished) {
		// Don't wait if no connection yet
		QThread::msleep(sleep*1000);
		}
	else {
		// Don't not wait again
		_connectionEstablished=true;
		}
	for (int i=0; i<retries; i++) {
		_webSocket->open(_webSocket->requestUrl());
		QThread::msleep(100);
		if (IsAlive()) {
			_attemptingReconnect=false;
			return true;
			}
		QThread::msleep(1000);
		}
	_attemptingReconnect=false;
	emit ConnectionLost();
	return false;
}
