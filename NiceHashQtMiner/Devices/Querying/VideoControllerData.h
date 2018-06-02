#ifndef DEVICES_QUERYING_VIDEOCONTROLLERDATA_H_
#define DEVICES_QUERYING_VIDEOCONTROLLERDATA_H_

#include <QString>


class VideoControllerData
{
public:
	QString Name;
	QString Description;
	QString PnpDeviceID;
	QString DriverVersion;
	QString Status;
	QString InfSection; // get arhitecture
	ulong AdapterRAM=0;
	QString SysFS;
	QString UDev;
	QString SysFS_BusID;
	QString modalias;
};


#endif
