#include "documentitem.hpp"
#include <QFileIconProvider>
#include "TextEditor/document.hpp"

DocumentItem::DocumentItem(QListWidget* widget, Document* doc):
	QListWidgetItem(widget),
	mDocument(doc)
{
	if(mDocument != nullptr) {
		setText(mDocument->getFileName());
		setToolTip(mDocument->getFullPath());
	}
}

DocumentItem::~DocumentItem() {

}

void DocumentItem::setDocument(Document* doc) {
	mDocument = doc;
}

void DocumentItem::update() {

	setText(mDocument->getFileName());
	setToolTip(mDocument->getFullPath());
}

Document* DocumentItem::getDocument() const {
	return mDocument;
}

