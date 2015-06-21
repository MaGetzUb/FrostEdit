#include "frostedit.hpp"
#include "ui_frostedit.h"
#include <QDebug>
#include <QLineEdit>
#include <QStringList>
#include <QFileDialog>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QTreeWidgetItem>
#include <QDebug>
#include <QDialog>
#include <QString>
#include <QProcess>
#include <QFile>
#include <QResource>

#include "TextEditor/textedit.hpp"
#include "frostdialog.hpp"
#include "issuelist.hpp"

#include "TextEditor/qate/defaultcolors.h"
#include "TextEditor/fatehighlighter.hpp"

QString FrostEdit::gAppName = "FrostEdit";

FrostEdit::FrostEdit(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::FrostEdit),
	mFont("Lucida Console"),
	mNewCount(1),
	mFrostCompiler(new QProcess(this)),
	mFileIconProvider(),
	mMimeDatabase(new Qate::MimeDatabase),
	mHiltDefManager(Qate::HighlightDefinitionManager::instance()),
	mFrostCompilerErrorRegEx("\\\"(.*)\\\"\\s+\\[\\s*(\\d+)\\,\\s*(\\d*)\\s*\\]\\s+(Warning|Error)\\s+(\\d+)\\:\\s+(.*)"),
	mFindReplace(new FindReplaceDialog(this)),
	mSettingsMenu(new SettingsMenu(this))
{

	mDocumentWatcher = new QFileSystemWatcher(this);
	connect(mDocumentWatcher, SIGNAL(fileChanged(QString)), this, SLOT(fileChangedOutside(QString)));
	ui->setupUi(this);


	mTabWidgetFrames.append(new TabWidgetFrame(this));
	mTabWidgetFrames.last()->hasCloseAction(false);
	mCurrentTabWidget = mTabWidgetFrames.last()->tabWidget();
	setCentralWidget(mTabWidgetFrames.last());

	connect(ui->openFilesWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addEditor(QListWidgetItem*)));

	for(TabWidgetFrame* tab: mTabWidgetFrames) {
		connectTabWidgetFrameSignals(tab);
	}



	connect(this, SIGNAL(tabWidgetChanged(TabWidget*)), this, SLOT(setActiveTabWidget(TabWidget*)));
	currentTabPageChanged(0);
	connect(mCurrentTabWidget, SIGNAL(currentChanged(int)), this, SLOT(currentTabPageChanged(int)));

	connect(mFrostCompiler, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(compileFinished(int, QProcess::ExitStatus)));

	connect(mFindReplace, SIGNAL(findTriggered()), this, SLOT(findFromTextEdit()));
	connect(mFindReplace, SIGNAL(findReplaceTriggered()), this, SLOT(findAndReplaceFromTextEdit()));
	connect(mFindReplace, SIGNAL(replaceAllTriggered()), this, SLOT(replaceAllFromTextEdit()));
	connect(mFindReplace, SIGNAL(settingsChanged(QString)), this, SLOT(pointOutOccurances(QString)));

	ui->browserWidget->setVisible(false);

	setAcceptDrops(true);
	mHiltDefManager->setMimeDatabase(mMimeDatabase);


#ifdef _WIN32
	//mHiltDefManager->addDefinitionPath("C:\\Qt\\Tools\\QtCreator\\share\\qtcreator\\generic-highlighter\\");
	//mHiltDefManager->addDefinitionPath(QDir::homePath() + "\\AppData\\Roaming\\QtProject\\qtcreator\\generic-highlighter");
#else
	// ubuntu/debian
	mHiltDefManager->addDefinitionPath("/usr/share/kde4/apps/katepart/syntax/");
	// arch
	mHiltDefManager->addDefinitionPath("/usr/share/apps/katepart/syntax/");
	// user instaled dirs
	mHiltDefManager->addDefinitionPath("~/.kde/share/apps/katepart/syntax/");
	mHiltDefManager->addDefinitionPath("~/.kde4/share/apps/katepart/syntax/");
#endif
	//mHiltDefManager->addDefinitionPath("Highlighters/");
	//mHiltDefManager->registerMimeTypes();

	//mHiltDef = mHiltDefManager->definition("Highlighters/frostbasic.xml");

	mCompileOutput = new Console(ui->ConsoleArea);
	mCompileOutput->setFont(mFont);
	mCompileOutput->disconnectSignalsAfterFinishing(false);


	mCompileOutput->hookToProcess(mFrostCompiler);
	connect(mCompileOutput, SIGNAL(stdOutAdded(QString)), this, SLOT(interpretCompileOut(QString)));

	mApplicationOutput = new QTabWidget(ui->ConsoleArea);
	mApplicationOutput->setTabsClosable(true);

	connect(mApplicationOutput, SIGNAL(tabCloseRequested(int)), this, SLOT(applicationCloseRequest(int)));

	mIssueList = new IssueList(ui->ConsoleArea);
	ui->consoleTabs->addTab(mIssueList, "Issues");
	ui->consoleTabs->addTab(mCompileOutput, "Console");
	ui->consoleTabs->addTab(mApplicationOutput, "Application");
	connect(mIssueList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(pointToIssue(QListWidgetItem*)));
	connect(mSettingsMenu, SIGNAL(settingsApplied()), this, SLOT(applySettings()));


	mCompileOutput->show();
	mIssueList->show();

	if(Settings::firstTime()) {
		if(Settings::get("DefaultCompiler/Path").isNull()) {
			QString compiler = QFileDialog::getExistingDirectory(this, tr("Set cbcompiler directory"), tr(""));
			Settings::set("DefaultCompiler/Path", compiler);
		}
		Settings::set("Appearance/StyleSheet", "");
		Settings::set("Appearance/ColorScheme", "");

		Settings::set("DefaultCompiler/Environment", "");
		Settings::set("TextEditor/Font", "Lucida Console");
		Settings::set("TextEditor/FontSize", 10);

		Settings::sync();
	} else {
		if(Settings::get("DefaultCompiler/Path").isNull()) {
			QString compiler = QFileDialog::getExistingDirectory(this, tr("Set cbcompiler directory"), tr(""));
			Settings::set("DefaultCompiler/Path", compiler);
		}
		Settings::sync();
	}

	updateSettings();

}

