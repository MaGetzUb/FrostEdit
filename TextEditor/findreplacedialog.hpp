#ifndef FINDREPLACEDIALOG_HPP
#define FINDREPLACEDIALOG_HPP

#include <QDialog>
#include "TextEditor/textedit.hpp"
#include <QTextCursor>
#include <QTextDocument>
#include <QString>

namespace Ui {
	class FindReplaceDialog;
}

class QLineEdit;
class FindReplaceDialog : public QDialog
{
		Q_OBJECT

	public:
		explicit FindReplaceDialog(QWidget *parent = 0);
		~FindReplaceDialog();

		bool isRegExfind();
		bool isCaseSensitive();
		bool isDownwardsSearch();
		bool isWholeWordsOnly();
		bool isWalkAround();

		QTextDocument::FindFlags findFlags();
		QString findString();
		QString replaceString();

		QLineEdit* findEdit();

	private slots:
		void hideThis();
		void on_findButton_clicked();

		void on_findAndReplace_clicked();

		void on_replaceAllButton_clicked();

		void emitReplaceHasText(QString);
		void emitFindHasText(QString);
		void emitSettingsChanged(bool b);
		void emitSettingsChanged(QString s);
	signals:
		void replaceHasText(bool);
		void findHasText(bool);

		void findTriggered();
		void findReplaceTriggered();
		void replaceAllTriggered();
		void settingsChanged(QString);
	private:
		Ui::FindReplaceDialog *ui;
};

#endif // FINDREPLACEDIALOG_HPP
