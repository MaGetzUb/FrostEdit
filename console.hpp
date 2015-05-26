#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <QTextEdit>
#include <QPlainTextEdit>
#include <QTextBlock>
#include <QProcess>

class Console : public QPlainTextEdit {
		Q_OBJECT

		QTextLine getLineUnderCursor(const QTextCursor &);
		QProcess* mProcess;
		bool mDisconnectSignals;
		bool mRunning;
	public:
		explicit Console(QWidget *parent = 0);
		~Console();
		void hookToProcess(QProcess* proc);
	signals:

		void doubleClickedLine(QString);
		void stdOutAdded(QString out);
		void stdErrorAdded(QString err);
		void textAdded(QString txt);


	public slots:
		void insertText(const QString& str, const QBrush& brsh = QBrush(Qt::black));
		void insertText(const QString& str, const QTextCharFormat&);

		void processFinished(int, QProcess::ExitStatus);
		void closeProcess();
		void processStarted();
		QProcess* getProcess();

		void disconnectSignalsAfterFinishing(bool);
		void insertStandardOut(const QString& str);
		void insertErrorOut(const QString& str);
		void mouseDoubleClickEvent(QMouseEvent* e);
	private slots:
		void readAll();
		void readAllStandardOut();
		void readAllStandardError();
		void kill();
};

#endif // CONSOLE_HPP
