#include "generalsettingstab.hpp"
#include "ui_generalsettingstab.h"
#include "settings.hpp"

GeneralSettingsTab::GeneralSettingsTab(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::GeneralSettingsTab)
{
	ui->setupUi(this);
}

GeneralSettingsTab::~GeneralSettingsTab()
{
	delete ui;
}


void GeneralSettingsTab::on_showLineNumbers_toggled(bool checked) {
	Settings::set("TextEditor/LineNumbersVisible", checked);
}
