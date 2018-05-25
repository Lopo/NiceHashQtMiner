#ifndef NVMLTYPES_H
#define NVMLTYPES_H

namespace ManagedCuda {
	namespace Nvml {


enum class nvmlBridgeChipType {
	
	};


enum class nvmlReturn { // 562
	Success=0, // The operation was successful
	Uninitialized=1, // NVML was not first initialized with nvmlInit()
	InvalidArgument=2, // A supplied argument is invalid
	NotSupported=3, // The requested operation is not available on target device
	NoPermission=4, // The current user does not have permission for operation
	AlreadyInitialized=5, // Deprecated: Multiple initializations are now allowed through ref counting
	NotFound=6, // A query to find an object was unsuccessful
	InsufficientSize=7, // An input argument is not large enough
	InsufficientPower=8, // A device's external power cables are not properly attached
	DriverNotLoaded=9, // NVIDIA driver is not loaded
	TimeOut=10, // User provided timeout passed
	IRQIssue=11, // NVIDIA Kernel detected an interrupt issue with a GPU
	LibraryNotFound=12, // NVML Shared Library couldn't be found or loaded
	FunctionNotFound=13, // Local version of NVML doesn't implement this function
	CorruptedInfoROM=14, // infoROM is corrupted
	GPUIsLost=15, // The GPU has fallen off the bus or has otherwise become inaccessible
	ResetRequired=16, // The GPU requires a reset before it can be used again
	OperatingSystem=17, // The GPU control device has been blocked by the operating system/cgroups
	LibRMVersionMismatch=18, // RM detects a driver/library version mismatch
	InUse=19, // An operation cannot be performed because the GPU is currently in use
	Unknown=999, // An internal driver error occurred
	};


struct nvmlDevice { // 1260
public:
	int* Pointer;
};

		}
	}

#endif
