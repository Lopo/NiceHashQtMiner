#ifndef FORMS_COMPONENTS_ALGORITHMSLISTVIEWMODEL_H_
#define FORMS_COMPONENTS_ALGORITHMSLISTVIEWMODEL_H_

#include "Algorithms/Algorithm.h"
#include <QAbstractTableModel>
#include <QList>
#include <QColor>
#include <QVector>


class aItem
{
protected:
	Qt::CheckState Checked=Qt::Unchecked;
	QVector<QString> SubItems=QVector<QString>(6);
	Algorithm* Tag=nullptr;
	QColor color=Qt::white;

	friend class AlgorithmsListViewModel;
};


class AlgorithmsListViewModel
	: public QAbstractTableModel
{
	Q_OBJECT
public:
	explicit AlgorithmsListViewModel(QObject* parent=Q_NULLPTR);

	int rowCount(const QModelIndex& parent=QModelIndex()) const override;
	int columnCount(const QModelIndex& parent=QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const override;
	bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role=Qt::EditRole) override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role=Qt::EditRole) override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;

	bool insertRows(int row, int count, const QModelIndex& parent) override;
	bool removeRows(int row, int count, const QModelIndex& parent) override;

	bool CheckBoxes=true;

private:
	QList<aItem> _data;
	QVector<QString> _header=QVector<QString>(6);

Q_SIGNALS:
	void ItemChecked(int);
};

#endif
