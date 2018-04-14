#ifndef DEVICES_CUDA_UNSUPPORTED_H_
#define DEVICES_CUDA_UNSUPPORTED_H_

#include <QStringList>


class CudaUnsupported
{
private:
	static QStringList SM10;
	static QStringList SM11;
	static QStringList SM12;
	static QStringList SM13;
	static QStringList SM20;

	static bool ContainsSM(QStringList list, QString text);

public:
	static bool IsSupported(QString text);
};

#endif