FrostEdit::~FrostEdit() {
	for(QMap<QString, Document*>::iterator i = mOpenDocuments.begin(); i != mOpenDocuments.end(); i++) {
		delete mOpenDocuments[i.key()];
		mDocumentWatcher->removePath(i.key());
	}
	mOpenDocuments.clear();

	delete mCompileOutput;
	for(int i = 0; i < mApplicationOutput->count(); i++) {
		Console* c = qobject_cast<Console*>(mApplicationOutput->widget(i));
		mApplicationOutput->removeTab(i);
		delete c;
	}

	delete mDocumentWatcher;
	delete mFindReplace;
	delete mSettingsMenu;
	delete ui;
}

void FrostEdit::dropEvent(QDropEvent* e)  {
	const QMimeData* data = e->mimeData();
	if(data != nullptr) {
		for(QUrl u: data->urls()) {
			QString i = u.path().mid(1);
			addDocument(i);
			addEditor(i);
		}
	}
}

void FrostEdit::dragEnterEvent(QDragEnterEvent* e) {
	e->accept();
}

void FrostEdit::fileChangedOutside(QString str) {
	blockSignals(true);
	QFileInfo finfo(str);
	int id = QMessageBox::warning(this, "Warning!", QString("File: ")+finfo.fileName()+" has been modified outside of the editor!\nDo you want to reload it?", "Yes", "No");
	if(id == 0)
		mOpenDocuments[str]->reload();
	blockSignals(false);
}

void FrostEdit::updateTabHeader(Document* doc, bool b) {
	for(TabWidgetFrame* tab: mTabWidgetFrames) {
		for(int i = 0; i < tab->tabWidget()->count(); i++) {
			TextEdit* edit = toTextEdit(tab->tabWidget()->widget(i));
			if(edit != nullptr && edit->document() == doc) {
				tab->tabWidget()->setTabText(i, doc->getDynamicName());
				changeTitle(tab->tabWidget(), i);
			}
		}
	}
}

void FrostEdit::updateSettings() {
	loadStyleSheet(Settings::get("Appearance/Stylesheet", "").toString());
	mSyntaxStyle.load(Settings::get("Appearance/Colorscheme").toString());
	mSettingsMenu->appearanceTab()->setSyntaxStyle(&mSyntaxStyle);
	mFont.setFamily(Settings::get("TextEditor/Font", "Lucida Console").toString());
	mFont.setPointSize(Settings::get("TextEditor/FontSize", 10).toInt());
	mSettingsMenu->appearanceTab()->setTextEditFont(mFont);
	Settings::instance().sync();
}

void FrostEdit::on_actionOpen_triggered() {
	QStringList list = QFileDialog::getOpenFileNames(this, "Open file", QDir::currentPath(), "FrostBasic Code Files (*.frb);;CoolBasic Code Files (*.cb);;All Files(*.*)");
	for(auto& i: list) {
		addDocument(i);
		addEditor(i);
	}
}

void FrostEdit::addEditor(QListWidgetItem* item) {
	DocumentItem* docitem = static_cast<DocumentItem*>(item);
	Document* doc = docitem->getDocument();
	for(int i = 0; i < mCurrentTabWidget->count(); i++) {
		TextEdit* edit = toTextEdit(mCurrentTabWidget->widget(i));
		if(toDocument(edit->document()) == doc) {
			mCurrentTabWidget->setCurrentIndex(i);
			return;
		}
	}

	TextEdit* edit = new TextEdit(mCurrentTabWidget, doc);
	edit->setFindReplaceInstance(mFindReplace);
	edit->setFont(mFont);
	mSyntaxStyle.applyToTextEdit(edit);
	mCurrentTabWidget->addTab(edit, doc->getDynamicName());
	mCurrentTabWidget->setCurrentIndex(mCurrentTabWidget->count()-1);
}

void FrostEdit::addEditor(const QString& path) {
	Document* doc = mOpenDocuments[path];

	for(TabWidgetFrame* tabwidget: mTabWidgetFrames) {
		//TextEdit* edit = new TextEdit(tabwidget->tabWidget(), doc);
		//edit->setFont(mFont);
		//tabwidget->tabWidget()->addTab(edit, doc->getDynamicName());
		addEditor(tabwidget, path);
	}

}

