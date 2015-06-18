#include "frostcodemodel.hpp"
#include "document.hpp"

using namespace Frost;

class CharacterAtCharIterator {
	public:
		CharacterAtCharIterator(const Document *doc, int charIndex, int column, int line) : mDocument(doc), mCharIndex(charIndex), mColumn(column), mLine(line) {}

		CharacterAtCharIterator &operator=(const CharacterAtCharIterator &o) = default;

		int line() const { return mLine; }
		int column() const { return mColumn; }
		int charIndex() const { return mCharIndex; }

		QChar value() const { return mDocument->characterAt(mCharIndex); }

		bool isEndOfLine() const { return value() == '\n'; }
		bool isEndOfFile() const { return mCharIndex >= mDocument->characterCount(); }

		CharacterAtCharIterator next() const {
			if (isEndOfLine()) {
				return CharacterAtCharIterator(mDocument, mCharIndex + 1, 1, mLine + 1);
			}
			else {
				return CharacterAtCharIterator(mDocument, mCharIndex + 1, mColumn + 1, mLine);
			}

		}

	private:
		const Document *mDocument;
		int mCharIndex;
		int mColumn;
		int mLine;
};

class StringRefCharIterator {
	public:
		StringRefCharIterator(const QString &code, int charIndex, int column, int line) : mCode(&code), mCharIndex(charIndex), mColumn(column), mLine(line) {}

		StringRefCharIterator &operator=(const StringRefCharIterator &o) = default;

		int line() const { return mLine; }
		int column() const { return mColumn; }
		int charIndex() const { return mCharIndex; }

		QChar value() const { return mCode->at(mCharIndex); }

		bool isEndOfLine() const { return value() == '\n'; }
		bool isEndOfFile() const { return mCharIndex >= mCode->size(); }

		StringRefCharIterator next() const {
			if (isEndOfLine()) {
				return StringRefCharIterator(*mCode, mCharIndex + 1, 1, mLine + 1);
			}
			else {
				return StringRefCharIterator(*mCode, mCharIndex + 1, mColumn + 1, mLine);
			}

		}

	private:
		const QString *mCode;
		int mCharIndex;
		int mColumn;
		int mLine;
};


FrostCodeModel::FrostCodeModel(const QSharedPointer<CodeModelContext> &context, QObject *parent) :
	CodeModel(parent),
	mFirstToken(0),
	mLastToken(0),
	mContext(context)
{

}

FrostCodeModel::~FrostCodeModel()
{
	deleteAllTokens();
}

void FrostCodeModel::parse()
{
	deleteAllTokens();
	mLineStartPoints.clear();

	mLineStartPoints = QLinkedList<LineStartPoint>() << LineStartPoint(1, 0, mMainDocument);
	QLinkedList<LineStartPoint>::Iterator lineStartPoint = mLineStartPoints.begin();

	QString code = mMainDocument->toPlainText();

	StringRefCharIterator it(code, 0, 1, 1);

	analyze(it, 0, lineStartPoint);
}

QList<Symbol *> FrostCodeModel::codeComplete(int position) const
{
	return QList<Symbol*>();
}

void FrostCodeModel::documentEdited(int position, int charsRemoved, int charsAdded) {
	Document *doc = qobject_cast<Document*>(sender());

	if (doc != mMainDocument) return;
	if (!mFirstToken) {
		parse();
		return;
	}

	Token *tok = mFirstToken;
	while (tok && tok->codePoint().charIndex() < position) {
		tok = tok->next();
	}

	Token *firstNewToken = 0, *lastNewToken = 0, *firstRemovedToken = 0, *lastRemovedToken = 0;
	if (!tok) {
		//Added characters to the end of the document (and maybe removed some withspaces)
		QLinkedList<LineStartPoint>::Iterator lineStartIt = --mLineStartPoints.end();
		CharacterAtCharIterator charIt(mMainDocument, position, position - lineStartIt->mCharIndex + 1, lineStartIt->mLineNumber);

		Token *oldLastToken = mLastToken;
		analyze(charIt, mLastToken, lineStartIt);
		firstNewToken = oldLastToken->next();
		lastNewToken = mLastToken;
	}
	else {
		Token *firstPossibleToken = tok->previous();
		if (firstPossibleToken->codePoint().charIndex() + firstPossibleToken->length() >= position) {
			firstRemovedToken = firstPossibleToken;
		}
		else if ( tok->codePoint().charIndex() < position + charsRemoved){
			firstRemovedToken = tok;
		}
		else {

		}


		lastRemovedToken = firstRemovedToken;
		while (lastRemovedToken && lastRemovedToken->codePoint().charIndex() < position + charsRemoved) {
			lastRemovedToken = lastRemovedToken->next();
		}
		if (lastRemovedToken && lastRemovedToken != firstRemovedToken) lastRemovedToken = lastRemovedToken->previous();

		Token *lastNotRemoved = firstRemovedToken->previous();
		Token *firstNotRemoved = lastRemovedToken->next();

		if (lastNotRemoved) {
			lastNotRemoved->setNext(firstNotRemoved);
			if (firstNotRemoved == 0) mFirstToken = lastNotRemoved;
		}
		firstRemovedToken->setPrevious(0);

		if (firstNotRemoved) {
			firstNotRemoved->setPrevious(lastNotRemoved);
			if (lastNotRemoved == 0) mLastToken = lastNotRemoved;
		}
		lastRemovedToken->setNext(0);


	}

}

