#ifndef INTERFACES_ILISTITEMCHECKCOLORSETTER_H_
#define INTERFACES_ILISTITEMCHECKCOLORSETTER_H_

#include <QAbstractTableModel>


class IListItemCheckColorSetter
{
public:
	virtual void LviSetColor(QAbstractTableModel* model, int idx)=0;
};

#endif
