#ifndef DEVICES_COMPUTEDEVICEMANAGER_H_
#define DEVICES_COMPUTEDEVICEMANAGER_H_

#include "config.h"
#include "Enums.h"

class IMessageNotifier;
class OpenCLDevice;
class ComputeDevice;
class CudaDevice;


class ComputeDeviceManager
{
public:
	static class Query {
	private:
		const static QString Tag;
		class NvidiaSmiDriver { //done
		public:
			NvidiaSmiDriver(int left, int right);
			bool IsLesserVersionThan(NvidiaSmiDriver* b);
			QString ToString();

			int LeftPart=0;
			int _rightPart=0;

		private:
			int GetRightVal(int val);
			}; //done

		static NvidiaSmiDriver* NvidiaRecomendedDriver;
		static NvidiaSmiDriver* NvidiaMinDetectionDriver;
		static NvidiaSmiDriver* _currentNvidiaSmiDriver;
		static NvidiaSmiDriver* InvalidSmiDriver;

		static int _cpuCount;
		static int _gpuCount;

		static NvidiaSmiDriver* GetNvidiaSmiDriver();

		static void ShowMessageAndStep(QString infoMsg); //done
		static IMessageNotifier* MessageNotifier_;

	public:
		static IMessageNotifier* MessageNotifier() { return MessageNotifier_;};
		static bool CheckVideoControllersCountMismath();
		static void QueryDevices(IMessageNotifier* messageNotifier);

	private:
		class VideoControllerData {
		public:
			QString Name;
			QString Description;
			QString PNPDeviceID;
			QString DriverVersion;
			QString Status;
			QString InfSection; // get arhitecture
			ulong AdapterRAM=0;
			QString SysFS;
			QString UDev;
			QString SysFS_BusID;
			QString modalias;
			}; //done
		static QList<VideoControllerData*>* AvailableVideoControllers;

		static class WindowsDisplayAdapters {
//		private:
//			static QString SafeGetProperty(/*ManagementBaseObject mbo, */QString key);

		public:
			static void QueryVideoControllers();
		private:
			static void QueryVideoControllers(QList<ComputeDeviceManager::Query::VideoControllerData*>* avaliableVideoControllers, bool warningsEnabled);
		public:
			static bool HasNvidiaVideoController();
			} WindowsDisplayAdapters;

		static class Cpu {
		public:
			static void QueryCpus();
			} Cpu;
#if WITH_NVIDIA
		static QList<CudaDevice*>* _cudaDevices;
		static class Nvidia {
		private:
			static QString _queryCudaDevicesString;
//		private Q_SLOTS:
//			static void QueryCudaDevicesOutputErrorDataReceived(QString str);

		public:
			static bool IsSkipNvidia();
			static void QueryCudaDevices();
			static void QueryCudaDevices(QList<CudaDevice*>* cudaDevices);
			} Nvidia; //done
#endif
		class OpenCLJsonData_t {
		public:
			QString PlatformName="NONE";
			int PlatformNum=0;
			QList<OpenCLDevice*>* Devices=new QList<OpenCLDevice*>;
			}; //done
		static QList<OpenCLJsonData_t>* _openCLJsonData;
		static bool _isOpenCLQuerySuccess;

		static class OpenCL {
		private:
			static QString _queryOpenCLDevicesString;
		private Q_SLOTS:
			static void QueryOpenCLDevicesOutputErrorDataReceived();
		public:
			static void QueryOpenCLDevices(); //done
			} OpenCL;

	public:
		static QList<OpenCLDevice*>* AmdDevices;

	private:
#if WITH_AMD
		static class Amd {
		public:
			static void QueryAmd();
			} Amd;
#endif
		} Query;

	static class SystemSpecs {
	public:
		static uint64_t FreePhysicalMemory;
		static uint64_t FreeSpaceInPagingFiles;
		static uint64_t FreeVirtualMemory;
		static uint32_t LargeSystemCache;
		static uint32_t MaxNumberOfProcesses;
		static uint64_t MaxProcessMemorySize;

		static uint32_t NumberOfLicensedUsers;
		static uint32_t NumberOfProcesses;
		static uint32_t NumberOfUsers;
		static uint32_t OperatingSystemSKU;

		static uint64_t SizeStoredInPagingFiles;

		static uint32_t SuiteMask;

		static uint64_t TotalSwapSpaceSize;
		static uint64_t TotalVirtualMemorySize;
		static uint64_t TotalVisibleMemorySize;

		static void QueryAndLog();
		} SystemSpecs;

	static class Available { //done
	public:
		static bool HasNvidia;
		static bool HasAmd;
		static bool HasCpu;
		static int CpusCount;

		static int AvailCpus();
		static int AvailNVGpus();
		static int AvailAmdGpus();

//		static int GPUsCount;
		inline static int AvailGpUs() { return AvailAmdGpus()+AvailNVGpus();};
		static int AmdOpenCLPlatformNum;
		static bool IsHyperThreadingEnabled;

		static ulong NvidiaRamSum;
		static ulong AmdRamSum;
		
		static QList<ComputeDevice*>* AllAvailableDevices;

		static ComputeDevice* GetDeviceWithUuid(QString uuid);
		static QList<ComputeDevice*>* GetSameDevicesTypeAsDeviceWithUuid(QString uuid);
		static ComputeDevice* GetCurrentlySelectedComputeDevice(int index, bool unique);
		static int GetCountForType(Enums::DeviceType type);
		} Avaliable; //done

	static class Group { //done
	public:
		static void DisableCpuGroup();
		static bool ContainsAmdGpus();
		static bool ContainsGpus();
		static void UncheckedCpu();
	} Group; //done
};

#ifdef __ComputeDeviceManager_cpp__
	ComputeDeviceManager ComputeDeviceManager;
#else
	extern ComputeDeviceManager ComputeDeviceManager;
#endif

#endif
