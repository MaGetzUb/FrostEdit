#include "quitdialog.hpp"
#include "ui_quitdialog.h"

QuitDialog::QuitDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QuitDialog)
{
	ui->setupUi(this);
}

QuitDialog::~QuitDialog()
{
	delete ui;
}

void QuitDialog::setDocuments(const QList<Document*>& documents) {
	mDocuments = documents;
	handleItems();
}

void QuitDialog::handleItems() {
	for(auto& i: mDocuments) {
		if(i->isModified()) {

		}
	}
}
