#include "combotabwidget.hpp"
#include "ui_combotabwidget.h"
#include <QDebug>

ComboTabWidget::ComboTabWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ComboTabWidget)
{
	ui->setupUi(this);
	ui->tabWidget->tabBar()->setHidden(true);
	ui->tabWidget->tabBar()->setDisabled(true);
//static_cast<void(FrostEdit::*)(TabWidgetFrame*, const QString&)>
	connect(ui->comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ComboTabWidget::changeTab);

}

ComboTabWidget::~ComboTabWidget()
{
	delete ui;
}

void ComboTabWidget::addTab(QWidget* widget, const QString& name) {
	ui->tabWidget->addTab(widget, name);
	ui->comboBox->addItem(name, QVariant(ui->tabWidget->indexOf(widget)));
}

void ComboTabWidget::addTab(QWidget* widget, const QIcon& icon, const QString& name) {
	ui->tabWidget->addTab(widget, name);
	ui->comboBox->addItem(icon, name, QVariant(ui->tabWidget->indexOf(widget)));
}

void ComboTabWidget::changeTab(int id) {
	int ind = ui->comboBox->itemData(id).toInt();
	ui->tabWidget->setCurrentIndex(ind);
	qDebug() << ind;
}
