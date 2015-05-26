#ifndef FROSTEDITOR_HPP
#define FROSTEDITOR_HPP

#include <QMainWindow>
#include <QList>
#include <QString>
#include <QFileSystemWatcher>
#include <QMap>
#include <QFont>
#include <QListWidgetItem>
#include <QSplitter>
#include <QProcess>

#include "TextEditor/document.hpp"
#include "tabwidgetframe.hpp"
#include "console.hpp"
#include "tabwidget.hpp"

#include "TextEditor/qate/highlighter.h"
#include "TextEditor/qate/highlightdefinition.h"
#include "TextEditor/qate/highlightersettings.h"
#include "TextEditor/qate/mimedatabase.h"
#include "TextEditor/qate/highlightdefinitionmanager.h"

namespace Ui {
	class FrostEdit;
}

class FrostDialog;
class QProcess;
class TextEdit;
class IssueList;
class FrostEdit : public QMainWindow {
		Q_OBJECT
		QFileSystemWatcher* mDocumentWatcher;
		QMap<QString, Document*> mOpenDocuments;
		QVector<QFileInfo> mTemporaryFiles;

		TabWidget* mCurrentTabWidget;

		QVector<TabWidgetFrame*> mTabWidgetFrames;
		QVector<FrostDialog*> mWindows;

		Qate::MimeDatabase* mMimeDatabase;
		Qate::HighlightDefinitionManager *mHiltDefManager;
		TextEditor::Internal::Highlighter *mHighlighter;
		QSharedPointer<TextEditor::Internal::HighlightDefinition> mHiltDef;

		static QString gCompilerPath;

		QFileInfo mCompileFile;

		QProcess* mFrostCompiler;
		QList<QProcess*> mRunningApplication;
		IssueList* mIssueList;

		Console* mCompileOutput;
		QTabWidget* mApplicationOutput;
		QFont mFont;
		int mNewCount;
	public:
		static QString gAppName;
		explicit FrostEdit(QWidget *parent = 0);
		~FrostEdit();
		void dropEvent(QDropEvent *);
		void dragEnterEvent(QDragEnterEvent *);
	public slots:
		void fileChangedOutside(QString);
		void updateTabHeader(Document*, bool);
	private slots:

		void addEditor(QListWidgetItem*);
		void addEditor(const QString&);
		void addEditor(TabWidgetFrame*, const QString&);

		void removeDocument(Document*);

		void addDocument(const QString&);
		void addDocumentItem(const Document* doc);
		void updateDocumentSelection(TabWidget*, int);
		void changeTitle(TabWidget*, int);

		void setActiveTabWidget(TabWidget*);
		void closeTab(TabWidget*, int);

		void currentTabPageChanged(int);


		void on_actionSave_triggered();
		void on_actionSave_As_triggered();
		void on_actionOpen_triggered();
		void on_closeDocument_clicked();

		void widgetChanged(QWidget*, QWidget*);
		int openEditors(Document* );
		void on_actionNew_triggered();

		void split(TabWidgetFrame*, Qt::Orientation);
		void splitHorizontally(TabWidgetFrame*);
		void splitVertically(TabWidgetFrame*);

		void closeTabWidgetFrame(TabWidgetFrame*);
		void openToNewWindow(TabWidgetFrame*);
		void closeWindow(FrostDialog*);

		void on_actionUndo_triggered();
		void on_actionRedo_triggered();
		void on_actionCopy_triggered();
		void on_actionCut_triggered();
		void on_actionPaste_triggered();

		void on_actionCompile_triggered();

		void printCompileStandardOutput();
		void printCompileStandardError();
		void printCompile();

		void compile();

		void compileFinished(int, QProcess::ExitStatus);
		void on_actionCompileAndRun_triggered();
		void applicationCloseRequest(int );

	signals:
		void tabWidgetChanged(TabWidget*);
		void documentChanged(Document*);
	private:
		void setUpDocumentHiltter(Document*);
		TextEdit* toTextEdit(QWidget*);
		Document* toDocument(QTextDocument*);
		TabWidgetFrame* toTabWidgetFrame(QWidget*);
		FrostDialog* toFrostDialog(QWidget*);

		Document* getActiveDocument();
		Ui::FrostEdit *ui;
		bool mRunToo;
};

#endif // FROSTEDITOR_HPP
