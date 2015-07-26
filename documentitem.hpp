#ifndef DOCUMENTITEM_HPP
#define DOCUMENTITEM_HPP

#include <QListWidget>
#include "TextEditor/document.hpp"

class FileListWidget;
class DocumentItem: public QListWidgetItem {
		Document* mDocument;
	public:
		DocumentItem(QListWidget*, Document* doc = 0);
		DocumentItem(FileListWidget*, Document* doc = 0);
		~DocumentItem();
		void setDocument(Document*);
		void update();
		Document* getDocument() const;
};

#endif // DOCUMENTITEM_HPP
