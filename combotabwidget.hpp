#ifndef COMBOTABWIDGET_HPP
#define COMBOTABWIDGET_HPP

#include <QWidget>

namespace Ui {
	class ComboTabWidget;
}

class ComboTabWidget : public QWidget
{
		Q_OBJECT
	public:
		explicit ComboTabWidget(QWidget *parent = 0);
		~ComboTabWidget();

		void addTab(QWidget*, const QString&);
		void addTab(QWidget*, const QIcon&, const QString&);
	private slots:

		void changeTab(int);

	private:
		Ui::ComboTabWidget *ui;
};

#endif // COMBOTABWIDGET_HPP