void FrostCodeModel::documentAdded(const QString &filePath, Document *doc)
{

}

void FrostCodeModel::deleteAllTokens() {
	Token *tok = mFirstToken;
	if (tok) {
		Token *nextTok = tok->next();
		delete tok;
		while (nextTok) {
			tok = nextTok;
			nextTok = tok->next();
			delete tok;
		}
	}
	mFirstToken = 0;
	mLastToken = 0;
}

Token *FrostCodeModel::createToken(Token *insertAfterThis, Token::Type type, const QString &text, QLinkedList<LineStartPoint>::Iterator lineStart, int column) {
	Token *newToken = new Token(type, text, CodePoint(column, lineStart), insertAfterThis);
	if (!newToken->previous()) {
		mFirstToken = newToken;
	}
	if (!newToken->next()) {
		mLastToken = newToken;
	}
	if (!lineStart->mNextToken) {
		lineStart->mNextToken = newToken;
	}
	else if (lineStart->mNextToken->codePoint().column() > column) {
		lineStart->mNextToken = newToken;
	}

	return newToken;
}

Token *FrostCodeModel::findTokenAfterPoint(int i)
{

}

Token *FrostCodeModel::findTokenBeforePoint(int i)
{

}

Token::Type FrostCodeModel::resolveIdentifierTokenType(const QString &str) const {
	Token::Type type = mContext->keyword(str);
	if (type != Token::KeywordsEnd) return type;
	return Token::Identifier;
}


static bool isHexChar(QChar c) {
	QChar lower = c.toLower();
	return c.isDigit() || lower >= 'a' && lower <= 'f';
}


