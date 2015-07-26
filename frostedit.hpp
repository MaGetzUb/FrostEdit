#ifndef FROSTEDITOR_HPP
#define FROSTEDITOR_HPP

#include <QMainWindow>
#include <QList>
#include <QString>
#include <QFileSystemWatcher>
#include <QMap>
#include <QFont>
#include <QTreeWidget>
#include <QListWidgetItem>
#include <QListWidget>
#include <QSplitter>
#include <QProcess>
#include <QSettings>
#include <QFileIconProvider>
#include <QTreeView>

#include "settings.hpp"
#include "TextEditor/document.hpp"
#include "TextEditor/findreplacedialog.hpp"
#include "settingsmenu.hpp"
#include "tabwidgetframe.hpp"
#include "console.hpp"
#include "tabwidget.hpp"
#include "colorscheme.hpp"
#include "filesystemmodel.hpp"

#include "TextEditor/qate/highlighter.h"
#include "TextEditor/qate/highlightdefinition.h"
#include "TextEditor/qate/highlightersettings.h"
#include "TextEditor/qate/mimedatabase.h"
#include "TextEditor/qate/highlightdefinitionmanager.h"

#include "TextEditor/frostcodemodelcontext.hpp"
#include "TextEditor/frostcodemodel.hpp"


namespace Ui {
	class FrostEdit;
}


class FileListWidget;
class ComboTabWidget;
class FrostDialog;
class QProcess;
class TextEdit;
class IssueList;
class FrostEdit : public QMainWindow {
		Q_OBJECT
		QFileSystemWatcher* mDocumentWatcher;
		QHash<QString, Document*> mOpenDocuments;
		QVector<QFileInfo> mTemporaryFiles;

		TabWidget* mCurrentTabWidget;

		QVector<TabWidgetFrame*> mTabWidgetFrames;
		QVector<FrostDialog*> mWindows;

		Qate::MimeDatabase* mMimeDatabase;
		Qate::HighlightDefinitionManager *mHiltDefManager;
		TextEditor::Internal::Highlighter *mHighlighter;
		QSharedPointer<TextEditor::Internal::HighlightDefinition> mHiltDef;
		QFileIconProvider mFileIconProvider;

		FindReplaceDialog* mFindReplace;
		SettingsMenu* mSettingsMenu;
		ComboTabWidget* mComboTabWidget;


		QFileInfo mCompileFile;

		QProcess* mFrostCompiler;
		QRegularExpression mFrostCompilerErrorRegEx;

		QList<QProcess*> mRunningApplication;
		IssueList* mIssueList;

		Console* mCompileOutput;
		QTabWidget* mApplicationOutput;
		QFont mFont;
		int mNewCount;
		QString mCompiledFile;

		FileSystemModel* mFileSystemModel;


		FileListWidget* mOpenDocumentsList;
		QTreeWidget* mProjectsTreeWidget;
		QTreeView* mFileSystemTreeView;

		Colorscheme mSyntaxStyle;

		QSharedPointer<Frost::CodeModelContext> mFrostModelContext;
	public:
		static QString gAppName;
		explicit FrostEdit(QWidget *parent = 0);
		~FrostEdit();
		void dropEvent(QDropEvent *);
		void dragEnterEvent(QDragEnterEvent *);
		void disableActions();

		void enableActions();
		bool hasDocument(const QString&);



	public slots:
		void fileChangedOutside(QString);
		void updateTabHeader(Document*, bool);
		void removeDocument(Document*);
	private slots:


		void updateSettings();

		void addEditor(QListWidgetItem*);
		void openDocument(const QModelIndex&);
		//void addEditor(const QModelIndex&);
		void addEditor(const QString&);
		void addEditor(TabWidgetFrame*, const QString&);

		void addDocumentItem(Document* doc);
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

		void pointToIssue(QListWidgetItem*);
		void parseCompileOut(QString);

		void on_actionRun_triggered();


		void on_close_clicked();

		void findFromTextEdit();
		void findAndReplaceFromTextEdit();
		void replaceAllFromTextEdit();
		void pointOutOccurances(QString);
		void on_actionFind_triggered();

		void on_actionSettings_triggered();

		void applySettings();

	signals:
		void tabWidgetChanged(TabWidget*);
		void documentChanged(Document*);
	private:

		int documentSafeClose(Document* );
		void loadStyleSheet(const QString&);

		TextEdit* currentTextEdit();

		void connectTabWidgetFrameSignals(TabWidgetFrame*);
		void closeEvent(QCloseEvent *) override;
		void setUpDocumentHiltter(Document*);
		int tabWidgetContains(TabWidget*, Document*);
		Document* addDocument(const QString&, bool ghost = false);

		void updateTabWidget(TabWidget* tabwid);

		Document* getActiveDocument();
		Ui::FrostEdit *ui;
		bool mRunToo;

	public:

		static TextEdit* toTextEdit(QWidget*);
		static Document* toDocument(QTextDocument*);
		static TabWidgetFrame* toTabWidgetFrame(QWidget*);
		static FrostDialog* toFrostDialog(QWidget*);
		static Console* toConsole(QWidget*);
};

#endif // FROSTEDITOR_HPP
