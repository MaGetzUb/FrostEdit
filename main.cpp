#include "frostedit.hpp"
#include <QApplication>
#include <QPixmap>
#include <QSplashScreen>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QPixmap map(":/Images/Splash.png");
	QSplashScreen splash(map, Qt::WindowStaysOnTopHint);
	splash.show();
	a.processEvents();
	FrostEdit w;
	QObject::connect(&a, SIGNAL(focusChanged(QWidget*, QWidget*)), &w, SLOT(widgetChanged(QWidget*,QWidget*)));
	w.show();
	splash.finish(&w);
	return a.exec();
}
