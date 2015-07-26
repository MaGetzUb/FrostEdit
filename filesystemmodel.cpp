#include "filesystemmodel.hpp"

FileSystemModel::FileSystemModel(QObject* parent):
	QFileSystemModel(parent)
{
}

QVariant FileSystemModel::headerData(int section, Qt::Orientation orientation, int role) {
	if ((section == 0) && (role == Qt::DisplayRole)) {
		return "Folder";
	} else {
		return QFileSystemModel::headerData(section, orientation, role);
	}
}

