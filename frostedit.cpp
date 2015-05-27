#include "frostedit.hpp"
#include "ui_frostedit.h"
#include <QDebug>

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
QString FrostEdit::gCompilerPath = "D:\\Programming\\Projects\\CBCompiler\\bin\\";


FrostEdit::FrostEdit(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::FrostEdit),
	mFont("Lucida Console"),
	mNewCount(1),
	mFrostCompiler(new QProcess(this)),
	mMimeDatabase(new Qate::MimeDatabase),
	mHiltDefManager(Qate::HighlightDefinitionManager::instance())
{
	mFont.setFixedPitch(true);
	mFont.setPointSize(8);
	mFont.setStyleHint(QFont::Monospace);


	mDocumentWatcher = new QFileSystemWatcher(this);
	connect(mDocumentWatcher, SIGNAL(fileChanged(QString)), this, SLOT(fileChangedOutside(QString)));
	ui->setupUi(this);


	mTabWidgetFrames.append(new TabWidgetFrame(this));
	mTabWidgetFrames.last()->hasCloseAction(false);
	mCurrentTabWidget = mTabWidgetFrames.last()->tabWidget();
	setCentralWidget(mTabWidgetFrames.last());

	connect(ui->openFilesWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addEditor(QListWidgetItem*)));

	for(TabWidgetFrame* tab: mTabWidgetFrames) {
		connect(tab->tabWidget(), SIGNAL(currentChanged(TabWidget*, int)), this, SLOT(updateDocumentSelection(TabWidget*, int)));
		connect(tab->tabWidget(), SIGNAL(currentChanged(TabWidget*, int)), this, SLOT(changeTitle(TabWidget*, int)));
		connect(tab->tabWidget(), SIGNAL(tabCloseRequested(TabWidget*,int)), this, SLOT(closeTab(TabWidget*, int)));
		connect(tab, SIGNAL(close(TabWidgetFrame*)), this, SLOT(closeTabWidgetFrame(TabWidgetFrame*)));
		connect(tab, SIGNAL(splitHorizontally(TabWidgetFrame*)), this, SLOT(splitHorizontally(TabWidgetFrame*)));
		connect(tab, SIGNAL(splitVertically(TabWidgetFrame*)), this, SLOT(splitVertically(TabWidgetFrame*)));
		connect(tab, SIGNAL(openToNewWindow(TabWidgetFrame*)), this, SLOT(openToNewWindow(TabWidgetFrame*)));
		connect(tab, SIGNAL(itemChanged(TabWidgetFrame*,QString)), this, SLOT(addEditor(TabWidgetFrame*,QString)));
	}

	connect(this, SIGNAL(tabWidgetChanged(TabWidget*)), this, SLOT(setActiveTabWidget(TabWidget*)));
	currentTabPageChanged(0);
	connect(mCurrentTabWidget, SIGNAL(currentChanged(int)), this, SLOT(currentTabPageChanged(int)));

	connect(mFrostCompiler, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(compileFinished(int, QProcess::ExitStatus)));


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

	//QFile style("Stylesheets/stylesheet.css");
	//if(style.open(QIODevice::ReadOnly | QIODevice::Text)) {
	//	setStyleSheet(style.readAll());
	//	style.close();
	//}



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
	connect(mIssueList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(pointIssueOut(QListWidgetItem*)));



	mCompileOutput->show();
	mIssueList->show();
}

FrostEdit::~FrostEdit() {
	for(QMap<QString, Document*>::iterator i = mOpenDocuments.begin(); i != mOpenDocuments.end(); i++) {
		mDocumentWatcher->removePath(i.key());
		delete i.value();
	}
	mOpenDocuments.clear();

	delete mCompileOutput;
	for(int i = 0; i < mApplicationOutput->count(); i++) {
		Console* c = qobject_cast<Console*>(mApplicationOutput->widget(i));
		mApplicationOutput->removeTab(i);
		delete c;
	}

	delete mDocumentWatcher;
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
			if(edit->document() == doc) {
				tab->tabWidget()->setTabText(i, doc->getDynamicName());
				changeTitle(tab->tabWidget(), i);
			}
		}
	}
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
	edit->setFont(mFont);
	mCurrentTabWidget->addTab(edit, doc->getDynamicName());
	mCurrentTabWidget->setCurrentIndex(mCurrentTabWidget->count()-1);
}

