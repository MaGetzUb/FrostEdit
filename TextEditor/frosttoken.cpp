#include "frosttoken.hpp"
namespace Frost {

Token::Token(Token::Type type, const QString &text, const CodePoint &cp, Token *previousToken) :
	TokenBase(text, cp, previousToken),
	mType(type)
{

}

Token::~Token() {
}

}

