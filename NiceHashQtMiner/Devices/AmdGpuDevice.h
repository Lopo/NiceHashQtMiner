#ifndef DEVICES_AMDGPUDEVICE_H_
#define DEVICES_AMDGPUDEVICE_H_

#include "Devices/OpenCLDevice.h"
#include <QObject>


class AmdGpuDevice
	: public QObject
{
	Q_OBJECT
	Q_PROPERTY(int DeviceID READ DeviceID)
	Q_PROPERTY(int BusID READ BusID)
	Q_PROPERTY(ulong DeviceGlobalMemory READ DeviceGlobalMemory)
	Q_PROPERTY(QString Codename READ Codename)
public:
	AmdGpuDevice(OpenCLDevice* openClSubset, bool isOldDriver, QString infSection, bool driverDisableAlgo);

	static const QStringList DefaultParam;
	static const QStringList TemperatureParam;

	inline int DeviceID() const { return _openClSubset->DeviceID;};
	inline int BusID() const { return _openClSubset->AMD_BUS_ID;};
	QString DeviceName; // init this with the ADL
	QString UUID; // init this with the ADL, use PCI_VEN & DEV IDs
	inline ulong DeviceGlobalMemory() const { return _openClSubset->_CL_DEVICE_GLOBAL_MEM_SIZE;};

	QString InfSection; // has arhitecture string

	bool DriverDisableAlgos=false; // new drivers make some algorithms unusable 21.19.164.1 => driver not working with NeoScrypt and

	inline QString Codename() const { return _openClSubset->_CL_DEVICE_NAME;};

	int AdapterIndex=-1; // init this with the ADL

	bool IsEtherumCapable();

private:
	OpenCLDevice* _openClSubset=new OpenCLDevice;
	int m_deviceID;
};

#endif