void FrostEdit::addEditor(TabWidgetFrame* wid, const QString& str) {
	Document* doc = addDocument(str);

	if(doc == nullptr)
		return;

	int index = tabWidgetContains(wid->tabWidget(), doc);
	if(index >= 0) {
		wid->tabWidget()->setCurrentIndex(index);
		return;
	}

	TextEdit* edit = new TextEdit(wid->tabWidget(), doc);
	edit->setFindReplaceInstance(mFindReplace);
	edit->setFont(mFont);
	mSyntaxStyle.applyToTextEdit(edit);

	wid->tabWidget()->addTab(edit, doc->getDynamicName());
	wid->tabWidget()->setCurrentIndex(wid->tabWidget()->count()-1);

}

void FrostEdit::removeDocument(Document* doc) {

	for(TabWidgetFrame* tab: mTabWidgetFrames) {
		tab->removeComboBoxItem(doc->getFullPath());
		for(int i = 0; i < tab->tabWidget()->count(); i++) {
			TextEdit* edit = toTextEdit(tab->tabWidget()->widget(i));
			if(edit->document() == doc) {
				tab->tabWidget()->removeTab(i);
				delete edit;
			}
		}
	}

	mOpenDocuments.remove(doc->getFullPath());
	mDocumentWatcher->removePath(doc->getFullPath());
	disconnect(doc, SIGNAL(textChanged(Document*,bool)), this, SLOT(updateTabHeader(Document*, bool)));
	delete doc;
	doc = nullptr;
}


void FrostEdit::on_actionSave_triggered() {
	TextEdit* e = toTextEdit(mCurrentTabWidget->currentWidget());
	Document* doc = toDocument(e->document());
	if(doc->getFileInfo().isFile()) {
		mDocumentWatcher->removePath(doc->getFullPath());
		doc->save();
		mDocumentWatcher->addPath(doc->getFullPath());
	} else {
		QString prevPath = doc->getFullPath();
		on_actionSave_As_triggered();
		if(mCompiledFile == prevPath)
			mCompiledFile = doc->getFullPath();
	}
	updateTabHeader(doc, doc->isModified());
}

void FrostEdit::updateDocumentSelection(TabWidget* wid, int sel) {
	if(wid->count() == 0)
		return;
	if(!wid->count())
		return;

	TextEdit* e = toTextEdit(wid->widget(sel));
	if(e == NULL)
		return;
	Document* doc = toDocument(e->document());
	TabWidgetFrame* parentWidget = toTabWidgetFrame(wid->parentWidget());
	parentWidget->setCurrentItem(doc->getFullPath());
	doc->getItem()->setSelected(true);

}

void FrostEdit::changeTitle(TabWidget* wid, int ind) {
	if(wid->currentIndex() >= 0)
		setWindowTitle(wid->tabText(ind)+" - "+gAppName);
	else
		setWindowTitle(gAppName);
}

void FrostEdit::setActiveTabWidget(TabWidget* wid) {

	if (!wid) return;

	QWidget* curwid = wid->currentWidget();
	TextEdit* e = toTextEdit(curwid);

	TabWidgetFrame* frame = toTabWidgetFrame(wid->parentWidget());
	FrostDialog* diag =toFrostDialog(frame->parentWidget());
	if(diag == NULL)
		changeTitle(wid, wid->currentIndex());

	if(e == nullptr) {
		ui->actionCopy->setDisabled(true);
		ui->actionCut->setDisabled(true);
		ui->actionPaste->setDisabled(true);
		ui->actionUndo->setDisabled(true);
		ui->actionRedo->setDisabled(true);
		ui->actionCompile->setDisabled(true);
		ui->actionCompileAndRun->setDisabled(true);
		ui->actionSave->setDisabled(true);
		ui->actionSave_As->setDisabled(true);
	} else {
		ui->actionCopy->setEnabled(true);
		ui->actionCut->setEnabled(true);
		ui->actionPaste->setEnabled(true);
		ui->actionUndo->setEnabled(true);
		ui->actionRedo->setEnabled(true);
		ui->actionCompile->setEnabled(true);
		ui->actionCompileAndRun->setEnabled(true);
		ui->actionSave->setEnabled(true);
		ui->actionSave_As->setEnabled(true);
	}

}

void FrostEdit::closeTab(TabWidget* wid, int id) {
	qDebug() << wid  <<  id;
	TextEdit* e = toTextEdit(wid->widget(id));
	Document* doc = toDocument(e->document());

	int editors = openEditors(doc);
	qDebug() << "Open in editors: " << editors;
	int ans = -1;
	if(editors == 1) {
		ans = documentSafeClose(doc);
		if(ans == 0 || ans == 1) {
			wid->removeTab(id);
			delete e;
			removeDocument(doc);
			return;
		}
	}
	wid->removeTab(id);

}

void FrostEdit::disableActions() {
	ui->actionCut->setDisabled(true);
	ui->actionPaste->setDisabled(true);
	ui->actionUndo->setDisabled(true);
	ui->actionRedo->setDisabled(true);
	ui->actionCompile->setDisabled(true);
	ui->actionCompileAndRun->setDisabled(true);
	ui->actionSave->setDisabled(true);
	ui->actionSave_As->setDisabled(true);
	ui->actionCopy->setDisabled(true);
}

