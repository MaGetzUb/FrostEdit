#include "findreplacedialog.hpp"
#include "ui_findreplacedialog.h"


FindReplaceDialog::FindReplaceDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::FindReplaceDialog)
{
	ui->setupUi(this);
	setHidden(true);
	connect(ui->doneButon, SIGNAL(pressed()), this, SLOT(hideThis()));
	connect(ui->replaceText, SIGNAL(textChanged(QString)), this, SLOT(emitReplaceHasText(QString)));
	connect(ui->findText, SIGNAL(textChanged(QString)), this, SLOT(emitFindHasText(QString)));
	connect(ui->findText, SIGNAL(textChanged(QString)), this, SLOT(emitSettingsChanged(QString)));

	connect(this, SIGNAL(replaceHasText(bool)), ui->replaceAllButton, SLOT(setEnabled(bool)));
	connect(this, SIGNAL(replaceHasText(bool)), ui->findAndReplace, SLOT(setEnabled(bool)));
	connect(this, SIGNAL(findHasText(bool)), ui->findButton, SLOT(setEnabled(bool)));

	connect(ui->useRegExp, SIGNAL(clicked(bool)), this, SLOT(emitSettingsChanged(bool)));
	connect(ui->caseSensitive, SIGNAL(clicked(bool)), this, SLOT(emitSettingsChanged(bool)));
	connect(ui->completeWordsOnly, SIGNAL(clicked(bool)), this, SLOT(emitSettingsChanged(bool)));
	connect(ui->caseSensitive, SIGNAL(clicked(bool)), this, SLOT(emitSettingsChanged(bool)));


	Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
	Qt::WindowFlags flags = windowFlags();
	flags = flags & (~helpFlag);
	setWindowFlags(flags);
}

FindReplaceDialog::~FindReplaceDialog()
{
	delete ui;
}

bool FindReplaceDialog::isRegExfind() {
	return ui->useRegExp->isChecked();
}

bool FindReplaceDialog::isCaseSensitive() {
	return ui->caseSensitive->isChecked();
}

bool FindReplaceDialog::isDownwardsSearch() {
	return ui->findDownwards->isChecked();
}

bool FindReplaceDialog::isWholeWordsOnly() {
	return ui->completeWordsOnly->isChecked();
}

bool FindReplaceDialog::isWalkAround() {
	return ui->walkAround->isChecked();
}

QTextDocument::FindFlags FindReplaceDialog::findFlags() {
	QTextDocument::FindFlags flags = QTextDocument::FindFlag(0x0);
	flags |= (!isDownwardsSearch()) ? QTextDocument::FindBackward : QTextDocument::FindFlag(0x0);
	flags |= (isWholeWordsOnly()) ? QTextDocument::FindWholeWords : QTextDocument::FindFlag(0x0);
	flags |= (isCaseSensitive()) ? QTextDocument::FindCaseSensitively : QTextDocument::FindFlag(0x0);
	return flags;
}

QString FindReplaceDialog::findString() {
	return ui->findText->text();
}

QString FindReplaceDialog::replaceString() {
	return ui->replaceText->text();
}

QLineEdit* FindReplaceDialog::findEdit() {
	return ui->findText;
}

void FindReplaceDialog::hideThis() {
	setHidden(true);
}

void FindReplaceDialog::on_findButton_clicked() {
	emit findTriggered();
}

void FindReplaceDialog::on_findAndReplace_clicked() {
	emit findReplaceTriggered();
}

void FindReplaceDialog::on_replaceAllButton_clicked() {
	emit replaceAllTriggered();
}

void FindReplaceDialog::emitReplaceHasText(QString) {
	emit replaceHasText(!ui->replaceText->text().isEmpty() && !ui->findText->text().isEmpty());
}

void FindReplaceDialog::emitFindHasText(QString) {
	emit findHasText(!ui->findText->text().isEmpty());
}

void FindReplaceDialog::emitSettingsChanged(bool b) {
	emit settingsChanged(ui->findText->text());
}

void FindReplaceDialog::emitSettingsChanged(QString s) {
	emit settingsChanged(ui->findText->text());
}
