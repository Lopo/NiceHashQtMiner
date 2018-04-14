#ifndef FORMS_COMPONENTS_DEVICESLISTVIEWENABLECONTROLMODEL_H_
#define FORMS_COMPONENTS_DEVICESLISTVIEWENABLECONTROLMODEL_H_

#include <QAbstractTableModel>
#include <QString>
#include <QList>
#include <QColor>

class ComputeDevice;


class dItem
{
protected:
	Qt::CheckState Checked=Qt::Unchecked;
	QString Text;
	ComputeDevice* Tag=nullptr;
	QColor color=Qt::white;

	friend class DevicesListViewEnableControlModel;
};


class DevicesListViewEnableControlModel
	: public QAbstractTableModel
{
	Q_OBJECT
public:
	explicit DevicesListViewEnableControlModel(QObject* parent=Q_NULLPTR);

	int rowCount(const QModelIndex& parent=QModelIndex()) const override;
	int columnCount(const QModelIndex& parent=QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role=Qt::EditRole) override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;

	bool insertRows(int row, int count, const QModelIndex& parent) override;
	bool removeRows(int row, int count, const QModelIndex& parent) override;

	bool CheckBoxes=true;

protected:
	QList<dItem> _data;

Q_SIGNALS:
	void ItemChecked(int row);
};

#endif