void FrostEdit::enableActions() {
	ui->actionCopy->setEnabled(true);
	ui->actionCut->setEnabled(true);
	ui->actionPaste->setEnabled(true);
	ui->actionUndo->setEnabled(true);
	ui->actionRedo->setEnabled(true);
	ui->actionCompile->setEnabled(true);
	ui->actionCompileAndRun->setEnabled(true);
	ui->actionSave->setEnabled(true);
	ui->actionSave_As->setEnabled(true);
}

void FrostEdit::currentTabPageChanged(int id) {
	if(id == -1) {
		disableActions();
		return;
	}
	QWidget* wid = mCurrentTabWidget->widget(id);
	if(wid == nullptr) {
		disableActions();
		return;
	}
	TextEdit* e = toTextEdit(wid);
	if(e != nullptr &&  e->document() != getActiveDocument())
		emit documentChanged(toDocument(e->document()));
	//if there's no editor, let's disable some buttons.
	if(e == nullptr) {
		disableActions();
		ui->actionFind->setDisabled(true);
		ui->compile->setDisabled(true);
		ui->compile_build->setDisabled(true);
	} else { //there was editor, enable them
		enableActions();
		ui->compile->setEnabled(true);
		ui->compile_build->setEnabled(true);
	}
}

Document* FrostEdit::addDocument(const QString& path, bool ghost) {
	//If path exists in the open documents, we will return the pointer from list
	QFileInfo tmpInfo(path);
	QString loadPath = path;
	if(tmpInfo.exists() && tmpInfo.isFile()) {
		loadPath = tmpInfo.absoluteFilePath();
	}

	if(mOpenDocuments.contains(loadPath)) {
		Document* doc = mOpenDocuments[loadPath];
		return doc;
	}


	//If file didn't exists and the path isn't file at all (and it's not ghost), we will make a MessageBox.
	if(!QFile::exists(loadPath)) {
		if(ghost == false)
			QMessageBox::critical(this, "ERROR!", tr("File: %1 does not exists!").arg(loadPath), "OK");
		return nullptr;
	}

	//We make a new file..
	Document* doc = new Document(this, loadPath);
	mOpenDocuments.insert(path, doc);
	//Setting up a highlighter for it..
	setUpDocumentHiltter(doc);

	//If file isn't ghost we'll ad it into a open documents list
	if(ghost == false)
		addDocumentItem(doc);

	//Let's keep eye on the file, to get notifications if the file was modified outside of the editor
	mDocumentWatcher->addPath(path);

	//Let's add the file in every TabWidgetFrames' comboboxes..
	for(TabWidgetFrame* tab: mTabWidgetFrames) {
		tab->addComboBoxItem(loadPath);
	}

	//Let's connect the textChanged signal to update tab header...
	connect(doc, SIGNAL(textChanged(Document*,bool)), this, SLOT(updateTabHeader(Document*, bool)));

	//We're done here.
	return doc;
}

void FrostEdit::addDocumentItem(Document* doc) {
	DocumentItem* item = new DocumentItem(ui->openFilesWidget, doc);
	item->setIcon(mFileIconProvider.icon(doc->getFileInfo()));
	ui->openFilesWidget->addItem(item);
	doc->setItem(item);
}

void FrostEdit::on_actionSave_As_triggered() {
	QString file = QFileDialog::getSaveFileName(this, "Save as", "", "FrostBasic code files (*.frb);;CoolBasic code files (*.cb);;Text files (.txt);;All files (*.*)");
	if(!file.isEmpty()) {
		TextEdit* e = toTextEdit(mCurrentTabWidget->currentWidget());
		Document* doc = toDocument(e->document());
		mDocumentWatcher->removePath(doc->getFullPath());

		for(TabWidgetFrame* tab: mTabWidgetFrames) {
			tab->removeComboBoxItem(doc->getFullPath());
			tab->addComboBoxItem(file);
		}

		doc->saveAs(file);
		mDocumentWatcher->addPath(file);
		updateTabHeader(doc, doc->isModified());
		doc->getItem()->update();
	}
}



void FrostEdit::on_closeDocument_clicked() {
	if(ui->openFilesWidget->currentItem() == nullptr)
		return;
	DocumentItem* item = static_cast<DocumentItem*>(ui->openFilesWidget->currentItem());
	Document* doc = item->getDocument();

	int ans = documentSafeClose(doc);
	if(ans == 0 || ans == 1)
		removeDocument(doc);
}

void FrostEdit::widgetChanged(QWidget* old, QWidget* now) {

	if(old == now)
		return;

	//Let's test was the last clicked widget A) TextEdit B) TabWidget C) TabBar
	TextEdit* wid = toTextEdit(now);
	TabWidget* tabwid = qobject_cast<TabWidget*>(now);
	QTabBar* bar = qobject_cast<QTabBar*>(now) ;

	//Was TabBar
	if(bar != nullptr && tabwid == nullptr && wid == nullptr) {
		tabwid = qobject_cast<TabWidget*>(bar->parentWidget());
		updateTabWidget(tabwid);

	} else if(wid != nullptr && tabwid == nullptr && bar == nullptr) {
		tabwid = qobject_cast<TabWidget*>(wid->getParentWidget());
		updateTabWidget(tabwid);

	} else if(tabwid != nullptr && bar == nullptr) {
		updateTabWidget(tabwid);
	}

	if(mCurrentTabWidget != nullptr) {
		if(mCurrentTabWidget->count() != 0 && mCurrentTabWidget != nullptr && mCurrentTabWidget->currentWidget() != nullptr) {
			QTextDocument* doc = toTextEdit(mCurrentTabWidget->currentWidget())->document();
			emit documentChanged(toDocument(doc));
		}
	}
}

