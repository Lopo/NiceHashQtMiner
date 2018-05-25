#ifndef BENCHMARKING_BENCHHELPERS_CLAYMOREZCASHBENCHHELPER_H_
#define BENCHMARKING_BENCHHELPERS_CLAYMOREZCASHBENCHHELPER_H_

#include <QStringList>
#include <QVector>

class ClaymoreZcashBenchHelper
{
public:
	ClaymoreZcashBenchHelper(QStringList oep);
	int Time=180;
	bool HasTest();
	void SetSpeed(double speed);
	void SetNext();
	QStringList GetTestExtraParams();
	QStringList GetFastestExtraParams();
	double GetFastestTime();

private:
	const int MaxBench=2;
	QVector<QStringList> _asmModes={{"-ams", "1"}, {"-asm", "0"}};
	QStringList _originalExtraParams;

	QVector<double> _speeds={0.0d, 0.0d};
	int _curIndex;

	int FastestIndex();
};

#endif
