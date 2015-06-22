#include "tabwidgetframe.hpp"
#include "ui_tabwidgetframe.h"
#include <QMenu>
#include <QString>
#include <QDebug>

int TabWidgetFrame::findItem(const QString& str) {
	int ind = -1;
	for(int i = 0; i < ui->comboBox->count(); i++) {
		if(ui->comboBox->itemText(i) == str)
			ind = i;
	}

	return ind;
}

TabWidgetFrame::TabWidgetFrame(QWidget *parent) :
	QFrame(parent),
	ui(new Ui::TabWidgetFrame)
{
	ui->setupUi(this);

	QMenu* menu = new QMenu();
	menu->addAction(ui->actionSplitHorizontally);
	menu->addAction(ui->actionSplitVertically);
	menu->addAction(ui->actionNewWindow);
	menu->addSeparator();
	menu->addAction(ui->actionClose);
	ui->splitButton->setMenu(menu);

	connect(ui->comboBox, &ComboBox::currentTextChanged, this, &TabWidgetFrame::emitItemChanged);
	resize(640, 480);
}

TabWidgetFrame::~TabWidgetFrame() {
	delete ui;
}

TabWidget* TabWidgetFrame::tabWidget() {
	return ui->tabWidget;
}

bool TabWidgetFrame::hasItem(const QString& str) {
	return findItem(str) != -1;
}

void TabWidgetFrame::hasCloseAction(bool b) {
	ui->actionClose->setEnabled(b);
}

void TabWidgetFrame::hideSplitMenu(bool b) {
	ui->splitButton->setVisible(!b);
}

void TabWidgetFrame::setCurrentItem(const QString& str) {
	int ind = findItem(str);
	if(ind !=-1) {
		mShouldEmitChanged = false;
		ui->comboBox->setCurrentIndex(ind);
	}
	mShouldEmitChanged = true;
}

void TabWidgetFrame::on_actionSplitHorizontally_triggered() {
	emit splitHorizontally(this);
}

void TabWidgetFrame::on_actionSplitVertically_triggered() {
	emit splitVertically(this);
}

void TabWidgetFrame::on_actionClose_triggered() {
	emit close(this);
}

void TabWidgetFrame::on_actionNewWindow_triggered() {
	emit openToNewWindow(this);
}

void TabWidgetFrame::emitItemChanged(QString str) {
	if(mShouldEmitChanged == true)
		emit itemChanged(this, str);
}

void TabWidgetFrame::removeComboBoxItem(const QString& str) {
	mShouldEmitChanged = false;
	int ind = findItem(str);
	if(ind != -1) {
		ui->comboBox->removeItem(ind);
	}
	mShouldEmitChanged = true;
}

void TabWidgetFrame::addComboBoxItem(const QString& str) {
	mShouldEmitChanged = false;
	ui->comboBox->addItem(str);
	mShouldEmitChanged = true;
}
