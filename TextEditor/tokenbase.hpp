#ifndef TOKEN_BASE_HPP
#define TOKEN_BASE_HPP

#include "codepoint.hpp"
class TokenBase
{
	public:
		TokenBase(const QString &text, const CodePoint &cp, TokenBase *previousToken) : mText(text), mCodePoint(cp), mPreviousToken(previousToken) {
			if (previousToken->mNextToken) {
				previousToken->mNextToken->mPreviousToken = this;
				this->mNextToken = previousToken->mNextToken;
			}
			previousToken->mNextToken = this;
		}

		virtual ~TokenBase() {}
		const QString &text() const { return mText; }
		const CodePoint &codePoint() const { return mCodePoint; }
		TokenBase *next() const { return mNextToken; }
		TokenBase *previous() const { return mPreviousToken; }
		void setNext(TokenBase *n) { mNextToken = n; }
		void setPrevious(TokenBase *p) { mPreviousToken = p; }

		int length() const { return mText.length(); }
	protected:
		QString mText;
		CodePoint mCodePoint;
		TokenBase *mNextToken;
		TokenBase *mPreviousToken;
};

#endif // TOKEN_BASE_HPP

