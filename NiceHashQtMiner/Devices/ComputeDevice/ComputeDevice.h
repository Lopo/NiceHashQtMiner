#ifndef DEVICES_COMPUTEDEVICE_COMPUTEDEVICE_H_
#define DEVICES_COMPUTEDEVICE_COMPUTEDEVICE_H_

#include "config.h"
#include "Enums.h"
class ComputeDeviceConfig;
class DeviceBenchmarkConfig;
#if WITH_NVIDIA
class CudaDevice;
#endif
#if WITH_AMD
class AmdGpuDevice;
#endif
class Algorithm;


class ComputeDevice // done
	: public QObject
{
	Q_OBJECT
	Q_PROPERTY(int Index READ Index) // for socket control, unique
	Q_PROPERTY(QString Uuid READ Uuid) // UUID now used for saving
	Q_PROPERTY(int BusID READ BusID)
	Q_PROPERTY(int Threads READ Threads)
	Q_PROPERTY(ulong AffinityMask READ AffinityMask)
	Q_PROPERTY(QString Codename READ Codename)
	Q_PROPERTY(QString InfSection READ InfSection)
	Q_PROPERTY(bool DriverDisableAlgos READ DriverDisableAlgos) // amd has some algos not working with new drivers
public:
	virtual ~ComputeDevice() {};

	int ID=0;
	inline int Index() const { return _Index; };
	QString Name; // to identify equality
	QString NameCount; // name count si the short name for displaying in mining groups
	bool Enabled=false;
	Enums::DeviceGroupType DeviceGroupType;
	Enums::DeviceType DeviceType; // CPU, NVIDIA, AMD
	inline QString Uuid() const { return _Uuid; };
	inline int BusID() const { return _BusID; }; // used for Claymore indexing
	int IDByBus=-1;

	// CPU extras
	inline int Threads() const { return _Threads; };
	inline ulong AffinityMask() const { return _AffinityMask; };

	// GPU extras
	ulong GpuRam=0;
	bool IsEtherumCapale=false;
	static ulong Memory3Gb;

	inline QString Codename() const { return _Codename; };
	inline QString InfSection() const { return _InfSection; };
	inline bool DriverDisableAlgos() const { return _DriverDisableAlgos; };

	QString BenchmarkCopyUuid;

	virtual float Load() { return 0;};
	virtual float Temp() { return 0;};
	virtual uint FanSpeed() { return 0;};

	ComputeDevice(int id); // Fake dev
	ComputeDevice(int id, QString group, QString name, int threads, ulong affinityMask, int cpuCount); // CPU
#if WITH_NVIDIA
	ComputeDevice(CudaDevice* cudaDevice, Enums::DeviceGroupType group, int gpuCount); // GPU NVIDIA
	bool IsSM50();
#endif
#if WITH_AMD
	ComputeDevice(AmdGpuDevice* amdDevice, int gpuCount, bool isDetectionFallback);// GPU AMD
#endif

	QString GetFullName(); // combines long and short name
	Algorithm* GetAlgorithm(Enums::MinerBaseType minerBaseType, Enums::AlgorithmType algorithmType, Enums::AlgorithmType secondaryAlgorithmType);
	void CopyBenchmarkSettingsFrom(ComputeDevice* copyBenchCDev);

	void SetFromComputeDeviceConfig(ComputeDeviceConfig* config);
	void SetAlgorithmDeviceConfig(DeviceBenchmarkConfig* config);
	ComputeDeviceConfig* GetComputeDeviceConfig();
	DeviceBenchmarkConfig* GetAlgorithmDeviceConfig();

	QList<Algorithm*>* GetAlgorithmSettings();
	QList<Algorithm*>* GetAlgorithmSettingsFastest();

	bool IsAlgorithmSettingsInitialized(); // internal

protected:
	int _Index=-1;
	QString _Uuid;
	int _BusID=-1;
	int _Threads=0;
	ulong _AffinityMask=0;
	QString _Codename;
	QString _InfSection;
	bool _DriverDisableAlgos=false;

	QList<Algorithm*>* AlgorithmSettings=new QList<Algorithm*>;

	ComputeDevice(int id, QString name, bool enabled, Enums::DeviceGroupType group, bool ethereumCapable, Enums::DeviceType type, QString nameCount, ulong gpuRam); // ambiguous constructor
#if WITH_NVIDIA
	int _SM_major=-1;
	int _SM_minor=-1;
#endif
	static QString GetUuid(int id, QString group, QString name, Enums::DeviceGroupType deviceGroupType);

private:

	QList<Algorithm*>* GetAlgorithmSettingsThirdParty(Enums::Use3rdPartyMiners use3rdParty);
};

#endif
