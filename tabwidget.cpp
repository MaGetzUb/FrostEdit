#include "tabwidget.hpp"
#include "ui_tabwidget.h"
#include <QDropEvent>
#include <QDragLeaveEvent>
#include <QDebug>
#include <QDialog>
#include "TextEditor/textedit.hpp"
#include <QPainter>
#include <QTextDocument>


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
	connect(this, &QTabWidget::tabCloseRequested, this, &TabWidget::destroyTab);
	connect(this, &QTabWidget::currentChanged, this, &TabWidget::emitCurrentChanged);

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

	float width = e->rect().size().width()/2;
	QTextDocument doc;

	doc.documentLayout()->setPaintDevice(p.device());
	doc.setHtml("<center><font size='10'><b><u>Open a document<u></b></font></center><font size='6'><ul><li>File menu > Open (Ctrl + O)</li><li>Drag and drop files here</li><li>If any, selecting file from the open documents list</li><li>Selecting file from the drop box in the header.</li></ul></font>");
	//doc.setTextWidth(width/2);
	p.setPen(palette().foreground().color());

	QRectF rct = e->rect();
	rct.setWidth(width);
	p.translate(rct.size().width()/2, rct.size().height()/2);
	doc.drawContents(&p, rct);

}

void TabWidget::destroyTab(int id) {
	emit tabCloseRequested(this, id);
}

void TabWidget::emitCurrentChanged(int id) {
	emit currentChanged(this, id);
}
