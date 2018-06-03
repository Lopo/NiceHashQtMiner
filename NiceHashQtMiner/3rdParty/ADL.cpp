#include "3rdParty/ADL.h"
#include <QLibrary>
#include <cstdlib>
#include <dlfcn.h>


using namespace ATI::ADL;

void* ADL::adlLib=nullptr;

ADL_MAIN_CONTROL_CREATE ADL::_ADL_Main_Control_Create=nullptr;
ADL2_MAIN_CONTROL_CREATE ADL::_ADL2_Main_Control_Create=nullptr;
ADL_MAIN_CONTROL_DESTROY ADL::_ADL_Main_Control_Destroy=nullptr;
ADL2_MAIN_CONTROL_DESTROY ADL::_ADL2_Main_Control_Destroy=nullptr;
ADL_ADAPTER_NUMBEROFADAPTERS_GET ADL::_ADL_Adapter_NumberOfAdapters_Get=nullptr;
ADL_ADAPTER_ADAPTERINFO_GET ADL::_ADL_Adapter_AdapterInfo_Get=nullptr;
ADL2_ADAPTER_ADAPTERINFO_GET ADL::_ADL2_Adapter_AdapterInfo_Get=nullptr;
ADL_ADAPTER_ACTIVE_GET ADL::_ADL_Adapter_Active_Get=nullptr;
ADL_OVERDRIVE5_CURRENTACTIVITY_GET ADL::_ADL_Overdrive5_CurrentActivity_Get=nullptr;
ADL_OVERDRIVE5_TEMPERATURE_GET ADL::_ADL_Overdrive5_Temperature_Get=nullptr;
ADL_OVERDRIVE5_FANSPEED_GET ADL::_ADL_Overdrive5_FanSpeed_Get=nullptr;
ADL2_OVERDRIVE6_CURRENTPOWER_GET ADL::_ADL2_Overdrive6_CurrentPower_Get=nullptr;


void* __stdcall ADL_Main_Memory_Alloc(int iSize)
{
    void* vP=malloc(iSize);
	return vP;
}

bool ADL::Init()
{
//	bool isLib=QLibrary::isLibrary("/tmp/libatiadlxx.so");
//	QLibrary* adLib=new QLibrary("/tmp/libatiadlxx.so");
//	bool l=adLib->load();

	if (adlLib==nullptr) {
		adlLib=dlopen("libatiadlxx.so", RTLD_LAZY|RTLD_GLOBAL);
		}

	if (_ADL_Main_Control_Create==nullptr) {
		_ADL_Main_Control_Create=(ADL_MAIN_CONTROL_CREATE)getProcAddress(adlLib, "ADL_Main_Control_Create");
		}
	if (_ADL_Main_Control_Create==nullptr) {
		return false;
		}
	if (_ADL2_Main_Control_Create==nullptr) {
		_ADL2_Main_Control_Create=(ADL2_MAIN_CONTROL_CREATE)getProcAddress(adlLib, "ADL2_Main_Control_Create");
		}
	if (_ADL2_Main_Control_Create==nullptr) {
		return false;
		}
	if (_ADL_Main_Control_Destroy==nullptr) {
		_ADL_Main_Control_Destroy=(ADL_MAIN_CONTROL_DESTROY)getProcAddress(adlLib, "ADL_Main_Control_Destroy");
		}
	if (_ADL_Main_Control_Destroy==nullptr) {
		return false;
		}
	if (_ADL2_Main_Control_Destroy==nullptr) {
		_ADL2_Main_Control_Destroy=(ADL2_MAIN_CONTROL_DESTROY)getProcAddress(adlLib, "ADL2_Main_Control_Destroy");
		}
	if (_ADL2_Main_Control_Destroy==nullptr) {
		return false;
		}

	if (_ADL_Adapter_NumberOfAdapters_Get==nullptr) {
		_ADL_Adapter_NumberOfAdapters_Get=(ADL_ADAPTER_NUMBEROFADAPTERS_GET)getProcAddress(adlLib, "ADL_Adapter_NumberOfAdapters_Get");
		}
	if (_ADL_Adapter_NumberOfAdapters_Get==nullptr) {
		return false;
		}
	if (_ADL_Adapter_AdapterInfo_Get==nullptr) {
		_ADL_Adapter_AdapterInfo_Get=(ADL_ADAPTER_ADAPTERINFO_GET)getProcAddress(adlLib, "ADL_Adapter_AdapterInfo_Get");
		}
	if (_ADL_Adapter_AdapterInfo_Get==nullptr) {
		return false;
		}
	if (_ADL2_Adapter_AdapterInfo_Get==nullptr) {
		_ADL2_Adapter_AdapterInfo_Get=(ADL2_ADAPTER_ADAPTERINFO_GET)getProcAddress(adlLib, "ADL_Adapter_AdapterInfo_Get");
		}
	if (_ADL2_Adapter_AdapterInfo_Get==nullptr) {
		return false;
		}
	if (_ADL_Adapter_Active_Get==nullptr) {
		_ADL_Adapter_Active_Get=(ADL_ADAPTER_ACTIVE_GET)getProcAddress(adlLib, "ADL_Adapter_Active_Get");
		}
	if (_ADL_Adapter_Active_Get==nullptr) {
		return false;
		}

	if (_ADL_Overdrive5_CurrentActivity_Get==nullptr) {
		_ADL_Overdrive5_CurrentActivity_Get=(ADL_OVERDRIVE5_CURRENTACTIVITY_GET)getProcAddress(adlLib, "ADL_Overdrive5_CurrentActivity_Get");
		}
	if (_ADL_Overdrive5_CurrentActivity_Get==nullptr) {
		return false;
		}
	if (_ADL_Overdrive5_Temperature_Get==nullptr) {
		_ADL_Overdrive5_Temperature_Get=(ADL_OVERDRIVE5_TEMPERATURE_GET)getProcAddress(adlLib, "ADL_Overdrive5_Temperature_Get");
		}
	if (_ADL_Overdrive5_Temperature_Get==nullptr) {
		return false;
		}
	if (_ADL_Overdrive5_FanSpeed_Get==nullptr) {
		_ADL_Overdrive5_FanSpeed_Get=(ADL_OVERDRIVE5_FANSPEED_GET)getProcAddress(adlLib, "ADL_Overdrive5_FanSpeed_Get");
		}
	if (_ADL_Overdrive5_FanSpeed_Get==nullptr) {
		return false;
		}

	if (_ADL2_Overdrive6_CurrentPower_Get==nullptr) {
		_ADL2_Overdrive6_CurrentPower_Get=(ADL2_OVERDRIVE6_CURRENTPOWER_GET)getProcAddress(adlLib, "ADL2_Overdrive6_CurrentPower_Get");
		}
	if (_ADL2_Overdrive6_CurrentPower_Get==nullptr) {
		return false;
		}

	return true;
}