void FrostEdit::addEditor(const QString& path) {
	for(TabWidgetFrame* tabwidget: mTabWidgetFrames) {
		Document* doc = mOpenDocuments[path];
		TextEdit* edit = new TextEdit(tabwidget->tabWidget(), doc);
		edit->setFont(mFont);
		tabwidget->tabWidget()->addTab(edit, doc->getDynamicName());
	}
}

void FrostEdit::addEditor(TabWidgetFrame* wid, const QString& str) {
	Document* doc = nullptr ;
	if(mOpenDocuments.contains(str) == false) {
		if(QFileInfo(str).isFile()) {
			addDocument(str);
			doc = mOpenDocuments[str];
		}
	} else
		doc = mOpenDocuments[str];



	if(doc == nullptr)
		return;

	for(int i = 0; i < wid->tabWidget()->count(); i++) {
		TextEdit* edit = toTextEdit(wid->tabWidget()->widget(i));
		if(edit != nullptr)
			if(toDocument(edit->document()) == doc) {
				wid->tabWidget()->setCurrentIndex(i);
				return;
			}
	}

	TextEdit* edit = new TextEdit(wid->tabWidget(), doc);
	edit->setFont(mFont);
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
	auto find = ui->openFilesWidget->findItems(doc->getFileName(), 0);
	for(auto& i: find) {
		DocumentItem* item = static_cast<DocumentItem*>(i);
		if(item->getDocument() == doc)
			delete item;
	}


	delete doc;
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
		return;
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

	auto items = ui->openFilesWidget->findItems(doc->getFileName(), Qt::MatchExactly);
	if(!items.isEmpty()) {
		for(auto& item: items) {
			if(item->data(0).toString() == doc->getFullPath()) {
				item->setSelected(true);
				break;
			}
		}
	}

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

	if(editors == 1) {
		if(doc->isModified()) {
			int msg = QMessageBox::warning(this, "Warning", "The file is modified, do you want to save the changes?", "Yes", "No", "Cancel");
			switch(msg) {
				case 0:
					if(doc->getFileInfo().isFile()) {
						doc->save();
					} else {
						on_actionSave_As_triggered();
					}
				case 1:
				break;
				case 2:
					return;
				break;
			}

		}
	}

	wid->removeTab(id);
	delete e;
	removeDocument(doc);


}

