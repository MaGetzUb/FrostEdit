#ifndef TABWIDGET_HPP
#define TABWIDGET_HPP

#include <QTabWidget>
#include <QPushButton>

namespace Ui {
	class TabWidget;
}

class TabWidget : public QTabWidget {
		Q_OBJECT
		bool isActive;
	public:
		explicit TabWidget(QWidget *parent = 0);
		~TabWidget();

		void dropEvent(QDropEvent*) override;
		void dragEnterEvent(QDragEnterEvent*) override;
		void dragLeaveEvent(QDragLeaveEvent*) override;
		void mousePressEvent(QMouseEvent *) override;
		void mouseReleaseEvent(QMouseEvent *) override;

		void setActive(bool b);
		void paintEvent(QPaintEvent*) override;

	public slots:
		void destroyTab(int);
	private slots:
		void emitCurrentChanged(int);
	signals:
		void tabCloseRequested(TabWidget*, int);
		void currentChanged(TabWidget*, int);
	private:
		Ui::TabWidget *ui;
};

#endif // TABWIDGET_HPP
