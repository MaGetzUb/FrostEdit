#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include <QTextDocument>
#include <QFileInfo>
#include <QPlainTextDocumentLayout>
#include <QTimer>
#include <QMultiMap>
#include <QRegularExpression>

#include "qate/highlighter.h"
#include "qate/tabsettings.h"
#include "fatehighlighter.hpp"
#include "fatedata.hpp"


class FrostEdit;
class DocumentItem;
class Document : public QTextDocument {
	Q_OBJECT
	friend class Fate::FateHighlighter;

	QFileInfo mFileInfo;
	QString mFile;
	QTimer mSafeCopyTimer;
	TextEditor::Internal::Highlighter* mHighlighter;
	TextEditor::TabSettings mTabSettings;
	DocumentItem* mItem;

	public:


		Document(QObject* parent = 0, const QString file = "", DocumentItem* item = 0);
		~Document();


		const QFileInfo& getFileInfo() const;
		const QString getFullPath() const;
		const QString getFilePath() const;
		const QString getFileName() const;
		const QString getDynamicName() const;

		void clearIdentifiers();
		void setItem(DocumentItem*);
		DocumentItem* getItem() const;

		bool isActualFile() const;
		void setFile(const QString& file);
		void setHighlighter(TextEditor::Internal::Highlighter* hiltter);
		bool hasHighlighter() const;
		void setTabSettings(const TextEditor::TabSettings& tabsettings);
		const TextEditor::TabSettings& getTabSettings() const;
		TextEditor::Internal::Highlighter* getHighlighter() const;


		FrostEdit* getEditor() const;


	public slots:

		void reload();
		void save();
		void saveAs(const QString& str);
		QFileInfo saveTemporary();
		void lexicalAnalyze();
		void parse();

	private slots:
		void emitTextChanged(bool b);


	signals:
		void pathChanged(QString);
		void textChanged(Document*, bool b);
		void highlighterChanged(Document*, TextEditor::Internal::Highlighter*);
};

#endif // DOCUMENT_HPP