int FrostEdit::openEditors(Document* doc) {
	int open = 0;
	for(TabWidgetFrame* tab: mTabWidgetFrames) {
		for(int i = 0; i < tab->tabWidget()->count(); i++) {
			TextEdit* edit = toTextEdit(tab->tabWidget()->widget(i));
			if(edit != NULL && edit->document() == doc)
				open++;
		}
	}
	return open;
}

void FrostEdit::on_actionNew_triggered() {
	QString newfile = tr("New<%1>").arg(mNewCount);
	Document* doc = new Document(this);
	mOpenDocuments[newfile] = doc;
	mOpenDocuments[newfile]->setFile(newfile);
	mNewCount++;


	setUpDocumentHiltter(doc);
	addDocumentItem(doc);
	connect(doc, SIGNAL(textChanged(Document*,bool)), this, SLOT(updateTabHeader(Document*, bool)));

	for(TabWidgetFrame* tab: mTabWidgetFrames) {
		tab->addComboBoxItem(newfile);
	}
	addEditor(newfile);
}

void FrostEdit::split(TabWidgetFrame* tab, Qt::Orientation orient) {
	QWidget* parent = tab->parentWidget();

	QSplitter* parentsplitter = qobject_cast<QSplitter*>(parent);
	FrostEdit* parentedit = qobject_cast<FrostEdit*>(parent);

	qDebug()<<parentsplitter;
	qDebug()<<parentedit;

	int size = (orient == Qt::Horizontal ? tab->width() : tab->height()) / 2;


	//Currently the tabwidget frame's parently is the Editor itself
	if(parentedit != nullptr && parentsplitter == nullptr) {
		QSplitter* splitter(new QSplitter(orient));

		qDebug() << centralWidget();

		mTabWidgetFrames.append(new TabWidgetFrame(splitter));
		tab->setParent(splitter);
		splitter->addWidget(tab);
		splitter->addWidget(mTabWidgetFrames.last());

		splitter->setSizes({size, size});
		setCentralWidget(splitter);
	//Parent widget seems to be splitter
	} else if(parentsplitter != nullptr && parentedit == nullptr) {
		QSplitter* splitter(new QSplitter(orient));
		parentsplitter->addWidget(splitter);
		splitter->addWidget(tab);
		mTabWidgetFrames.append(new TabWidgetFrame());

		splitter->addWidget(mTabWidgetFrames.last());
		splitter->setSizes({size, size});
		splitter->show();
	}


	connectTabWidgetFrameSignals(mTabWidgetFrames.last());

	for(Document* doc: mOpenDocuments)
		if(doc != nullptr)
			mTabWidgetFrames.last()->addComboBoxItem(doc->getFullPath());


}

void FrostEdit::splitHorizontally(TabWidgetFrame* tab) {
	split(tab, Qt::Horizontal);
}

void FrostEdit::splitVertically(TabWidgetFrame* tab) {
	split(tab, Qt::Vertical);
}



void FrostEdit::closeTabWidgetFrame(TabWidgetFrame* tabWidFrame) {
	QWidget* parent = tabWidFrame->parentWidget();
	QSplitter* parentsplitter = qobject_cast<QSplitter*>(parent);
	if(tabWidFrame->tabWidget() == mCurrentTabWidget) mCurrentTabWidget = nullptr;
	if(parentsplitter != NULL) {

		for(int i = 0; i < tabWidFrame->tabWidget()->count(); i++) {
			closeTab(tabWidFrame->tabWidget(), i);
		}

		int index = mTabWidgetFrames.indexOf(tabWidFrame);
		mTabWidgetFrames.first()->tabWidget()->setActive(true);
		mTabWidgetFrames.remove(index);
		delete tabWidFrame;



		if(!parentsplitter->count() > 1) {
			FrostEdit* edit = qobject_cast<FrostEdit*>(parentsplitter->parentWidget());
			QSplitter* splitter = qobject_cast<QSplitter*>(parentsplitter->parentWidget());
			TabWidgetFrame* wid = toTabWidgetFrame(parentsplitter->widget(0));
			if(edit != nullptr && splitter == nullptr) {
				edit->setCentralWidget(wid);
				wid->setParent(edit);
			} else if (splitter != nullptr && edit == nullptr) {
				splitter->addWidget(wid);
				wid->setParent(splitter);
			}

			delete parentsplitter;
		}

	}


}

