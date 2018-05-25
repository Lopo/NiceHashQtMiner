#ifndef INTERFACES_IBENCHMARKFORM_H_
#define INTERFACES_IBENCHMARKFORM_H_

class ComputeDevice;
class Algorithm;

class IBenchmarkForm
{
public:
	virtual bool InBenchmark()=0;

	virtual void SetCurrentStatus(ComputeDevice* device, Algorithm* algorithm, QString status)=0;
	virtual void AddToStatusCheck(ComputeDevice* device, Algorithm* algorithm)=0;
	virtual void RemoveFromStatusCheck(ComputeDevice* device, Algorithm* algorithm)=0;
	virtual void EndBenchmarkForDevice(ComputeDevice* device, bool failedAlgos)=0;
	virtual void StepUpBenchmarkStepProgress()=0;
};

#endif
