#ifndef DEVICES_GROUPNAMES_H_
#define DEVICES_GROUPNAMES_H_

//#include <QVector>
//#include <QString>
#include "Enums.h"


class GroupNames
{
private:
	static QStringList Names;

public:
	static QString GetGroupName(Enums::DeviceGroupType type, int id);
	static QString GetNameGeneral(Enums::DeviceType type);

};

#endif