void FrostEdit::openToNewWindow(TabWidgetFrame* wid) {
	FrostDialog* dialog = new FrostDialog(this);
	dialog->setLayout(new QGridLayout(dialog));
	dialog->setWindowTitle(gAppName);
	mTabWidgetFrames.append(new TabWidgetFrame(dialog));

	connect(mTabWidgetFrames.last()->tabWidget(), SIGNAL(currentChanged(TabWidget*, int)), this, SLOT(updateDocumentSelection(TabWidget*, int)));
	connect(mTabWidgetFrames.last()->tabWidget(), SIGNAL(currentChanged(TabWidget*, int)), dialog, SLOT(changeTitle(TabWidget*, int)));
	connect(mTabWidgetFrames.last()->tabWidget(), SIGNAL(tabCloseRequested(TabWidget*,int)), this, SLOT(closeTab(TabWidget*, int)));
	connect(mTabWidgetFrames.last(), SIGNAL(close(TabWidgetFrame*)), this, SLOT(closeTabWidgetFrame(TabWidgetFrame*)));
	connect(dialog, SIGNAL(closed(FrostDialog*)), this, SLOT(closeWindow(FrostDialog*)));
	connect(mTabWidgetFrames.last(), SIGNAL(itemChanged(TabWidgetFrame*,QString)), this, SLOT(addEditor(TabWidgetFrame*,QString)));


	dialog->layout()->setMargin(0);
	dialog->layout()->addWidget(mTabWidgetFrames.last());
	dialog->show();

	for(Document* doc: mOpenDocuments) {
		mTabWidgetFrames.last()->addComboBoxItem(doc->getFullPath());
	}


	mTabWidgetFrames.last()->hideSplitMenu(true);
	mWindows.append(dialog);
}

void FrostEdit::closeWindow(FrostDialog* dia) {
	TabWidgetFrame* tab = toTabWidgetFrame(dia->layout()->widget());
	closeTabWidgetFrame(tab);
	mWindows.remove(mWindows.indexOf(dia));
}

void FrostEdit::on_actionUndo_triggered() {
	TextEdit* edit = toTextEdit(mCurrentTabWidget->currentWidget());
	if(edit != nullptr)
		edit->undo();
}

void FrostEdit::on_actionRedo_triggered() {
	TextEdit* edit = toTextEdit(mCurrentTabWidget->currentWidget());
	if(edit != nullptr)
		edit->redo();
}

void FrostEdit::on_actionCopy_triggered() {
	TextEdit* edit = toTextEdit(mCurrentTabWidget->currentWidget());
	if(edit != nullptr)
		edit->copy();
}

void FrostEdit::on_actionCut_triggered() {
	TextEdit* edit = toTextEdit(mCurrentTabWidget->currentWidget());
	if(edit != nullptr)
		edit->cut();
}

void FrostEdit::on_actionPaste_triggered() {
	TextEdit* edit = toTextEdit(mCurrentTabWidget->currentWidget());
	if(edit != nullptr)
		edit->paste();
}

Document* FrostEdit::getActiveDocument() {
	TextEdit* edit = toTextEdit(mCurrentTabWidget->currentWidget());
	if(edit == nullptr)
		return nullptr;
	return toDocument(edit->document());
}

void FrostEdit::on_actionCompileAndRun_triggered() {
	compile();
	mRunToo = true;

}

void FrostEdit::applicationCloseRequest(int id) {
	Console* c = qobject_cast<Console*>(mApplicationOutput->widget(id));
	if(c != nullptr) {
		QProcess* proc = c->getProcess();
		if(proc != nullptr) {
			if(proc->state() == QProcess::Running)
				proc->close();
			mRunningApplication.removeAt(mRunningApplication.indexOf(proc));
			delete proc;
		}
		mApplicationOutput->removeTab(id);
		delete c;
	}
}

void FrostEdit::pointToIssue(QListWidgetItem* item) {
	IssueItem* issue = static_cast<IssueItem*>(item);
	TabWidgetFrame* curTabFrameWidget = toTabWidgetFrame(mCurrentTabWidget->parentWidget());
	TextEdit* edit = nullptr;

	QString path;
	QFileInfo file(mCompiledFile);
	if(file.exists()  && file.isFile()) {
		path = file.absolutePath();
	} else {
		path = QDir::currentPath();
	}
	QString open = path+"/"+issue->getFile();

	addEditor(curTabFrameWidget, open);


	edit = toTextEdit(mCurrentTabWidget->currentWidget());

	edit->setCursorPosition(issue->getRow()-1, issue->getColumn()-1);
	edit->ensureCursorVisible();
	edit->setFocus();

}

void FrostEdit::interpretCompileOut(QString line) {

	auto match = mFrostCompilerErrorRegEx.match(line);
	while(match.hasMatch()) {

		ui->consoleTabs->setCurrentIndex(0);
		QStringList captures = match.capturedTexts();

		QString wholeMsg = captures[0];
		QString file = captures[1];
		int row = captures[2].toInt();
		int col = captures[3].toInt();
		QString type = captures[4];
		int code = captures[5].toInt();
		QString explanation = captures[6];

		if(type.toLower() == "warning")
			mIssueList->addWarning(wholeMsg, file, explanation, row, col);
		else if(type.toLower() == "error")
			mIssueList->addError(wholeMsg, file, explanation, row, col);
		match = mFrostCompilerErrorRegEx.match(line, match.capturedEnd());
	}
}

void FrostEdit::setUpDocumentHiltter(Document* doc) {

	TextEditor::Internal::Highlighter* hilt = new Fate::FateHighlighter();
	//Qate::DefaultColors::ApplyToHighlighter(hilt);
	mSyntaxStyle.applyToHighlighter(hilt);
	QSharedPointer<TextEditor::Internal::HighlightDefinition> ptr = mHiltDefManager->definition("Highlighters/frostbasic.xml");
	if(!ptr.isNull())
		hilt->setDefaultContext(ptr->initialContext());
	else
		Q_ASSERT("Default highlight definition loading failed" && 0);
	doc->setHighlighter(hilt);
}

