#include "issuelist.hpp"

IssueList::IssueList(QWidget* parent):
	QListWidget(parent)
{
}

void IssueList::addError(const QString& file, const QString& error, int row, int column) {
	QListWidgetItem* item;
	item->setText(error);
	item->setIcon(mErrorIcon);
	item->setData(0, file.toLocal8Bit());
	item->setData(1, row);
	item->setData(2, column);
	addItem(item);
}

void IssueList::addWarning(const QString& file, const QString& warning, int row, int column) {
	QListWidgetItem* item;
	item->setText(warning);
	item->setIcon(mWarningIcon);
	item->setData(0, file.toLocal8Bit());
	item->setData(1, row);
	item->setData(2, column);
	addItem(item);
}

IssueList::~IssueList()
{

}

