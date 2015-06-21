#ifndef FROSTTOKEN_H
#define FROSTTOKEN_H
#include "codepoint.hpp"
#include <QString>
#include "tokenbase.hpp"

namespace Frost {
class Token : public TokenBase
{
	public:
		enum Type {
			EndOfTokens = 0,
			EOL,
			Integer,
			IntegerHex,
			Float,
			String,
			Identifier,
			Label,
			Colon, // :
			Comma,

			SingleLineComment,
			MultiLineComment,

			IntegerTypeMark, //%
			StringTypeMark, //$
			FloatTypeMark, //#

			LeftParenthese,
			RightParenthese,

			LeftSquareBracket,
			RightSquareBracket,
			//Operators

			OperatorsBegin,
			opEqual,
			opNotEqual,
			opGreater,
			opLess,
			opGreaterEqual,
			opLessEqual,
			opPlus,
			opMinus,
			opMultiply,
			opPower,
			opMod,
			opShl,
			opShr,
			opSar,
			opDivide,
			opAnd,
			opOr,
			opXor,
			opNot,
			opDot,  //   Dot
			OperatorsEnd,

			//Keywords
			KeywordsBegin,
			kIf,
			kThen,
			kElseIf,
			kElse,
			kEndIf,
			kSelect,
			kCase,
			kDefault,
			kEndSelect,
			kFunction,
			kReturn,
			kEndFunction,
			kType,
			kStruct,
			kField,
			kEndType,
			kEndStruct,
			kWhile,
			kWend,
			kRepeat,
			kForever,
			kUntil,
			kGoto,
			kGosub,
			kFor,
			kTo,
			kEach,
			kStep,
			kNext,
			kDim,
			kRedim,
			kClearArray,
			kConst,
			kGlobal,
			kData,
			kRead,
			kRestore,
			kAs,
			kInclude,
			kExit,
			kEnd,

			kNew,
			kFirst,
			kLast,
			kBefore,
			kAfter,
			kDelete,
			kArraySize,

			KeywordsEnd,
			TypeCount
		};
		Token(Type type, const QString &text, const CodePoint &cp, Token *previousToken);
		~Token();

		Type type() const { return mType; }
		const QString &text() const { return mText; }
		const CodePoint &codePoint() const { return mCodePoint; }
		Token *next() const { return static_cast<Token*>(mNextToken); }
		Token *previous() const { return static_cast<Token*>(mPreviousToken); }
	private:
		Type mType;
};

}

#endif // FROSTTOKEN_H
