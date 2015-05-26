#include "frostedit.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	FrostEdit w;
	QObject::connect(&a, SIGNAL(focusChanged(QWidget*, QWidget*)), &w, SLOT(widgetChanged(QWidget*,QWidget*)));
	w.show();

	return a.exec();
}