template <typename CharIterator>
Token *FrostCodeModel::analyze(CharIterator &it, Token *insertAfterThis, QLinkedList<LineStartPoint>::Iterator &lineStartPoint, int endCharIndex) {

	while(!it.isEndOfFile() && (endCharIndex > -1 && it.charIndex() <= endCharIndex)) {
		QChar c = it.value();
		if (c.isSpace() || c == '\t') { }
		else if (c == '=') {
			if (!it.next().isEndOfFile()) {
				if (it.next().value() == '<') {
					insertAfterThis = createToken(insertAfterThis, Token::opLessEqual, "=<", lineStartPoint, it.column());
					it = it.next().next();
					continue;
				}
				else if (it.next().value() == '>') {
					insertAfterThis = createToken(insertAfterThis, Token::opGreaterEqual, "=>", lineStartPoint, it.column());
					it = it.next().next();
					continue;
				}
			}
			insertAfterThis = createToken(insertAfterThis, Token::opEqual, "=", lineStartPoint, it.column());
		}
		else if (c == '+') {
			insertAfterThis = createToken(insertAfterThis, Token::opPlus, "+", lineStartPoint, it.column());
		}
		else if (c == '-') {
			insertAfterThis = createToken(insertAfterThis, Token::opMinus, "-", lineStartPoint, it.column());
		}
		else if (c == '*') {
			insertAfterThis = createToken(insertAfterThis, Token::opMultiply, "*", lineStartPoint, it.column());
		}
		else if (c == '/') {
			insertAfterThis = createToken(insertAfterThis, Token::opDivide, "/", lineStartPoint, it.column());
		}
		else if (c == '^') {
			insertAfterThis = createToken(insertAfterThis, Token::opPower, "^", lineStartPoint, it.column());
		}
		else if (c == '(') {
			insertAfterThis = createToken(insertAfterThis, Token::LeftParenthese, "(", lineStartPoint, it.column());
		}
		else if (c == ')') {
			insertAfterThis = createToken(insertAfterThis, Token::RightParenthese, ")", lineStartPoint, it.column());
		}
		else if (c == '[') {
			insertAfterThis = createToken(insertAfterThis, Token::LeftSquareBracket, "[", lineStartPoint, it.column());
		}
		else if (c == ']') {
			insertAfterThis = createToken(insertAfterThis, Token::RightSquareBracket, "]", lineStartPoint, it.column());
		}
		else if (c == ',') {
			insertAfterThis = createToken(insertAfterThis, Token::Comma, ",", lineStartPoint, it.column());
		}
		else if (c == ':') {
			insertAfterThis = createToken(insertAfterThis, Token::Colon, ":", lineStartPoint, it.column());
		}
		else if (c == '\'') {
			insertAfterThis = createSingleLineCommentToken(it, insertAfterThis, lineStartPoint);
		}
		else if (c == '$') {
			if (!it.next().isEndOfFile() && isHexChar(it.next().value())) {
				insertAfterThis = createHexToken(it, insertAfterThis, lineStartPoint);
			}
			else {
				insertAfterThis = createToken(insertAfterThis, Token::StringTypeMark, "$", lineStartPoint, it.column());
			}
		}
		else if (c == '#') {
			insertAfterThis = createToken(insertAfterThis, Token::FloatTypeMark, "#", lineStartPoint, it.column());
		}
		else if (c == '%') {
			insertAfterThis = createToken(insertAfterThis, Token::IntegerTypeMark, "%", lineStartPoint, it.column());
		}
		else if (c == '<') {
			if (!it.next().isEndOfFile()) {
				if (it.next().value() == '=') {
					insertAfterThis = createToken(insertAfterThis, Token::opLessEqual, "<=", lineStartPoint, it.column());
					it = it.next().next();
					continue;
				}
				else if (it.next().value() == '>') {
					insertAfterThis = createToken(insertAfterThis, Token::opNotEqual, "<>", lineStartPoint, it.column());
					it = it.next().next();
					continue;
				}
			}
			insertAfterThis = createToken(insertAfterThis, Token::opLess, "<", lineStartPoint, it.column());
		}
		else if (c == '>') {
			if (!it.next().isEndOfFile()) {
				if (it.next().value() == '=') {
					insertAfterThis = createToken(insertAfterThis, Token::opGreaterEqual, ">=", lineStartPoint, it.column());
					it = it.next().next();
					continue;
				}
			}
			insertAfterThis = createToken(insertAfterThis, Token::opLess, "<", lineStartPoint, it.column());
		}
		else if (c == '\n') {
			insertAfterThis = createToken(insertAfterThis, Token::EOL, "\n", lineStartPoint, it.column());
			mLineStartPoints.append(LineStartPoint(it.line() + 1, it.charIndex() + 1, lineStartPoint->mDocument));
			++lineStartPoint;
		}
		else if (c.isDigit() || (c == '.' && !it.next().isEndOfFile() && it.next().value().isDigit())) {
			insertAfterThis = createNumberToken(it, insertAfterThis, lineStartPoint);
		}
		else if (c.isLetter() || c == '_') {
			insertAfterThis = createIdentifierToken(it, insertAfterThis, lineStartPoint);
		}
		else if (c == '.') {
			insertAfterThis = createToken(insertAfterThis, Token::opDot, ".", lineStartPoint, it.column());
		}
		else {
			emit warning(tr("Unhandled character %1").arg(c), CodePoint(it.column(), lineStartPoint));
		}
		it = it.next();
	}
	return insertAfterThis;
}

template <typename CharIterator>
Token *FrostCodeModel::createNumberToken(CharIterator &it, Token *insertAfterThis, QLinkedList<LineStartPoint>::Iterator &lineStartPoint) {

	int startColumn = it.column();
	QChar c = it.value();
	bool isFloat = false;
	if (c == '.') isFloat = true;
	QString num(c);
	it = it.next();
	while (!it.isEndOfFile()) {
		c = it.value();
		if (c.isDigit()) {
			num += c;
		}
		else if (c == '.') {
			isFloat = true;
			num += c;
		}
		else if (isFloat && c.toLower() == 'e') {
			num += c;
		}
		else {
			break;
		}
		it = it.next();
	}
	if (isFloat) {
		return createToken(insertAfterThis, Token::Float, num, lineStartPoint, startColumn);
	}
	else {
		return createToken(insertAfterThis, Token::Integer, num, lineStartPoint, startColumn);
	}
}