int FrostEdit::tabWidgetContains(TabWidget* wid, Document* doc) {
	for(int i = 0; i < wid->count(); i++) {
		TextEdit* edit = toTextEdit(wid->widget(i));
		if(edit == nullptr)
			continue;
		if(toDocument(edit->document()) == doc) {
			return i;
		}
	}
	return -1;
}

void FrostEdit::on_actionCompile_triggered() {
	compile();
	mRunToo = false;
}

void FrostEdit::printCompileStandardOutput() {
	QString output(mFrostCompiler->readAllStandardOutput());
	qDebug() << output;
	mCompileOutput->insertStandardOut(output);
}

void FrostEdit::printCompileStandardError() {
	QString output(mFrostCompiler->readAllStandardError());
	qDebug() << output;
	mCompileOutput->insertErrorOut(output);
}

void FrostEdit::printCompile() {
	QString output(mFrostCompiler->readAll());
	qDebug() << output;
	mCompileOutput->insertText(output);
}


void FrostEdit::compile() {
	ui->consoleTabs->setCurrentIndex(ui->consoleTabs->indexOf(mCompileOutput));
	mCompileOutput->clear();
	mIssueList->clear();

	Document* doc = getActiveDocument();
	if(doc == nullptr)
		return;
	QFileInfo file;


	mCompiledFile = doc->getFullPath();
	if(doc->isActualFile()) {
		on_actionSave_triggered();
		file = doc->getFileInfo();
	} else {
		file = doc->saveTemporary();
		mTemporaryFiles.append(file);
	}

	mCompileFile = file;
	ui->actionCompile->setDisabled(true);
	ui->actionCompileAndRun->setDisabled(true);

	QProcessEnvironment procenv;
	QString env = Settings::getDefaultCompilerEnvironment();
	if(!env.isEmpty())
		procenv.insert("PATH", env);

	QString defaultCompilerPath = Settings::getDefaultCompilerPath();
	mFrostCompiler->setProgram(defaultCompilerPath+"CBCompiler.exe");
	mFrostCompiler->setArguments(QStringList() << file.absoluteFilePath());
	mFrostCompiler->setWorkingDirectory(defaultCompilerPath);
	mFrostCompiler->setProcessEnvironment(procenv);
	mFrostCompiler->setProcessChannelMode(QProcess::SeparateChannels);
	mFrostCompiler->start(QProcess::ReadOnly);


}


void FrostEdit::compileFinished(int output, QProcess::ExitStatus status) {


	if(!mTemporaryFiles.isEmpty()) {
		QFile::remove(mTemporaryFiles.last().absoluteFilePath());
		mTemporaryFiles.pop_back();
	}

	/* Are we running the code too? */
	if(mRunToo == true) {
		if(output == 0 && status == QProcess::NormalExit) {
			on_actionRun_triggered();
		}
		mRunToo = false;
	}


	ui->actionCompile->setEnabled(true);
	ui->actionCompileAndRun->setEnabled(true);
}

TextEdit* FrostEdit::toTextEdit(QWidget* wid) {
	return qobject_cast<TextEdit*>(wid);
}

Document* FrostEdit::toDocument(QTextDocument* doc) {
	return qobject_cast<Document*>(doc);
}

TabWidgetFrame* FrostEdit::toTabWidgetFrame(QWidget* wid) {
	return qobject_cast<TabWidgetFrame*>(wid);
}

FrostDialog* FrostEdit::toFrostDialog(QWidget* wid) {
	return qobject_cast<FrostDialog*>(wid);
}

Console*FrostEdit::toConsole(QWidget* wid) {
	return qobject_cast<Console*>(wid);
}

void FrostEdit::updateTabWidget(TabWidget* tabwid) {
	if(mCurrentTabWidget != tabwid && tabwid != nullptr) {
		emit tabWidgetChanged(tabwid);
		if(mCurrentTabWidget != nullptr) mCurrentTabWidget->setActive(false);
		mCurrentTabWidget = tabwid;
		mCurrentTabWidget->setActive(true);
	}
}


void FrostEdit::on_actionRun_triggered() {
	QString defaultCompilerPath = Settings::getDefaultCompilerPath();

	mRunningApplication.append(new QProcess(this));
	mRunningApplication.last()->setWorkingDirectory(mCompileFile.absolutePath());
	mRunningApplication.last()->setProcessChannelMode(QProcess::SeparateChannels);
	Console* console = new Console(this);
	console->setFont(mFont);
	mApplicationOutput->addTab(console, "cbrun.exe");

	mRunningApplication.last()->start(defaultCompilerPath+"cbrun.exe", QProcess::ReadWrite);

	console->hookToProcess(mRunningApplication.last());

	ui->consoleTabs->setCurrentIndex(ui->consoleTabs->indexOf(mApplicationOutput));
	mApplicationOutput->setCurrentIndex(mApplicationOutput->indexOf(console));

}

