#ifndef IASJSONOBJECT_H
#define IASJSONOBJECT_H

#include <QJsonObject>


class IasJsonObject
{
public:
	virtual QJsonObject asJsonObject()=0;
};


#endif
