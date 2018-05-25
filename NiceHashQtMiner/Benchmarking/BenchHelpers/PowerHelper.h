#ifndef BENCHMARKING_BENCHHELPERS_POWERHELPER_H_
#define BENCHMARKING_BENCHHELPERS_POWERHELPER_H_


#include <QObject>
#include <QTimer>
class ComputeDevice;


class PowerHelper
	: public QObject
{
public:
	PowerHelper(ComputeDevice* dev);
	void Start();
	double EndAndReset();
	double Stop();
	double End();

	inline double Interval() const {return _interval;};
	void Interval(double value) { if (value<=0)return; _interval=value; if (_timer!=nullptr) _timer->setInterval(value);};

	inline bool IsRunning() const { return _timer!=nullptr && _timer->isActive();};
	double LoadThreshold=30;

private:
	double CalculatePower();

	QTimer* _timer=nullptr;

	double _powerSum;
	int _powerCount;
	bool _started;

	ComputeDevice* _device;

	double _interval=1000;

private Q_SLOTS:
	void TimerOnElapsed();
};

#endif