int FrostEdit::documentSafeClose(Document* doc) {
	if(doc->isModified()) {
		int msg = QMessageBox::warning(this, "Warning", tr("The file %1 is modified, do you want to save the changes?").arg(doc->getFileName()), "Yes", "No", "Cancel");
		switch(msg) {
			case 0:
				if(doc->getFileInfo().isFile()) {
					doc->save();
				} else {
					on_actionSave_As_triggered();
				}
			case 1:
			case 2:
				return msg;
			break;
		}

	}
	return 1;
}

void FrostEdit::loadStyleSheet(const QString& sheet) {
	qDebug() << sheet;
	if(sheet.isEmpty())
		setStyleSheet("");
	QFile style(sheet);
	if(!style.exists())
		return;
	if(style.open(QIODevice::ReadOnly | QIODevice::Text)) {
		setStyleSheet(style.readAll());
		style.close();
	}
}

TextEdit* FrostEdit::currentTextEdit() {
	return toTextEdit(mCurrentTabWidget->currentWidget());
}


void FrostEdit::connectTabWidgetFrameSignals(TabWidgetFrame* tab) {
	connect(tab->tabWidget(), SIGNAL(currentChanged(TabWidget*, int)), this, SLOT(updateDocumentSelection(TabWidget*, int)));
	connect(tab->tabWidget(), SIGNAL(currentChanged(TabWidget*, int)), this, SLOT(changeTitle(TabWidget*, int)));
	connect(tab->tabWidget(), SIGNAL(tabCloseRequested(TabWidget*,int)), this, SLOT(closeTab(TabWidget*, int)));
	connect(tab, SIGNAL(close(TabWidgetFrame*)), this, SLOT(closeTabWidgetFrame(TabWidgetFrame*)));
	connect(tab, SIGNAL(splitHorizontally(TabWidgetFrame*)), this, SLOT(splitHorizontally(TabWidgetFrame*)));
	connect(tab, SIGNAL(splitVertically(TabWidgetFrame*)), this, SLOT(splitVertically(TabWidgetFrame*)));
	connect(tab, SIGNAL(openToNewWindow(TabWidgetFrame*)), this, SLOT(openToNewWindow(TabWidgetFrame*)));
	connect(tab, SIGNAL(itemChanged(TabWidgetFrame*,QString)), this, SLOT(addEditor(TabWidgetFrame*,QString)));
}

void FrostEdit::closeEvent(QCloseEvent* e) {
	e->ignore();

	for(auto& i: mOpenDocuments.keys()) {
		Document* doc = mOpenDocuments[i];
		int id = documentSafeClose(doc);
		if(id == 0 || id == 1)
			removeDocument(doc);
		else if(id == 2) {
			e->ignore();
			return;
		}
	}
	mFindReplace->setHidden(true);
	e->accept();
}

void FrostEdit::on_close_clicked() {

	Console* c = qobject_cast<Console*>(mApplicationOutput->currentWidget());
	if(c != nullptr) {
		QProcess* proc = c->getProcess();
		if(proc != nullptr) {
			if(c->running())
				c->closeProcess();
			mRunningApplication.removeAt(mRunningApplication.indexOf(proc));
			delete proc;
		}
	}
}

void FrostEdit::findFromTextEdit() {
	if(currentTextEdit() != nullptr)
		currentTextEdit()->find();
}

void FrostEdit::findAndReplaceFromTextEdit() {
	if(currentTextEdit() != nullptr)
		currentTextEdit()->findReplace();
}

void FrostEdit::replaceAllFromTextEdit() {
	if(currentTextEdit() != nullptr)
		currentTextEdit()->replaceAll();
}

void FrostEdit::pointOutOccurances(QString str) {
	if(currentTextEdit() != nullptr)
		currentTextEdit()->showOccurances(str);
}

void FrostEdit::on_actionFind_triggered() {
	mFindReplace->show();
}

void FrostEdit::on_actionSettings_triggered() {
	mSettingsMenu->show();
}

void FrostEdit::applySettings() {
	loadStyleSheet(Settings::get("Appearance/StyleSheet", "").toString());
	//mSyntaxStyle.load(Settings::get("Appearance/Colorscheme").toString());
	//mSettingsMenu->setSyntaxStyle(&mSyntaxStyle);
	mFont.setFamily(Settings::get("TextEditor/Font", "Lucida Console").toString());
	mFont.setPointSize(Settings::get("TextEditor/FontSize", 10).toInt());

	for(TabWidgetFrame* tabf: mTabWidgetFrames) {
		TabWidget* wid =tabf->tabWidget();
		for(int i = 0; i < wid->count(); i++) {
			TextEdit* e = toTextEdit(wid->widget(i));
			e->setFont(mFont);
			mSyntaxStyle.applyToTextEdit(e);
		}
	}


	for(auto i: mOpenDocuments.keys()) {
		Document* doc = mOpenDocuments[i];
		TextEditor::Internal::Highlighter* hilt = doc->getHighlighter();

		if(hilt != nullptr) {
			qDebug() << "Found a highlighter!";
			mSyntaxStyle.applyToHighlighter(hilt);
			hilt->rehighlight();
		}
	}

	for(int i = 0; i < mApplicationOutput->count(); i++) {
		Console* c = toConsole(mApplicationOutput->widget(i));
		mSyntaxStyle.applyToConsole(c);
	}

	mSyntaxStyle.applyToConsole(mCompileOutput);
	mSyntaxStyle.applyToIssueList(mIssueList);

}
