#include "Benchmarking/BenchHelpers/ClaymoreZcashBenchHelper.h"


ClaymoreZcashBenchHelper::ClaymoreZcashBenchHelper(QStringList oep) 
{
	_originalExtraParams=oep;
}

bool ClaymoreZcashBenchHelper::HasTest()
{
	return _curIndex<MaxBench;
}

void ClaymoreZcashBenchHelper::SetSpeed(double speed)
{
	if (HasTest()) {
		_speeds[_curIndex]=speed;
		}
}

void ClaymoreZcashBenchHelper::SetNext()
{
	_curIndex++;
}

QStringList ClaymoreZcashBenchHelper::GetTestExtraParams()
{
	if (HasTest()) {
		return _originalExtraParams+_asmModes[_curIndex];
		}
	return _originalExtraParams;
}

int ClaymoreZcashBenchHelper::FastestIndex()
{
	int maxIndex=0;
	double maxValue=_speeds[maxIndex];
	for (int i=1; i<_speeds.length(); ++i) {
		if (_speeds[i]>maxValue) {
			maxIndex=i;
			maxValue=_speeds[i];
			}
		}

	return 0;
}

QStringList ClaymoreZcashBenchHelper::GetFastestExtraParams()
{
	return _originalExtraParams+_asmModes[FastestIndex()];
}

double ClaymoreZcashBenchHelper::GetFastestTime()
{
	return _speeds[FastestIndex()];
}
