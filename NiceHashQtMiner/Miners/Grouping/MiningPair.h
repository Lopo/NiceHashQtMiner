#ifndef MINERS_GROUPING_MININGPAIR_H_
#define MINERS_GROUPING_MININGPAIR_H_

#include <QObject>
class Algorithm;
class ComputeDevice;


class MiningPair
	: public QObject
{
	Q_OBJECT
public:
	MiningPair(ComputeDevice* d, Algorithm* a);
	MiningPair(const MiningPair& other);

	ComputeDevice* Device=nullptr;
	Algorithm* algorithm=nullptr;
	QStringList CurrentExtraLaunchParameters;
};

#endif
