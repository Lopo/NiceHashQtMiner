#include "Forms/Components/IntensitiesModel.h"


IntensitiesModel::IntensitiesModel(QObject* parent)
	: QAbstractTableModel(parent)
{ }

int IntensitiesModel::rowCount(const QModelIndex& parent) const
{
	return  _data.count();
}

int IntensitiesModel::columnCount(const QModelIndex& parent) const
{
	return 5;
}

bool IntensitiesModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
	if (role!=Qt::EditRole || orientation!=Qt::Horizontal || section<0 || section>=5) {
		return false;
		}
	_header[section]=value.toString();
	return true;
}

QVariant IntensitiesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role!=Qt::DisplayRole || orientation!=Qt::Horizontal) {
		return QVariant();
		}
	return _header.value(section, "");
}

bool IntensitiesModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (!index.isValid()) {
		return false;
		}
	int row=index.row();
	switch (role) {
		case Qt::DisplayRole: // 0
//			int col=index.column();
//			if (col) {
				_data[row].columns[index.column()]=value.toString();
/*				}
			else {
				int intensity=value.value<int>();
				_data[row].Tag=intensity;
				}*/
			emit dataChanged(index, index, {role});
			return true;
		case Qt::EditRole: // 2
//			int col=index.column();
/*			if (col) {
				_data[row].columns[col]=value.toString();
				}
			else {*/
				_data[row].Tag=value.value<int>();
//				}
			emit dataChanged(index, index, {role});
			return true;
		}
	return false;
}

QVariant IntensitiesModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid()) {
		return QVariant();
		}

	int row=index.row();
	if (row>=_data.count() || row<0) {
		return QVariant();
		}

	int col=index.column();
	switch (role) {
		case Qt::DisplayRole: // 0
			return _data.at(row).columns.at(col);
		case Qt::EditRole: // 2
			return QVariant::fromValue<int>(_data.at(row).Tag);
		}
	return QVariant();
}

Qt::ItemFlags IntensitiesModel::flags(const QModelIndex& index) const
{
	if (!index.isValid()) {
		return Qt::ItemIsEnabled;
		}
	return Qt::ItemNeverHasChildren | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool IntensitiesModel::insertRows(int row, int count, const QModelIndex& parent)
{
	beginInsertRows(QModelIndex(), row, row+count-1);

	for (int r=0; r<count; r++) {
		_data.insert(row, iItem());
		}

	endInsertRows();
	return true;
}

bool IntensitiesModel::removeRows(int row, int count, const QModelIndex& parent)
{
	beginRemoveRows(QModelIndex(), row, row+count-1);

	for (int r=0; r<count; r++) {
		_data.removeAt(row);
		}

	endRemoveRows();
	return true;
}
