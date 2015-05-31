#include "frostcodemodel.hpp"
#include "document.hpp"
namespace Frost {
FrostCodeModel::FrostCodeModel() :
	mFirstToken(0),
	mLastToken(0)
{
	mKeywords["not"] = Token::opNot;
	mKeywords["or"] = Token::opOr;
	mKeywords["xor"] = Token::opXor;
	mKeywords["mod"] = Token::opMod;
	mKeywords["shr"] = Token::opShr;
	mKeywords["shl"] = Token::opShl;
	mKeywords["sar"] = Token::opSar;
	mKeywords["and"] = Token::opAnd;

	mKeywords["if"] = Token::kIf;
	mKeywords["then"] = Token::kThen;
	mKeywords["elseif"] = Token::kElseIf;
	mKeywords["else"] = Token::kElse;
	mKeywords["endif"] = Token::kEndIf;
	mKeywords["select"] = Token::kSelect;
	mKeywords["case"] = Token::kCase;
	mKeywords["default"] = Token::kDefault;
	mKeywords["endselect"] = Token::kEndSelect;
	mKeywords["function"] = Token::kFunction;
	mKeywords["return"] = Token::kReturn;
	mKeywords["endfunction"] = Token::kEndFunction;
	mKeywords["type"] = Token::kType;
	mKeywords["struct"] = Token::kStruct;
	mKeywords["field"] = Token::kField;
	mKeywords["endtype"] = Token::kEndType;
	mKeywords["endstruct"] = Token::kEndStruct;
	mKeywords["while"] = Token::kWhile;
	mKeywords["wend"] = Token::kWend;
	mKeywords["repeat"] = Token::kRepeat;
	mKeywords["forever"] = Token::kForever;
	mKeywords["until"] = Token::kUntil;
	mKeywords["goto"] = Token::kGoto;
	mKeywords["gosub"] = Token::kGosub;
	mKeywords["for"] = Token::kFor;
	mKeywords["to"] = Token::kTo;
	mKeywords["each"] = Token::kEach;
	mKeywords["step"] = Token::kStep;
	mKeywords["next"] = Token::kNext;
	mKeywords["dim"] = Token::kDim;
	mKeywords["redim"] = Token::kRedim;
	mKeywords["cleararray"] = Token::kClearArray;
	mKeywords["const"] = Token::kConst;
	mKeywords["global"] = Token::kGlobal;
	mKeywords["data"] = Token::kData;
	mKeywords["read"] = Token::kRead;
	mKeywords["restore"] = Token::kRestore;
	mKeywords["as"] = Token::kAs;
	mKeywords["exit"] = Token::kExit;
	mKeywords["end"] = Token::kEnd;

	mKeywords["new"] = Token::kNew;
	mKeywords["first"] = Token::kFirst;
	mKeywords["last"] = Token::kLast;
	mKeywords["before"] = Token::kBefore;
	mKeywords["after"] = Token::kAfter;
	mKeywords["delete"] = Token::kDelete;
	mKeywords["arraysize"] = Token::kArraySize;

	mKeywords["include"] = Token::kInclude;
}

FrostCodeModel::~FrostCodeModel()
{
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
}

void FrostCodeModel::parse()
{
	QString code = mMainDocument->toPlainText();


}

QList<Symbol *> FrostCodeModel::codeComplete(int position) const
{
	return QList<Symbol*>();
}

void FrostCodeModel::documentEdited(int position, int charsRemoved, int charsAdded)
{

}

void FrostCodeModel::documentAdded(const QString &filePath, Document *doc)
{

}

}
