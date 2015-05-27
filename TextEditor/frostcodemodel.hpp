#ifndef FROSTCODEMODEL_H
#define FROSTCODEMODEL_H
#include "codemodel.hpp"
#include <QHash>
#include "frosttoken.hpp"
namespace Frost {

class FrostCodeModel : public CodeModel
{
	Q_OBJECT


	public:
		FrostCodeModel();
		~FrostCodeModel();

		void parse();

		QList<Symbol*> codeComplete(int position) const;
	public:
		void documentEdited(int position, int charsRemoved, int charsAdded);

	protected:
		void documentAdded(const QString &filePath, Document *doc);

		QHash<QString, Token::Type> mKeywords;
		QHash<Document*, Token*> mDocumentStartTokens;
		Token *mFirstToken;
		Token *mLastToken;
};

}

#endif // FROSTCODEMODEL_H
