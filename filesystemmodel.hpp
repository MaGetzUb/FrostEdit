#ifndef FILESYSTEMMODEL_HPP
#define FILESYSTEMMODEL_HPP

#include <QFileSystemModel>


class FileSystemModel : public QFileSystemModel {
	Q_OBJECT

	public:
		FileSystemModel(QObject* parent = 0);

		QVariant headerData(int section, Qt::Orientation orientation, int role);

};

#endif // FILESYSTEMMODEL_HPP
