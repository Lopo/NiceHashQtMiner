#ifndef DEVICES_QUERYING_OPENCLJSONDATA_H_
#define DEVICES_QUERYING_OPENCLJSONDATA_H_

#include <Devices/Querying/OpenCLJsonData.h>
#include <QString>
#include <QList>


class OpenCLJsonData
{
public:
	QString PlatformName="NONE";
	int PlatformNum=0;
	QList<OpenCLDevice*>* Devices=new QList<OpenCLDevice*>;
};

#endif
