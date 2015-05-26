#include "settingsmenu.hpp"
#include "ui_settingsmenu.h"

SettingsMenu::SettingsMenu(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsMenu)
{
	ui->setupUi(this);
}

SettingsMenu::~SettingsMenu()
{
	delete ui;
}
