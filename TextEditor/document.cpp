#include "document.hpp"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QPlainTextDocumentLayout>
#include <QTime>
#include <QDate>
#include <QDir>

#include "frostedit.hpp"
#include "../documentitem.hpp"

Document::Document(QObject* parent, const QString file, DocumentItem* item):
	QTextDocument(parent),
	mFile(file),
	mHighlighter(nullptr),
	mItem(item),
	mCodeModel(nullptr)
{
	setDocumentLayout(new QPlainTextDocumentLayout(this));

	if(!file.isEmpty()) {
		QFile in(file);
		if(in.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QTextStream stream(&in);
			setPlainText(stream.readAll());
			mFileInfo.setFile(file);
			setModified(false);
			in.close();
		}
	}

	connect(this, SIGNAL(modificationChanged(bool)), this, SLOT(emitTextChanged(bool)));
}

Document::~Document() {
	if(isModified() && !mFileInfo.isFile()) {
		//saveTemporary();
	}
	if(mHighlighter != nullptr)
		delete mHighlighter;
	if(mItem != nullptr)
		delete mItem;
	if(mCodeModel != nullptr)
		delete mCodeModel;
}

void Document::reload() {
	if(mFileInfo.isFile()) {
		QString file = mFileInfo.filePath();
		QFile in(file);
		if(in.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QTextStream stream(&in);
			setPlainText(stream.readAll());
			mFileInfo.setFile(file);
			setModified(false);
			in.close();
		}
	}
}

void Document::save() {
	if(isModified()) {
		if(mFileInfo.isFile() && mFileInfo.exists()) {
			QFile out(mFileInfo.absoluteFilePath());
			if(out.open(QIODevice::WriteOnly | QIODevice::Text)) {
				QTextStream stream(&out);
				stream << toPlainText();
				out.close();
				setModified(false);
			}
		} else {
			saveTemporary();
		}
	}
}

void Document::saveAs(const QString& str) {
	QFile out(str);
	if(out.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream stream(&out);
		stream << toPlainText();
		out.close();
		mFileInfo.setFile(str);
		setModified(false);
		emit pathChanged(str);
	}
}

QFileInfo Document::saveTemporary() {
	QTime time = QTime::currentTime();
	QDate date = QDate::currentDate();
	QString cpath = QDir::currentPath();
	QString fileName = date.toString("dd MM yy")+" "+time.toString("hh mm ss zz")+".tmp";

	if(mFileInfo.isFile())
		fileName = mFileInfo.absolutePath() + "-" + fileName;
	else {
		QString tmpFile = mFile;
		tmpFile.remove("<").remove(">");
		fileName = cpath+"/"+tmpFile + "-" + fileName;
	}
	fileName.replace(" ", "_");

	//QDir dir(cpath+"/unsaved/");
	//if(!dir.exists())
	//	dir.mkdir(dir.path());

	QFile out(fileName);
	if(out.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream stream(&out);
		stream << toPlainText();
		out.close();
		emit pathChanged(fileName);
	}

	return QFileInfo(fileName);
}

void Document::lexicalAnalyze()
{

}

void Document::parse() {
	if(mCodeModel != nullptr)
		mCodeModel->parse();
}

void Document::emitTextChanged(bool b) {
	emit textChanged(this, b);
}


const QFileInfo& Document::getFileInfo() const {
	return mFileInfo;
}

const QString Document::getFullPath() const {
	if(!mFileInfo.isFile())
		return mFile;
	return mFileInfo.absoluteFilePath();
}

const QString Document::getFilePath() const {
	if(!mFileInfo.isFile())
		return mFile;
	return mFileInfo.path();
}

const QString Document::getFileName() const {
	if(!mFileInfo.isFile())
		return mFile;
	return mFileInfo.fileName();
}

const QString Document::getDynamicName() const {
	if(mFileInfo.isFile()) {
		if(isModified()){
			return "*"+getFileName();
		}
		return getFileName();
	} else {
		if(isModified()){
			return "*"+mFile;
		}
		return mFile;
	}
}

void Document::setItem(DocumentItem* item) {
	mItem = item;
}

DocumentItem* Document::getItem() const {
	return mItem;
}

bool Document::isActualFile() const {
	return QFile::exists(mFile);
}

void Document::setFile(const QString& file) {
	mFileInfo.setFile(file);
	mFile = file;
}

void Document::setHighlighter(TextEditor::Internal::Highlighter* hiltter) {
	mHighlighter = hiltter;
	hiltter->setDocument(qobject_cast<QTextDocument*>(this));
	emit highlighterChanged(this, mHighlighter);
}

bool Document::hasHighlighter() const {
	if(mHighlighter != nullptr)
		return true;
	return false;
}

void Document::setTabSettings(const TextEditor::TabSettings& tabsettings) {
	mTabSettings = tabsettings;
}

const TextEditor::TabSettings& Document::getTabSettings() const {
	return mTabSettings;
}

TextEditor::Internal::Highlighter* Document::getHighlighter() const {
	return mHighlighter;
}

void Document::close() {
	emit exterminate(this);
}

void Document::setCodeModel(CodeModel *codeModel) {
	codeModel->setMainDocument(this);

	connect(codeModel, &CodeModel::error, [](const QString &msg, const CodePoint &cp) {
		qDebug() << "Error " << cp.toString() << ": " << msg;
	});

	connect(this, &Document::contentsChange, codeModel, &CodeModel::documentEdited);


	codeModel->parse();



	mCodeModel = codeModel;
	connect(this, &Document::contentsChanged, this, &Document::parse);
}

CodeModel *Document::getCodeModel() const {
	return mCodeModel;
}


FrostEdit* Document::getEditor() const {
	QObject* parentobj = this->parent();
	return qobject_cast<FrostEdit*>(parentobj);
}



