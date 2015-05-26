#include "combobox.hpp"
#include <QPaintEvent>
ComboBox::ComboBox(QWidget *parent):
	QComboBox(parent)
{
}

ComboBox::~ComboBox()
{
}

void ComboBox::paintEvent(QPaintEvent* e) {
	QStyleOptionComboBox opt;
	opt.fontMetrics.elidedText(currentText(), Qt::ElideLeft, e->rect().width());
	initStyleOption(&opt);
	QComboBox::paintEvent(e);
}
