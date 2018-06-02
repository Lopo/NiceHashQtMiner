#ifndef DEVICES_ALGORITHMS_DEFAULTALGORITHMS_H_
#define DEVICES_ALGORITHMS_DEFAULTALGORITHMS_H_

#include "Algorithms/DualAlgorithm.h"
#include <QMap>
#include <QList>


class DefaultAlgorithms
{
public:
	static QMap<Enums::MinerBaseType, QList<Algorithm*>*>* Cpu();
	static QMap<Enums::MinerBaseType, QList<Algorithm*>*>* Amd();
	static QMap<Enums::MinerBaseType, QList<Algorithm*>*>* Nvidia();

private:
	static QMap<Enums::MinerBaseType, QList<Algorithm*>*>* All();
	static QMap<Enums::MinerBaseType, QList<Algorithm*>*>* Gpu();

	static const QStringList RemDis;
	static const QStringList DefaultParam;
};

#endif
