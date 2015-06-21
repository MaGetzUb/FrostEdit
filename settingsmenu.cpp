#include "settingsmenu.hpp"
#include "ui_settingsmenu.h"
#include <QCloseEvent>
#include "settings.hpp"

SettingsMenu::SettingsMenu(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsMenu)
{
	ui->setupUi(this);
	setHidden(true);

}

SettingsMenu::~SettingsMenu() {
	delete ui;
}

AppearanceTab* SettingsMenu::appearanceTab()
{
	return ui->appearance;
}


void SettingsMenu::on_okButton_clicked() {
	on_applyButton_clicked();
	setHidden(true);
}

void SettingsMenu::on_applyButton_clicked() {
	ui->appearance->updateSyntaxStyle();
	Settings::set("TextEditor/Font", ui->appearance->mFont.family());
	Settings::set("TextEditor/FontSize", ui->appearance->mFont.pointSize());
	Settings::sync();
	emit settingsApplied();
}

void SettingsMenu::on_cancelButton_clicked() {
	ui->appearance->useSyntaxStyle();
	setHidden(true);
}

void SettingsMenu::closeEvent(QCloseEvent* e) {
	on_cancelButton_clicked();
	e->accept();
}
