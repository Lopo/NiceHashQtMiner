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
	Q_PROPERTY(int Index READ Index) // For socket control, unique
	Q_PROPERTY(QString Uuid READ Uuid) // UUID now used for saving
	Q_PROPERTY(int BusID READ BusID)
	Q_PROPERTY(int Threads READ Threads)
	Q_PROPERTY(ulong AffinityMask READ AffinityMask)
	Q_PROPERTY(QString Codename READ Codename)
	Q_PROPERTY(QString InfSection READ InfSection)
	Q_PROPERTY(bool DriverDisableAlgos READ DriverDisableAlgos) // amd has some algos not working with new drivers
public:

	int ID=0;
	inline int Index() const { return Index_; };
	QString Name; // to identify equality
	QString NameCount; // name count si the short name for displaying in mining groups
	bool Enabled=false;
	Enums::DeviceGroupType DeviceGroupType;
	Enums::DeviceType DeviceType; // CPU, NVIDIA, AMD
	inline QString Uuid() const { return Uuid_; };
	inline int BusID() const { return BusID_; }; // used for Claymore indexing
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
	QString TuningCopyUuid;

	virtual float Load() { return -1;};
	virtual float Temp() { return -1;};
	virtual int FanSpeed() { return -1;};
	virtual double PowerUsage() { return -1;};

	ComputeDevice(int id); // Fake dev

	QString GetFullName(); // combines long and short name
	Algorithm* GetAlgorithm(Algorithm* modelAlgo);
	Algorithm* GetAlgorithm(Enums::MinerBaseType minerBaseType, Enums::AlgorithmType algorithmType, Enums::AlgorithmType secondaryAlgorithmType);
	void CopyBenchmarkSettingsFrom(ComputeDevice* copyBenchCDev);
	void CopyTuningSettingsFrom(ComputeDevice* copyTuningCDev);

	void SetFromComputeDeviceConfig(ComputeDeviceConfig* config);
	void SetAlgorithmDeviceConfig(DeviceBenchmarkConfig* config);
	ComputeDeviceConfig* GetComputeDeviceConfig();
	DeviceBenchmarkConfig* GetAlgorithmDeviceConfig();

	QList<Algorithm*>* GetAlgorithmSettings();
	QList<Algorithm*>* GetAlgorithmSettingsFastest();

	bool IsAlgorithmSettingsInitialized(); // internal

//	int GetHashCode();
	bool operator==(const ComputeDevice& right);
	bool operator!=(const ComputeDevice& right);

protected:
	int Index_=-1;
	QString Uuid_;
	int BusID_=-1;
	int _Threads=0;
	ulong _AffinityMask=0;
	QString _Codename;
	QString _InfSection;
	bool _DriverDisableAlgos=false;

	QList<Algorithm*>* AlgorithmSettings=new QList<Algorithm*>;

	ComputeDevice(int id, QString name, bool enabled, Enums::DeviceGroupType group, bool ethereumCapable, Enums::DeviceType type, QString nameCount, ulong gpuRam); // ambiguous constructor
	static QString GetUuid(int id, QString group, QString name, Enums::DeviceGroupType deviceGroupType);

	bool Equals(const ComputeDevice* other);

private:

	QList<Algorithm*>* GetAlgorithmSettingsThirdParty(Enums::Use3rdPartyMiners use3rdParty);
};

#endif
