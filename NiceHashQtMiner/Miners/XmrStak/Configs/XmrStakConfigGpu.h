#ifndef MINERS_XMRSTAK_CONFIGS_XMRSTAKCONFIGGPU_H_
#define MINERS_XMRSTAK_CONFIGS_XMRSTAKCONFIGGPU_H_

#include "Miners/XmrStak/Configs/IasJsonObject.h"



class XmrStakGpuItem
	: public IasJsonObject
{
public:
	int index;
	bool affine_to_cpu;

	virtual QJsonObject asJsonObject();
	virtual void loadJsonObject(QJsonObject obj);

protected:
	XmrStakGpuItem(int index, bool affineToCpu);
};

template<class I>
class XmrStakConfigGpu
	: public IasJsonObject
{
public:
	void SetupThreads(QList<int> indices);

	QList<I*> gpu_threads_conf;

	virtual QJsonObject asJsonObject();
	virtual void loadJsonObject(QJsonObject obj);

protected:
	virtual I* GetGpuItem(int index, bool affine)=0;
};

class XmrStakAmdItem
	: public XmrStakGpuItem
{
public:
	XmrStakAmdItem(int index, bool affine);

	int intensity=512;
	int worksize=8;
	bool strided_index=true;
	int mem_chunk=2;
	int unroll=8;
	bool comp_mode=true;
	virtual QJsonObject asJsonObject();
	static XmrStakAmdItem* fromJsonObject(QJsonObject obj);
};

class XmrStakConfigAmd
	: public XmrStakConfigGpu<XmrStakAmdItem>
{
public:
	int platform_index=0;

	virtual QJsonObject asJsonObject();
	static XmrStakConfigAmd* fromJson(QString json);

protected:
	XmrStakAmdItem* GetGpuItem(int index, bool affine) override;
};

class XmrStakNvidiaItem
	: public XmrStakGpuItem
{
public:
	XmrStakNvidiaItem(int index, bool affine);

	int threads=24;
	int block=60;
	int bfactor=8;
	int bsleep=100;
	/** method used to synchronize the device
	 * documentation: http://docs.nvidia.com/cuda/cuda-runtime-api/group__CUDART__DEVICE.html#group__CUDART__DEVICE_1g69e73c7dda3fc05306ae7c811a690fac
	 * 0 = cudaDeviceScheduleAuto
	 * 1 = cudaDeviceScheduleSpin - create a high load on one cpu thread per gpu
	 * 2 = cudaDeviceScheduleYield
	 * 3 = cudaDeviceScheduleBlockingSync(default)
	 */
	int sync_mode=3;
	/** select the memory access pattern (this option has only a meaning for cryptonight_v8 and monero)
	 * 0 = 64bit memory loads
	 * 1 = 256bit memory loads 
	 */
	int mem_mode=1;

	virtual QJsonObject asJsonObject();
	static XmrStakNvidiaItem* fromJsonObject(QJsonObject obj);
};

class XmrStakConfigNvidia
	: public XmrStakConfigGpu<XmrStakNvidiaItem>
{
public:
	void OverrideBVals();
	static XmrStakConfigNvidia* fromJson(QString json);

protected:
	XmrStakNvidiaItem* GetGpuItem(int index, bool affine) override;
};

#endif
