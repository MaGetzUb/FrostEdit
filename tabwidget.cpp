#include "tabwidget.hpp"
#include "ui_tabwidget.h"
#include <QDropEvent>
#include <QDragLeaveEvent>
#include <QDebug>
#include <QDialog>
#include "TextEditor/textedit.hpp"
#include <QPainter>

TabWidget::TabWidget(QWidget *parent) :
	QTabWidget(parent),
	ui(new Ui::TabWidget),
	isActive(false)
{
	ui->setupUi(this);
	acceptDrops();
	setTabsClosable(true);
	setMovable(true);
	setMouseTracking(true);
	//this->tabBar()->installEventFilter();
	connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(destroyTab(int)));
	connect(this, SIGNAL(currentChanged(int)), this, SLOT(emitCurrentChanged(int)));

}

TabWidget::~TabWidget() {
	delete ui;
}

void TabWidget::dropEvent(QDropEvent* e) {
}

void TabWidget::dragEnterEvent(QDragEnterEvent* e) {
	e->accept();
}

void TabWidget::dragLeaveEvent(QDragLeaveEvent* e) {

}

void TabWidget::mousePressEvent(QMouseEvent* e) {
	setFocus(Qt::MouseFocusReason);
	QTabWidget::mousePressEvent(e);
}

void TabWidget::mouseReleaseEvent(QMouseEvent*) {

}

void TabWidget::setActive(bool b) {
	isActive = b;
	repaint();
}

void TabWidget::paintEvent(QPaintEvent* e) {
	QTabWidget::paintEvent(e);
	QPainter p(this);
	if(isActive) {
		p.fillRect(e->rect(), QColor(32, 128, 255, 96));
	}
}

void TabWidget::destroyTab(int id) {
	emit tabCloseRequested(this, id);
}

void TabWidget::emitCurrentChanged(int id) {
	emit currentChanged(this, id);
}
