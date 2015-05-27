#include "frosttoken.hpp"
namespace Frost {

Token::Token(Token::Type type, const QString &text, const CodePoint &cp, Token *previousToken) :
	mType(type),
	mText(text),
	mCodePoint(cp),
	mPreviousToken(previousToken)
{
	previousToken->mNextToken = this;
}

Token::~Token() {

}

}

