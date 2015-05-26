#ifndef TABWIDGETFRAME_HPP
#define TABWIDGETFRAME_HPP

#include <QFrame>
#include "tabwidget.hpp"
namespace Ui {
	class TabWidgetFrame;
}

class TabWidgetFrame : public QFrame
{
		Q_OBJECT
		bool isClosable;
		bool mShouldEmitChanged;
		int findItem(const QString& str);
	public:
		explicit TabWidgetFrame(QWidget *parent = 0);
		~TabWidgetFrame();

		TabWidget* tabWidget();
		const QString& getCurrentItem() const;
		bool hasItem(const QString& str);
	public slots:
		void hasCloseAction(bool b);
		void hideSplitMenu(bool b);

		void setCurrentItem(const QString& str);
		void addComboBoxItem(const QString& str);
		void removeComboBoxItem(const QString& str);


	private slots:


		void on_actionSplitHorizontally_triggered();
		void on_actionSplitVertically_triggered();
		void on_actionClose_triggered();
		void on_actionNewWindow_triggered();
		void emitItemChanged(QString);


	signals:
		void splitHorizontally(TabWidgetFrame*);
		void splitVertically(TabWidgetFrame*);
		void close(TabWidgetFrame*);
		void openToNewWindow(TabWidgetFrame*);
		void itemChanged(TabWidgetFrame*, QString item);
	private:
		Ui::TabWidgetFrame *ui;
};

#endif // TABWIDGETFRAME_HPP
