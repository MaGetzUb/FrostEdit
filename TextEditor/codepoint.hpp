#ifndef CODEPOINT
#define CODEPOINT
#include <QLinkedList>
#include <QString>
class Document;



class TokenBase;
struct LineStartPoint {
	LineStartPoint() : mLineNumber(0), mCharIndex(0), mDocument(0), mNextToken(0) {}
	LineStartPoint(int lineNum, int charIndex, Document *doc, TokenBase *tok = 0) : mLineNumber(lineNum), mCharIndex(charIndex), mDocument(doc), mNextToken(tok) { }
	int mLineNumber;
	int mCharIndex;
	Document *mDocument;
	TokenBase *mNextToken;
};

class CodePoint {
	public:
		CodePoint() : mColumn(0), mLineStartPoint(){}
		CodePoint(int column, QLinkedList<LineStartPoint>::Iterator line) : mColumn(column), mLineStartPoint(line) {}
		~CodePoint() {}

		int column() const { return mColumn; }
		int line() const { return mLineStartPoint->mLineNumber; }
		Document *document() const { return mLineStartPoint->mDocument; }

		int charIndex() const { return mLineStartPoint->mCharIndex + mColumn - 1; }


		QString toString() const;

		QLinkedList<LineStartPoint>::Iterator lineStartPoint() const;

private:
		int mColumn;
		QLinkedList<LineStartPoint>::Iterator mLineStartPoint;
};

#endif // CODEPOINT

