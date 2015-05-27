#include "issuelist.hpp"

IssueList::IssueList(QWidget* parent):
	QListWidget(parent),
	mWarningIco(":/icons/Icons/warning.ico"),
	mErrorIco(":/icons/Icons/error.ico")
{
}

void IssueList::addError(const QString& wholeMsg, const QString& file, const QString& error, int row, int column) {
	IssueItem* item = new IssueItem(mErrorIco, tr("ERROR! ")+error+tr(" at line: %1").arg(row), this);
	item->setToolTip(wholeMsg);
	item->setFile(file);
	item->setRow(row);
	item->setColumn(column);
	addItem(item);
}

void IssueList::addWarning(const QString& wholeMsg, const QString& file, const QString& warning, int row, int column) {
	IssueItem* item = new IssueItem(mWarningIco, tr("Warning! ")+warning+tr(" at line: %1").arg(row), this);
	item->setToolTip(wholeMsg);
	item->setFile(file);
	item->setRow(row);
	item->setColumn(column);
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

const QString&IssueItem::getExplanation() {
	return mExplanation;
}

const QString&IssueItem::getFile() {
	return mFile;
}

int IssueItem::getRow() {
	return mRow;
}

int IssueItem::getColumn() {
	return mCol;
}
