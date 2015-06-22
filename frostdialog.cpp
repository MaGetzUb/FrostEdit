#include "frostdialog.hpp"
#include "ui_frostdialog.h"
#include "frostedit.hpp"

FrostDialog::FrostDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::FrostDialog)
{
	ui->setupUi(this);
	connect(this, &FrostDialog::destroyed, this, &FrostDialog::emitClose);
	Qt::WindowFlags flags = 0;
	flags |= Qt::Window;
	flags |= Qt::WindowCloseButtonHint;
	flags |= Qt::WindowFullscreenButtonHint;
	flags |= Qt::WindowMinimizeButtonHint;
	flags |= Qt::WindowMaximizeButtonHint;
	setWindowFlags(flags);

}

FrostDialog::~FrostDialog() {
	delete ui;
}

void FrostDialog::changeTitle(TabWidget* wid, int ind) {
	setWindowTitle(wid->tabText(ind)+" - "+FrostEdit::gAppName);
}

void FrostDialog::emitClose() {
	emit closed(this);
}
