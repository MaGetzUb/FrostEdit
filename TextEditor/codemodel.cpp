#include "codemodel.hpp"



CodeModel::CodeModel(QObject *parent) : QObject(parent) {

}

CodeModel::~CodeModel() {

}

void CodeModel::setMainDocument(Document *document) {
	mMainDocument = document;
}

Document *CodeModel::mainDocument() const {
	return mMainDocument;
}

void CodeModel::addDocument(const QString &filePath, Document *document) {
	mDocuments.append(document);
	documentAdded(filePath, document);
}

