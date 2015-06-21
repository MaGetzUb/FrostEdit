#include "issuelist.hpp"

IssueList::IssueList(QWidget* parent):
	QListWidget(parent),
	mWarningIco(":/icons/Icons/warning.ico"),
	mErrorIco(":/icons/Icons/error.ico")
{
}

void IssueList::addError(const QString& wholeMsg, const QString& file, const QString& error, int row, int column) {
	IssueItem* item = new IssueItem(mErrorIco, tr("ERROR! %1 in file '%2' at line %3").arg(error).arg(file).arg(row), this);
	item->setToolTip(wholeMsg);
	item->setFile(file);
	item->setRow(row);
	item->setColumn(column);
	item->setFormat(mErrorFormat);
	addItem(item);
}

void IssueList::addWarning(const QString& wholeMsg, const QString& file, const QString& warning, int row, int column) {
	IssueItem* item = new IssueItem(mWarningIco, tr("Warning! %1 in file '%2' at line %3").arg(warning).arg(file).arg(row), this);
	item->setToolTip(wholeMsg);
	item->setFile(file);
	item->setRow(row);
	item->setColumn(column);
	item->setFormat(mWarningFormat);
	addItem(item);
}

IssueList::~IssueList()
{

}



void IssueItem::setExplanation(const QString& str) {
	mExplanation = str;
}

void IssueItem::setFile(const QString& filepath) {
	mFile = filepath;
}

void IssueItem::setRow(int row) {
	mRow = row;
}

void IssueItem::setColumn(int col) {
	mCol = col;
}

void IssueList::setErrorFormat(const QTextCharFormat& fmt) {
	mErrorFormat = fmt;
}

void IssueList::setWarningFormat(const QTextCharFormat& fmt) {
	mWarningFormat = fmt;
}

const QString& IssueItem::getExplanation() {
	return mExplanation;
}

const QString& IssueItem::getFile() {
	return mFile;
}

int IssueItem::getRow() {
	return mRow;
}

int IssueItem::getColumn() {
	return mCol;
}