void* ADL::getProcAddress(void* lib, const char* name)
{
	void* adr=dlsym(lib, name);
	return adr;
}

int ADL::ADL_Main_Control_Create(ADL_MAIN_MALLOC_CALLBACK callback, int enumConnectedAdapters)
{
	return Init()
		? _ADL_Main_Control_Create(callback, enumConnectedAdapters)
		: -1;
}

int ADL::ADL2_Main_Control_Create(ADL_MAIN_MALLOC_CALLBACK callback, int enumConnectedAdapters, ADL_CONTEXT_HANDLE context)
{
	return Init()
		? _ADL2_Main_Control_Create(callback, enumConnectedAdapters, context)
		: -1;
}

int ADL::ADL_Main_Control_Destroy()
{
	return Init()
		? _ADL_Main_Control_Destroy()
		: -1;
}

int ADL::ADL2_Main_Control_Destroy(ADL_CONTEXT_HANDLE context)
{
	return Init()
		? _ADL2_Main_Control_Destroy(context)
		: -1;
}

int ADL::ADL_Adapter_Active_Get(int adapterIndex, int* status)
{
	return Init()
		? _ADL_Adapter_Active_Get(adapterIndex, status)
		: -1;
}

int ADL::ADL_Adapter_AdapterInfo_Get(LPAdapterInfo info, int inputSize)
{
	return Init()
		? _ADL_Adapter_AdapterInfo_Get(info, inputSize)
		: -1;
}

int ADL::ADL2_Adapter_AdapterInfo_Get(ADL_CONTEXT_HANDLE context, LPAdapterInfo lpInfo, int iInputSize)
{
	return Init()
		? _ADL2_Adapter_AdapterInfo_Get(context, lpInfo, iInputSize)
		: -1;
}

int ADL::ADL_Adapter_NumberOfAdapters_Get(int* numAdapters)
{
	return Init()
		? _ADL_Adapter_NumberOfAdapters_Get(numAdapters)
		: -1;
}

int ADL::ADL_Overdrive5_CurrentActivity_Get(int iAdapterIndex, ADLPMActivity* activity)
{
	return Init()
		? _ADL_Overdrive5_CurrentActivity_Get(iAdapterIndex, activity)
		: -1;
}

int ADL::ADL_Overdrive5_Temperature_Get(int adapterIndex, int thermalControllerIndex, ADLTemperature* temperature)
{
	return Init()
		? _ADL_Overdrive5_Temperature_Get(adapterIndex, thermalControllerIndex, temperature)
		: -1;
}

int ADL::ADL_Overdrive5_FanSpeed_Get(int adapterIndex, int thermalControllerIndex, ADLFanSpeedValue* temperature)
{
	return Init()
		? _ADL_Overdrive5_FanSpeed_Get(adapterIndex, thermalControllerIndex, temperature)
		: -1;
}


int ADL::ADL2_Overdrive6_CurrentPower_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iPowerType, int* lpCurrentValue)
{
	return Init()
		? _ADL2_Overdrive6_CurrentPower_Get(context, iAdapterIndex, iPowerType, lpCurrentValue)
		: -1;
}
