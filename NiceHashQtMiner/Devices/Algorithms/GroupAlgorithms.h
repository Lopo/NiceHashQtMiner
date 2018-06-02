#ifndef DEVICES_ALGORITHMS_GROUPALGORITHMS_H_
#define DEVICES_ALGORITHMS_GROUPALGORITHMS_H_


#include "Enums.h"
#include <QList>
class Algorithm;
class ComputeDevice;


class GroupAlgorithms
{
public:
	static QList<Algorithm*>* CreateForDeviceList(ComputeDevice* device);
	static QMap<Enums::MinerBaseType, QList<Algorithm*>*>* CreateDefaultsForGroup(Enums::DeviceGroupType deviceGroupType);

private:
	static QMap<Enums::MinerBaseType, QList<Algorithm*>*>* CreateForDevice(ComputeDevice* device);

	static QMap<Enums::MinerBaseType, QList<Algorithm*>*>* FilterMinerBaseTypes(QMap<Enums::MinerBaseType, QList<Algorithm*>*>* minerAlgos, QList<Enums::MinerBaseType> toRemove);
	static QMap<Enums::MinerBaseType, QList<Algorithm*>*>* FilterMinerAlgos(QMap<Enums::MinerBaseType, QList<Algorithm*>*>* minerAlgos, QList<Enums::AlgorithmType> toRemove, QList<Enums::MinerBaseType>* toRemoveBase=nullptr);
};

#endif
