#include "frostdialog.hpp"
#include "ui_frostdialog.h"
#include "frostedit.hpp"

FrostDialog::FrostDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::FrostDialog)
{
	ui->setupUi(this);
	connect(this, SIGNAL(destroyed()), this, SLOT(emitClose()));
	Qt::WindowFlags flags = windowFlags();
	Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
	flags = flags & (~helpFlag);
	flags = flags | Qt::WindowFullscreenButtonHint | Qt::WindowMinimizeButtonHint;
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
