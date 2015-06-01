#ifndef QUITDIALOG_HPP
#define QUITDIALOG_HPP

#include <QDialog>
#include "TextEditor/document.hpp"




namespace Ui {
	class QuitDialog;
}

class QuitDialog : public QDialog
{
		Q_OBJECT
		QList<Document*> mDocuments;
	public:
		explicit QuitDialog(QWidget *parent = 0);
		~QuitDialog();

		void setDocuments(const QList<Document*>& documents);
		void handleItems();
	private:
		Ui::QuitDialog *ui;
};

#endif // QUITDIALOG_HPP
