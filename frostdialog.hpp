#ifndef FROSTDIALOG_HPP
#define FROSTDIALOG_HPP

#include <QDialog>

namespace Ui {
	class FrostDialog;
}

class TabWidget;
class FrostDialog : public QDialog
{
		Q_OBJECT

	public:
		explicit FrostDialog(QWidget *parent = 0);
		~FrostDialog();
	public slots:
		void changeTitle(TabWidget* wid, int ind);
	private slots:
		void emitClose();
	signals:
		void closed(FrostDialog*);

	private:
		Ui::FrostDialog *ui;
};

#endif // FROSTDIALOG_HPP