template <typename CharIterator>
Token *FrostCodeModel::createStringLiteralToken(CharIterator &it, Token *insertAfterThis, QLinkedList<LineStartPoint>::Iterator &lineStartPoint) {

	QLinkedList<LineStartPoint>::Iterator strLitStartLine = lineStartPoint;
	CharIterator start = it;
	QString text(it.value());
	Q_ASSERT(it.value() == '"');

	CharIterator lastIt = it;
	it = it.next();



	while (!it.isEndOfFile() && it != '"') {
		if (it == '\n') {
			mLineStartPoints.append(LineStartPoint(it.line() + 1, it.charIndex() + 1, lineStartPoint->mDocument));
			++lineStartPoint;
		}
		text += it.value();
		it = (lastIt = it).next();
	}
	if (it.isEndOfFile()) {
		emit error(tr("Expecting '\"' to end a string literal starting at %1").arg(CodePoint(start.column(), strLitStartLine).toString()), CodePoint(it.column(), lineStartPoint));
	}
	else {
		text += it.value();
		lastIt = it;
	}

	it = lastIt;
	Token *tok = createToken(insertAfterThis, Token::String, text, strLitStartLine, start.column());

	auto lineStartEndIt = lineStartPoint;++lineStartEndIt;

	for (auto lineStartIt = ++strLitStartLine;lineStartIt != lineStartEndIt; ++lineStartIt) {
		lineStartIt->mNextToken = tok;
	}

	return tok;
}

template <typename CharIterator>
Token *FrostCodeModel::createIdentifierToken(CharIterator &it, Token *insertAfterThis, QLinkedList<LineStartPoint>::Iterator &lineStartPoint) {

	CharIterator start = it;
	QString text = it.value();

	CharIterator lastIt = it;
	for (it = it.next(); !it.isEndOfFile() && (it.value().isLetterOrNumber() || it.value() == '_'); it = (lastIt = it).next()) {
		text += it.value();
	}


	Token::Type tokenType = resolveIdentifierTokenType(text);

	if (tokenType == Token::MultiLineComment) {
		it = start;
		return createMultiLineCommentToken(it, insertAfterThis, lineStartPoint);
	}

	it = lastIt;
	return createToken(insertAfterThis, tokenType, text, lineStartPoint, start.column());
}

template <typename CharIterator>
Token *FrostCodeModel::createHexToken(CharIterator &it, Token *insertAfterThis, QLinkedList<LineStartPoint>::Iterator &lineStartPoint) {

	int column = it.column();
	QString text = it.value();

	CharIterator lastIt = it;
	for (it = it.next(); !it.isEndOfFile() && isHexChar(it.value()); it = (lastIt = it).next()) {
		text += it.value();
	}


	it = lastIt;

	return createToken(insertAfterThis, Token::IntegerHex, text, lineStartPoint, column);
}

template <typename CharIterator>
Token *FrostCodeModel::createSingleLineCommentToken(CharIterator &it, Token *insertAfterThis, QLinkedList<LineStartPoint>::Iterator &lineStartPoint) {
	CharIterator start = it;
	QString text = it.value();
	CharIterator next = it.next();
	while (!next.isEndOfFile() && !next.isEndOfLine()) {
		text += next.value();
		it = next;
		next = next.next();
	}

	return createToken(insertAfterThis, Token::SingleLineComment, text, lineStartPoint, start.column());
}

template <typename CharIterator>
Token *FrostCodeModel::createMultiLineCommentToken(CharIterator &it, Token *insertAfterThis, QLinkedList<LineStartPoint>::Iterator &lineStartPoint) {
	QLinkedList<LineStartPoint>::Iterator startLineStartPoint = lineStartPoint;
	CharIterator start = it;
	QString text = it.value();
	CharIterator next = it.next();

	const char endRem[] = "endrem";
	int curIndex = 0;

	while (!next.isEndOfFile()) {
		text += next.value();
		if (next.isEndOfLine()) {
			mLineStartPoints.append(LineStartPoint(it.line() + 1, it.charIndex() + 1, lineStartPoint->mDocument));
			++lineStartPoint;
		}
		else if (next.value().toLower() == endRem[curIndex]) {
			++curIndex;
			if (curIndex == 6) {
				it = next;
				break;
			}
		}
		else {
			curIndex = 0;
		}

		it = next;
		next = next.next();
	}

	Token *tok = createToken(insertAfterThis, Token::MultiLineComment, text, startLineStartPoint, start.column());

	auto lineStartEndIt = lineStartPoint;++lineStartEndIt;
	for (auto lineStartIt = ++startLineStartPoint;lineStartIt != lineStartEndIt; ++lineStartIt) {
		lineStartIt->mNextToken = tok;
	}

	return tok;
}
