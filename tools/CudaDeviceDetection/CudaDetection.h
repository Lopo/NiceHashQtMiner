#ifndef NHQTM_TOOLS_CUDADEVICEDETECTION_CUDADETECTION_H
#define NHQTM_TOOLS_CUDADEVICEDETECTION_CUDADETECTION_H

#include <map>
#include <vector>
#include <string>

#include "cuda_nvml_helper.h"
#include "CudaDevice.h"

#include <cstdint>

class CudaDetection
{
public:
	CudaDetection();
	~CudaDetection();

	bool QueryDevices();
	void PrintDevicesJson();
	void PrintDevicesJson_d();

private:
	void json_print(CudaDevice &dev);

	void json_print_d(CudaDevice &dev);

	std::vector<std::string> _errorMsgs;
	std::vector<CudaDevice> _cudaDevices;

	static std::map<uint16_t, std::string> _VENDOR_NAMES;
	static uint16_t getVendorId(nvmlPciInfo_t &nvmlPciInfo);
	static std::string getVendorString(nvmlPciInfo_t &nvmlPciInfo);
};

#endif // NHQTM_TOOLS_CUDADEVICEDETECTION_CUDADETECTION_H
