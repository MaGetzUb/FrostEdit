#ifndef CODEMODEL_H
#define CODEMODEL_H

#include <QObject>
#include "symbol.hpp"
#include "codepoint.hpp"
class Document;

class CodeModel : public QObject
{
	Q_OBJECT
	public:
		explicit CodeModel(QObject *parent = 0);
		virtual ~CodeModel();

		virtual void parse() = 0;

		void setMainDocument(Document *mainDocument);
		Document *mainDocument() const;

		void addDocument(const QString &filePath, Document *document);

		virtual QList<Symbol*> codeComplete(int position) const = 0;

	signals:
		void requestDocument(const QString &filePath);
		void warning(const QString &msg, const CodePoint &cp);
		void error(const QString &msg, const CodePoint &cp);
	public slots:

		virtual void documentEdited(int position, int charsRemoved, int charsAdded) = 0;
	protected:
		virtual void documentAdded(const QString &filePath, Document *doc) = 0;

		Document *mMainDocument;
		QList<Document*> mDocuments;
};

#endif // CODEMODEL_H
