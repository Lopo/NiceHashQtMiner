#ifndef DEVICES_ALGORITHMS_DEFAULTALGORITHMS_H_
#define DEVICES_ALGORITHMS_DEFAULTALGORITHMS_H_

#include "config.h"
#include "Algorithms/DualAlgorithm.h"
#include <QMap>
#include <QList>


class DefaultAlgorithms
{
public:
	static QMap<Enums::MinerBaseType, QList<Algorithm*>*>* Cpu();
#if WITH_AMD
	static QMap<Enums::MinerBaseType, QList<Algorithm*>*>* Amd();
#endif
#if WITH_NVIDIA
	static QMap<Enums::MinerBaseType, QList<Algorithm*>*>* Nvidia();
#endif

private:
	static QMap<Enums::MinerBaseType, QList<Algorithm*>*>* All();
	static QMap<Enums::MinerBaseType, QList<Algorithm*>*>* Gpu();

#if WITH_AMD
	static const QStringList RemDis;
	static const QStringList DefaultParam;
#endif
};

#endif
