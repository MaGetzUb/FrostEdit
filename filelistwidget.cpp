#include "filelistwidget.hpp"
#include <QContextMenuEvent>
#include <QMenu>
#include <QProcess>

#include "documentitem.hpp"
#include "frostedit.hpp"

FileListWidget::FileListWidget(QWidget* parent):
	QListWidget(parent)
{
}

void FileListWidget::contextMenuEvent(QContextMenuEvent* e) {
	mSelection = dynamic_cast<DocumentItem*>(itemAt(e->pos()));

	if(mSelection == nullptr)
		return;

	QMenu* menu = new QMenu(this);

	if(mSelection->getDocument()->isActualFile()){
		menu->addAction("Show in explorer", this, SLOT(openFileDialog()));
	}

	menu->addAction("Close", mSelection->getDocument(), SLOT(close()));
	menu->exec(mapToGlobal(e->pos()));
	delete menu;
}

void FileListWidget::openFileDialog() {
	QFileInfo info = mSelection->getDocument()->getFileInfo();
	QString path = info.absoluteFilePath();
	QString call;
	#ifdef _WIN32
	path.replace("/", "\\");
	call = QString("Explorer /select,%1").arg(path);
	#endif
	QProcess::execute(call);

}


