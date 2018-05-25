#include "3rdParty/ADL.h"
#include <QLibrary>
#include <cstdlib>
#include <dlfcn.h>


using namespace ATI::ADL;

void* ADL::adlLib=nullptr;

ADL_MAIN_CONTROL_CREATE ADL::_ADL_Main_Control_Create;
ADL2_MAIN_CONTROL_CREATE ADL::_ADL2_Main_Control_Create;
ADL_MAIN_CONTROL_DESTROY ADL::_ADL_Main_Control_Destroy;
ADL2_MAIN_CONTROL_DESTROY ADL::_ADL2_Main_Control_Destroy;
ADL_ADAPTER_NUMBEROFADAPTERS_GET ADL::_ADL_Adapter_NumberOfAdapters_Get;
ADL_ADAPTER_ADAPTERINFO_GET ADL::_ADL_Adapter_AdapterInfo_Get;
ADL2_ADAPTER_ADAPTERINFO_GET ADL::_ADL2_Adapter_AdapterInfo_Get;
ADL_ADAPTER_ACTIVE_GET ADL::_ADL_Adapter_Active_Get;
ADL_OVERDRIVE5_CURRENTACTIVITY_GET ADL::_ADL_Overdrive5_CurrentActivity_Get;
ADL_OVERDRIVE5_TEMPERATURE_GET ADL::_ADL_Overdrive5_Temperature_Get;
ADL_OVERDRIVE5_FANSPEED_GET ADL::_ADL_Overdrive5_FanSpeed_Get;
ADL2_OVERDRIVE6_CURRENTPOWER_GET ADL::_ADL2_Overdrive6_CurrentPower_Get;


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

	_ADL_Main_Control_Create=(ADL_MAIN_CONTROL_CREATE)getProcAddress(adlLib, "ADL_Main_Control_Create");
	if (_ADL_Main_Control_Create==0) {
		return false;
		}
	_ADL2_Main_Control_Create=(ADL2_MAIN_CONTROL_CREATE)getProcAddress(adlLib, "ADL2_Main_Control_Create");
	if (_ADL2_Main_Control_Create==0) {
		return false;
		}
	_ADL_Main_Control_Destroy=(ADL_MAIN_CONTROL_DESTROY)getProcAddress(adlLib, "ADL_Main_Control_Destroy");
	if (_ADL_Main_Control_Destroy==0) {
		return false;
		}
	_ADL2_Main_Control_Destroy=(ADL2_MAIN_CONTROL_DESTROY)getProcAddress(adlLib, "ADL2_Main_Control_Destroy");
	if (_ADL2_Main_Control_Destroy==0) {
		return false;
		}

	_ADL_Adapter_NumberOfAdapters_Get=(ADL_ADAPTER_NUMBEROFADAPTERS_GET)getProcAddress(adlLib, "ADL_Adapter_NumberOfAdapters_Get");
	_ADL_Adapter_AdapterInfo_Get=(ADL_ADAPTER_ADAPTERINFO_GET)getProcAddress(adlLib, "ADL_Adapter_AdapterInfo_Get");
	_ADL2_Adapter_AdapterInfo_Get=(ADL2_ADAPTER_ADAPTERINFO_GET)getProcAddress(adlLib, "ADL_Adapter_AdapterInfo_Get");
	_ADL_Adapter_Active_Get=(ADL_ADAPTER_ACTIVE_GET)getProcAddress(adlLib, "ADL_Adapter_Active_Get");

	_ADL_Overdrive5_CurrentActivity_Get=(ADL_OVERDRIVE5_CURRENTACTIVITY_GET)getProcAddress(adlLib, "ADL_Overdrive5_CurrentActivity_Get");
	_ADL_Overdrive5_Temperature_Get=(ADL_OVERDRIVE5_TEMPERATURE_GET)getProcAddress(adlLib, "ADL_Overdrive5_Temperature_Get");
	_ADL_Overdrive5_FanSpeed_Get=(ADL_OVERDRIVE5_FANSPEED_GET)getProcAddress(adlLib, "ADL_Overdrive5_FanSpeed_Get");

	_ADL2_Overdrive6_CurrentPower_Get=(ADL2_OVERDRIVE6_CURRENTPOWER_GET)getProcAddress(adlLib, "ADL2_Overdrive6_CurrentPower_Get");

	return true;
}

void* ADL::getProcAddress(void* lib, const char* name)
{
	void* adr=dlsym(lib, name);
	return adr;
}

bool ADL::GetAdlErr(int error)
{
	return false;
}

bool ADL::ADL_Main_Control_Create(ADL_MAIN_MALLOC_CALLBACK callback, int enumConnectedAdapters)
{
	return GetAdlErr(_ADL_Main_Control_Create(callback, enumConnectedAdapters));
}

bool ADL::ADL2_Main_Control_Create(ADL_MAIN_MALLOC_CALLBACK callback, int enumConnectedAdapters, ADL_CONTEXT_HANDLE context)
{
	return GetAdlErr(_ADL2_Main_Control_Create(callback, enumConnectedAdapters, context));
}

bool ADL::ADL_Main_Control_Destroy()
{
	return GetAdlErr(_ADL_Main_Control_Destroy());
}

bool ADL::ADL2_Main_Control_Destroy(ADL_CONTEXT_HANDLE context)
{
	return GetAdlErr(_ADL2_Main_Control_Destroy(context));
}

bool ADL::ADL_Adapter_Active_Get(int adapterIndex, int* status)
{
	return GetAdlErr(_ADL_Adapter_Active_Get(adapterIndex, status));
}

bool ADL::ADL_Adapter_AdapterInfo_Get(LPAdapterInfo info, int inputSize)
{
	return GetAdlErr(_ADL_Adapter_AdapterInfo_Get(info, inputSize));
}

bool ADL::ADL2_Adapter_AdapterInfo_Get(ADL_CONTEXT_HANDLE context, LPAdapterInfo lpInfo, int iInputSize)
{
	return GetAdlErr(_ADL2_Adapter_AdapterInfo_Get(context, lpInfo, iInputSize));
}

bool ADL::ADL_Adapter_NumberOfAdapters_Get(int* numAdapters)
{
	return GetAdlErr(_ADL_Adapter_NumberOfAdapters_Get(numAdapters));
}

bool ADL::ADL_Overdrive5_CurrentActivity_Get(int iAdapterIndex, ADLPMActivity* activity)
{
	return GetAdlErr(_ADL_Overdrive5_CurrentActivity_Get(iAdapterIndex, activity));
}

bool ADL::ADL_Overdrive5_Temperature_Get(int adapterIndex, int thermalControllerIndex, ADLTemperature* temperature)
{
	return GetAdlErr(_ADL_Overdrive5_Temperature_Get(adapterIndex, thermalControllerIndex, temperature));
}

bool ADL::ADL_Overdrive5_FanSpeed_Get(int adapterIndex, int thermalControllerIndex, ADLFanSpeedValue* temperature)
{
	return GetAdlErr(_ADL_Overdrive5_FanSpeed_Get(adapterIndex, thermalControllerIndex, temperature));
}


bool ADL::ADL2_Overdrive6_CurrentPower_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iPowerType, int* lpCurrentValue)
{
	return GetAdlErr(_ADL2_Overdrive6_CurrentPower_Get(context, iAdapterIndex, iPowerType, lpCurrentValue));
}
