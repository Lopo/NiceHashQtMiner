#include "Benchmarking/BenchHelpers/PowerHelper.h"
#include "Devices/ComputeDevice/ComputeDevice.h"


PowerHelper::PowerHelper(ComputeDevice* dev)
{
	_device=dev;
}

void PowerHelper::Start()
{
	if (_timer==nullptr) {
		_timer=new QTimer;
		_timer->setInterval(Interval());
		}
	connect(_timer, SIGNAL(timeout()), this, SLOT(TimerOnElapsed()));
	_timer->start();
}

void PowerHelper::TimerOnElapsed()
{
	double power=_device->PowerUsage();
	if ((_device->Load()>LoadThreshold && power>0) || _started) {
		_powerSum+=power;
		_powerCount++;
		_started=true;
		}
}

double PowerHelper::EndAndReset()
{
	_timer->stop();
	double power=CalculatePower();
	_timer->start();

	return power;
}

double PowerHelper::Stop()
{
	_timer->stop();

	return CalculatePower();
}

double PowerHelper::End()
{
	double power=Stop();
	_timer->deleteLater();

	return power;
}

double PowerHelper::CalculatePower()
{
	double power=_powerSum/std::max(1, _powerCount);

	_powerSum=0;
	_powerCount=0;
	_started=false;

	return power;
}
