#ifndef LISTWIDGET_HPP
#define LISTWIDGET_HPP

#include <QListWidget>

class DocumentItem;
class FileListWidget : public QListWidget {
	Q_OBJECT

		DocumentItem* mSelection;

	public:
		FileListWidget(QWidget* parent = 0);


	protected:

		void contextMenuEvent(QContextMenuEvent *) override;
	private slots:
		void openFileDialog();


};

#endif // LISTWIDGET_HPP
