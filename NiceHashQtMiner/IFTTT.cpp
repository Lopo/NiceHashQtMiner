#include "IFTTT.h"
#include "Configs/ConfigManager.h"
#include "Configs/Data/GeneralConfig.h"
#include "Utils/Helpers.h"
#include <QException>
#include <qt/QtNetwork/qnetworkaccessmanager.h>
#include <qt/QtCore/qurlquery.h>
#include <qt/QtNetwork/qhttpmultipart.h>
#include <qt/QtNetwork/qnetworkreply.h>


extern QNetworkAccessManager* qnam;


const QString Ifttt::ApiUrl="https://maker.ifttt.com/trigger/";


void Ifttt::PostToIfttt(QString action, QString msg)
{
	try {
		QString key=ConfigManager.generalConfig->IFTTTKey;
		QString worker=ConfigManager.generalConfig->WorkerName;
		QString minProfit=QString::number(ConfigManager.generalConfig->MinimumProfit, 'f', 2).replace(',', '.');

		QNetworkRequest request(QUrl(ApiUrl+action+"/with/key/"+key));
//		request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
		QUrlQuery params;
		params.addQueryItem("value1", worker);
		params.addQueryItem("value2", msg);
		params.addQueryItem("value3", minProfit);
		QNetworkReply* response= qnam->post(request, params.toString().toLatin1());
		QString responseString(response->readAll());
		}
	catch (QException& ex) {
		Helpers::ConsolePrint("NICEHASH", ex.what());
		}
}
