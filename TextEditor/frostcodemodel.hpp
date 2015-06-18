#ifndef FROSTCODEMODEL_H
#define FROSTCODEMODEL_H
#include <QHash>
#include <QVector>

#include "codemodel.hpp"
#include "frosttoken.hpp"
#include "frostcodemodelcontext.hpp"

namespace Frost {

class FrostCodeModel : public CodeModel
{
	Q_OBJECT


	public:
		FrostCodeModel(const QSharedPointer<CodeModelContext> &context, QObject *parent = 0);
		~FrostCodeModel();

		void parse();

		QList<Symbol*> codeComplete(int position) const;
	public:
		void documentEdited(int position, int charsRemoved, int charsAdded);
	protected:
		void documentAdded(const QString &filePath, Document *doc);
		void deleteAllTokens();

		Token *createToken(Token *insertAfterThis, Token::Type type, const QString &text, QLinkedList<LineStartPoint>::Iterator lineStart, int column);
		Token *findTokenAfterPoint(int i);
		Token *findTokenBeforePoint(int i);

		template <typename CharIterator>
		Token *analyze(CharIterator &it, Token *insertAfterThis, QLinkedList<LineStartPoint>::Iterator &lineStartPoint, int endCharIndex = -1);

		template <typename CharIterator>
		Token *createNumberToken(CharIterator &it, Token *insertAfterThis, QLinkedList<LineStartPoint>::Iterator &lineStartPoint);

		template <typename CharIterator>
		Token *createStringLiteralToken(CharIterator &it, Token *insertAfterThis, QLinkedList<LineStartPoint>::Iterator &lineStartPoint);

		template <typename CharIterator>
		Token *createIdentifierToken(CharIterator &it, Token *insertAfterThis, QLinkedList<LineStartPoint>::Iterator &lineStartPoint);

		template <typename CharIterator>
		Token *createHexToken(CharIterator &it, Token *insertAfterThis, QLinkedList<LineStartPoint>::Iterator &lineStartPoint);

		template <typename CharIterator>
		Token *createSingleLineCommentToken(CharIterator &it, Token *insertAfterThis, QLinkedList<LineStartPoint>::Iterator &lineStartPoint);

		template <typename CharIterator>
		Token *createMultiLineCommentToken(CharIterator &it, Token *insertAfterThis, QLinkedList<LineStartPoint>::Iterator &lineStartPoint);


		Token::Type resolveIdentifierTokenType(const QString &str) const;

		QLinkedList<LineStartPoint> mLineStartPoints;

		Token *mFirstToken;
		Token *mLastToken;
		QSharedPointer<CodeModelContext> mContext;

};


}

#endif // FROSTCODEMODEL_H
