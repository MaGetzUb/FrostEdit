#include "console.hpp"
#include <QDebug>
#include <QTime>

QTextLine Console::getLineUnderCursor(const QTextCursor &cursor) {

	const QTextBlock block = cursor.block();
	if (!block.isValid())
		return QTextLine();

	const QTextLayout *layout = block.layout();
	if (!layout)
		return QTextLine();

	const int relativePos = cursor.position() - block.position();
	return layout->lineForTextPosition(relativePos);
}

Console::Console(QWidget *parent) :
	QPlainTextEdit(parent)
{
	setReadOnly(true);

	mStdErrFmt.setForeground(QColor(255, 0, 0));
	mStdOutFmt.setForeground(QColor(0, 96, 192));
	setLineWrapMode(NoWrap);
}

Console::~Console() {
	if(running())
		closeProcess();
}

void Console::hookToProcess(QProcess* proc) {
	if(proc != nullptr)
		mProcess = proc;
	//connect(mProcess, SIGNAL(readyRead()), this, SLOT(readAll()));
	connect(mProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readAllStandardOut()));
	connect(mProcess, SIGNAL(readyReadStandardError()), this, SLOT(readAllStandardError()));
	connect(mProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
	connect(mProcess, SIGNAL(started()), this, SLOT(processStarted()));
	qDebug() << "Hooked to qprocess!";
}

void Console::insertText(const QString& str) {
	QTextCursor cursor = textCursor();
	cursor.insertText(str+tr("\n"));
	setTextCursor(cursor);
}

void Console::insertText(const QString& str, const QBrush& brsh) {
	QTextCursor cursor = textCursor();
	QTextCharFormat oldfmt = cursor.charFormat();
	QTextCharFormat fmt;
	fmt.setForeground(brsh);
	cursor.insertText(str+tr("\n"), fmt);
	cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
	cursor.setCharFormat(oldfmt);

	setTextCursor(cursor);
}

void Console::insertText(const QString& str, const QTextCharFormat& fmt) {
	QTextCursor cursor = textCursor();
	QTextCharFormat oldfmt = cursor.charFormat();
	cursor.insertText(str, fmt);
	cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
	cursor.setCharFormat(oldfmt);
	setTextCursor(cursor);
}

void Console::processFinished(int err, QProcess::ExitStatus status) {

	QString insertion = QTime::currentTime().toString("hh:mm:ss");
	if(status == 1) {
		insertText(tr("<%1> %2 crashed!").arg(insertion).arg(mProcess->program()));
	} else {
		insertText(tr("<%1> %2 finished succesfully! With exit code: %3").arg(insertion).arg(mProcess->program()).arg(err));
	}

	if(mDisconnectSignals) {
		//disconnect(mProcess, SIGNAL(readyRead()), this, SLOT(readAll()));
		disconnect(mProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readAllStandardOut()));
		disconnect(mProcess, SIGNAL(readyReadStandardError()), this, SLOT(readAllStandardError()));
		disconnect(mProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
		mProcess = nullptr;
	}

	mRunning = false;
}

void Console::closeProcess() {
	if(mRunning)
		mProcess->close();
}

void Console::processStarted() {
	mRunning = true;
}

bool Console::running() {
	return mProcess->state() == QProcess::Running;
}

QProcess* Console::getProcess() {
	return mProcess;
}

void Console::setStdErrFormat(const QTextCharFormat& fmt) {
	mStdErrFmt = fmt;
}

void Console::setStdOutFormat(const QTextCharFormat& fmt) {
	mStdOutFmt = fmt;
}

void Console::disconnectSignalsAfterFinishing(bool b) {
	mDisconnectSignals = b;
}

void Console::readAll() {
	QString str = mProcess->readAll();
	insertText(str);
	emit textAdded(str);
}

void Console::readAllStandardOut() {
	QString str = mProcess->readAllStandardOutput();
	insertStandardOut(str);
	emit stdOutAdded(str);
}

void Console::readAllStandardError() {
	QString str = mProcess->readAllStandardError();
	insertStandardOut(str);
	emit stdErrorAdded(str);
}


void Console::insertStandardOut(const QString &str) {
	QTextCursor cursor = textCursor();
	QTextCharFormat oldfmt = cursor.charFormat();
	QTextCharFormat fmt;



	QStringList list = str.split("\n");
	for(auto& s: list) {
		if(s.isEmpty())
			continue;
		QString insertion = QTime::currentTime().toString("hh:mm:ss");
		cursor.insertText(tr("<%1> %2\n").arg(insertion).arg(s), mStdOutFmt);
	}
	cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
	cursor.setCharFormat(oldfmt);

	setTextCursor(cursor);
}

void Console::insertErrorOut(const QString &str) {
	QTextCursor cursor = textCursor();
	QTextCharFormat oldfmt = cursor.charFormat();
	QTextCharFormat fmt;
	QStringList list = str.split("\n");
	for(auto& s: list) {
		if(s.isEmpty())
			continue;
		QString insertion = QTime::currentTime().toString("hh:mm:ss");
		cursor.insertText(tr("<%1> %2\n").arg(insertion).arg(s), mStdErrFmt);
	}
	cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
	cursor.setCharFormat(oldfmt);

	setTextCursor(cursor);
}

void Console::mouseDoubleClickEvent(QMouseEvent* e) {
	if(e->button() == Qt::MouseButton::LeftButton) {
		QTextLine line = getLineUnderCursor(textCursor());
		QString text = this->toPlainText().mid(line.textStart(), line.textLength()-1);
		emit doubleClickedLine(text);
	}
}