void FrostEdit::currentTabPageChanged(int id) {
	TextEdit* e = toTextEdit(mCurrentTabWidget->widget(id));
	if(e != nullptr &&  e->document() != getActiveDocument())
		emit documentChanged(toDocument(e->document()));

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

void FrostEdit::addDocument(const QString& path) {

	if(mOpenDocuments.contains(path))
		return;

	Document* doc = new Document(this, path);
	mOpenDocuments.insert(path, doc);
	connect(doc, SIGNAL(textChanged(Document*,bool)), this, SLOT(updateTabHeader(Document*, bool)));
	setUpDocumentHiltter(doc);


	addDocumentItem(doc);

	mDocumentWatcher->addPath(path);

	for(TabWidgetFrame* tab: mTabWidgetFrames) {
		tab->addComboBoxItem(path);
	}

}

void FrostEdit::addDocumentItem(Document* doc) {
	DocumentItem* item = new DocumentItem(ui->openFilesWidget, doc);
	ui->openFilesWidget->addItem(item);
}

void FrostEdit::on_actionSave_As_triggered() {
	QString file = QFileDialog::getSaveFileName(this, "Save as", "", "FrostBasic code files (*.frb);;CoolBasic code files (*.cb);;Text files (.txt);;All files (*.*)");
	if(!file.isEmpty()) {
		TextEdit* e = toTextEdit(mCurrentTabWidget->currentWidget());
		Document* doc = toDocument(e->document());
		mDocumentWatcher->removePath(doc->getFullPath());
		auto list = ui->openFilesWidget->findItems(doc->getFileName(), Qt::MatchExactly);

		DocumentItem* docItem;
		for(auto& i: list) {
			DocumentItem* docy = static_cast<DocumentItem*>(i);
			if(docy->getDocument() == doc) {
				docItem = docy;
				break;
			}
		}



		for(TabWidgetFrame* tab: mTabWidgetFrames) {
			tab->removeComboBoxItem(doc->getFullPath());
			tab->addComboBoxItem(file);
		}

		doc->saveAs(file);
		mDocumentWatcher->addPath(file);
		updateTabHeader(doc, doc->isModified());
		docItem->update();
	}
}


//TODO: a document item.
void FrostEdit::on_closeDocument_clicked() {
	DocumentItem* item = static_cast<DocumentItem*>(ui->openFilesWidget->currentItem());
	Document* doc = item->getDocument();
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
		if(mCurrentTabWidget != tabwid) {
			emit tabWidgetChanged(tabwid);
			if(mCurrentTabWidget != nullptr) mCurrentTabWidget->setActive(false);
		}
		mCurrentTabWidget = tabwid;
		mCurrentTabWidget->setActive(true);

	//Was TextEdit
	} else if(wid != nullptr && tabwid == nullptr && bar == nullptr) {
		tabwid = qobject_cast<TabWidget*>(wid->getParentWidget());
		qDebug() << wid;
		if(mCurrentTabWidget != tabwid) {
			emit tabWidgetChanged(tabwid);
			if(mCurrentTabWidget != nullptr) mCurrentTabWidget->setActive(false);
		}
		mCurrentTabWidget = tabwid;
		mCurrentTabWidget->setActive(true);

	//Was TabWidget
	} else if(tabwid != nullptr && bar == nullptr) {
		if(mCurrentTabWidget != tabwid) {
			emit tabWidgetChanged(tabwid);
			if(mCurrentTabWidget != nullptr) mCurrentTabWidget->setActive(false);
		}
		mCurrentTabWidget = tabwid;
		mCurrentTabWidget->setActive(true);
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
	Document* doc = new Document();
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



	connect(mTabWidgetFrames.last()->tabWidget(), SIGNAL(currentChanged(TabWidget*, int)), this, SLOT(updateDocumentSelection(TabWidget*, int)));
	connect(mTabWidgetFrames.last()->tabWidget(), SIGNAL(currentChanged(TabWidget*, int)), this, SLOT(changeTitle(TabWidget*, int)));
	connect(mTabWidgetFrames.last()->tabWidget(), SIGNAL(tabCloseRequested(TabWidget*,int)), this, SLOT(closeTab(TabWidget*, int)));
	connect(mTabWidgetFrames.last(), SIGNAL(close(TabWidgetFrame*)), this, SLOT(closeTabWidgetFrame(TabWidgetFrame*)));
	connect(mTabWidgetFrames.last(), SIGNAL(splitHorizontally(TabWidgetFrame*)), this, SLOT(splitHorizontally(TabWidgetFrame*)));
	connect(mTabWidgetFrames.last(), SIGNAL(splitVertically(TabWidgetFrame*)), this, SLOT(splitVertically(TabWidgetFrame*)));
	connect(mTabWidgetFrames.last(), SIGNAL(openToNewWindow(TabWidgetFrame*)), this, SLOT(openToNewWindow(TabWidgetFrame*)));
	connect(mTabWidgetFrames.last(), SIGNAL(itemChanged(TabWidgetFrame*,QString)), this, SLOT(addEditor(TabWidgetFrame*,QString)));

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

		mTabWidgetFrames.removeOne(tabWidFrame);
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

void FrostEdit::pointIssueOut(QListWidgetItem* item) {
	IssueItem* issue = static_cast<IssueItem*>(item);
	TabWidgetFrame* curTabFrameWidget = toTabWidgetFrame(mCurrentTabWidget->parentWidget());
	TextEdit* edit = nullptr;

	QString file = issue->getFile();
	QFileInfo compiledFile(mCompiledFile);
	QString path;
	if(compiledFile.isFile()) {
		path = compiledFile.path();
	} else {
		path = QDir::currentPath();
	}

	if(!QFileInfo(file).isFile()) {
		file = path+"/"+file;
	}

	if(QFileInfo(file).isFile())
		addDocument(file);
	qDebug() << file;

	if(mOpenDocuments.contains(file) == true) {
		addEditor(curTabFrameWidget, file);
		edit = toTextEdit(mCurrentTabWidget->currentWidget());
	} else if(mOpenDocuments.contains(issue->getFile()) == false) {
		addEditor(curTabFrameWidget, mCompiledFile);
		edit = toTextEdit(mCurrentTabWidget->currentWidget());
	} else {
		addEditor(curTabFrameWidget, issue->getFile());
		edit = toTextEdit(mCurrentTabWidget->currentWidget());
	}
	if(edit != nullptr)
		edit->setCursorPosition(issue->getRow()-1, issue->getColumn()-1);
}

void FrostEdit::interpretCompileOut(QString line) {
	//"C:/Ohjelmointi/FrostEditor/TestCodes/stars.frb" [141, 7] Warning 3: The variable name after "Next" is ignored

	QRegularExpression expression("\\\"(.*)\\\"\\s+\\[\\s*(\\d+)\\,\\s*(\\d*)\\s*\\]\\s+(Warning|Error)\\s+(\\d+)\\:\\s+(.*)"); //\\s*(\\d+)\\:\\s*(.*)
	auto match = expression.match(line);
	while(match.hasMatch()) {
		//qDebug() << "Match! " <<match.capturedTexts();
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
		match = expression.match(line, match.capturedEnd());
	}
}

void FrostEdit::setUpDocumentHiltter(Document* doc) {

	TextEditor::Internal::Highlighter* hilt = new Fate::Highlighter();
	Qate::DefaultColors::ApplyToHighlighter(hilt);
	QSharedPointer<TextEditor::Internal::HighlightDefinition> ptr = mHiltDefManager->definition("Highlighters/frostbasic.xml");
	if(!ptr.isNull())
		hilt->setDefaultContext(ptr->initialContext());
	else
		Q_ASSERT("Default highlight definition loading failed" && 0);
	doc->setHighlighter(hilt);
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
	//procenv.insert("PATH", QString("C:\\llvm-3.5-bin-mingw\\bin;C:\\Qt\\5.4\\mingw491_32\\bin;"));
	mFrostCompiler->setProgram(gCompilerPath+"CBCompiler.exe");
	mFrostCompiler->setArguments(QStringList() << file.absoluteFilePath());
	mFrostCompiler->setWorkingDirectory(gCompilerPath);
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
			mRunningApplication.append(new QProcess(this));
			mRunningApplication.last()->setWorkingDirectory(mCompileFile.absolutePath());
			mRunningApplication.last()->setProcessChannelMode(QProcess::SeparateChannels);
			//mRunningApplication.last()->setProgram(gCompilerPath+"cbrun.exe");
			//mRunningApplication.last()->setProgram("cmd");
			Console* console = new Console(this);
			console->setFont(mFont);
			mApplicationOutput->addTab(console, "cbrun.exe");

			mRunningApplication.last()->start(gCompilerPath+"cbrun.exe", QProcess::ReadWrite);

			console->hookToProcess(mRunningApplication.last());

			ui->consoleTabs->setCurrentIndex(ui->consoleTabs->indexOf(mApplicationOutput));
			mApplicationOutput->setCurrentIndex(mApplicationOutput->indexOf(console));
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


