#ifndef DEVICES_QUERYING_AMDQUERY_H_
#define DEVICES_QUERYING_AMDQUERY_H_

#include "config.h"
#include "Devices/Querying/VideoControllerData.h"
#include "Devices/OpenCLDevice.h"
#include "Devices/Querying/OpenCLJsonData.h"
#include <QList>
#include <QMap>
#include <QString>


class AmdQuery
{
public:
	AmdQuery(QList<VideoControllerData*>* availControllers);
	QList<OpenCLDevice*>* QueryAmd(bool openCLSuccess, QList<OpenCLJsonData>* openCLData);

private:
	void DriverCheck();
	QList<OpenCLDevice*>* ProcessDevices(QList<OpenCLJsonData>* openCLData);
	QList<OpenCLDevice*>* AmdDeviceCreationPrimary(QList<OpenCLDevice*>* amdDevices);
	QList<OpenCLDevice*>* AmdDeviceCreationFallback(QList<OpenCLDevice*>* amdDevices);
#if ADL_LIBWORKS
	bool QueryAdl();
#endif

	const QString Tag="AmdQuery";
	const int AmdVendorID=1002;

	struct BusIdInfo {
		public:
			QString Name;
			QString Uuid;
			QString InfSection;
			int Adl1Index;
			int Adl2Index;
		};

	QList<VideoControllerData*>* _availableControllers=nullptr;
	QMap<QString, bool> _driverOld;
	QMap<QString, bool> _noNeoscryptLyra2;
	QMap<int, BusIdInfo> _busIdInfos;
	QList<QString> _amdDeviceUuid;
};

#endif
