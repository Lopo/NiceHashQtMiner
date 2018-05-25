#ifndef FORMS_COMPONENTS_INTENSITIESMODEL_H_
#define FORMS_COMPONENTS_INTENSITIESMODEL_H_

#include <QAbstractTableModel>


class iItem
{
protected:
	QVector<QString> columns=QVector<QString>(5);
	int Tag;

	friend class IntensitiesModel;
};


class IntensitiesModel
	: public QAbstractTableModel
{
public:
	IntensitiesModel(QObject* parent=Q_NULLPTR);

	int rowCount(const QModelIndex& parent=QModelIndex()) const override;
	int columnCount(const QModelIndex& parent=QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const override;
	bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role=Qt::EditRole) override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role=Qt::EditRole) override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;

	bool insertRows(int row, int count, const QModelIndex& parent) override;
	bool removeRows(int row, int count, const QModelIndex& parent) override;

protected:
	QList<iItem> _data;
	QVector<QString> _header=QVector<QString>(5);
};

#endif
