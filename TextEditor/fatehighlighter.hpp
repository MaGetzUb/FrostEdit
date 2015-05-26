#ifndef HIGHLIGHTER_HPP
#define HIGHLIGHTER_HPP

#include "qate/highlighter.h"
#include "qate/qateblockdata.h"
#include "qate/highlighter.h"
#include "fatedata.hpp"
#include <QStringRef>
class Document;
namespace Fate {

namespace Utils {

	static inline bool isSpace(QChar c) {return c == ' ' || c == '\t';}
    static inline bool isLetter(QChar c) {return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');}
	static inline bool isUnderScore(QChar c) {return c == '_';}
	static inline bool isDigit(QChar c) {return c >= '0' && c <= '9';}
	static inline bool isSeparator(QChar c) {
			if(c == '.')
				return true;
			else if(c == '(')
				return true;
			else if(c == ')')
				return true;
			else if(c == ':')
				return true;
			else if(c == '!')
				return true;
			else if(c == '+')
				return true;
			else if(c == ',')
				return true;
			else if(c == '-')
				return true;
			else if(c == '<')
				return true;
			else if(c == '=')
				return true;
			else if(c == '>')
				return true;
			else if(c == '%')
				return true;
			else if(c == '&')
				return true;
			else if(c == '/')
				return true;
			else if(c == ';')
				return true;
			else if(c == '?')
				return true;
			else if(c == '[')
				return true;
			else if(c == ']')
				return true;
			else if(c == '^')
				return true;
			else if(c == '{')
				return true;
			else if(c == '|')
				return true;
			else if(c == '}')
				return true;
			else if(c == '~')
				return true;
			else if(c == '\\')
				return true;
			else if(c == '*')
				return true;
			else if(c == ',')
				return true;
			else
				return false;
	}

	static inline bool letterCmp(QChar a, QChar b, bool underscores, bool sensitive = false) {
		if(underscores)
			if(a == '_' && a == b)
				return true;

		if(sensitive)
			return a==b;
		else
			return a.toLower() == b.toLower();
		return false;
	}
	static inline bool charCmp(QChar a, QChar b, bool sensitive = false) {
		bool lettercmp = isLetter(a) && isLetter(b);
		if(lettercmp)
			return letterCmp(a, b, sensitive);
		else
			return a == b;
		return false;
	}
	static inline void skipSpaces(const QString& ref, int& pos) {
		while(isSpace(ref[pos])) {
			pos++;
		}
	}
	static inline void skipSeparators(const QString& ref, int& pos) {
		while(isSeparator(ref[pos]) || isSpace(ref[pos]))
			pos++;
	}



	static bool stringEquals(const QString& ref, const QString& cmp, bool caseSensitive, int& pos) {
		int cpos = 0;
		while(charCmp(ref[pos+cpos], cmp[cpos], caseSensitive) && pos < ref.length() && cpos < cmp.length()) {
			cpos++;
		}
		if(cpos != cmp.length())
			return false;
		else {
			pos+=cpos;
			return true;
		}
	}

	static bool wordEquals(const QString& ref, const QString& cmp, bool underscores, bool caseSensitive, int& pos) {
		int cpos = 0;
		while(letterCmp(ref[pos+cpos], cmp[cpos], underscores, caseSensitive) && pos < ref.length() && cpos < cmp.length()) {
			cpos++;
		}
		if(cpos != cmp.length())
			return false;
		else {
			pos+=cpos;
			return true;
		}
	}

	static inline QString nextIdentifier(const QString& ref, int& pos, int& start, int& length) {
		QString out("");
		skipSeparators(ref, pos);
		if(!(isLetter(ref[pos]) || isUnderScore(ref[pos])))
			return out;
		start = pos;
		while(pos < ref.length() && isLetter(ref[pos]) || isUnderScore(ref[pos]) || isDigit(ref[pos])) {
			out.append(ref[pos]);
			pos++;
		}
		length = pos - start;
		return out;
	}

	static QString nextWord(const QString& ref, bool underscores, int& pos, int& start, int length) {
		QString out;
		skipSeparators(ref, pos);
		start = pos;
		while(pos < ref.length() && isLetter(ref[pos]) || (underscores && isUnderScore(pos))) {
			out.append(ref[pos]);
			pos++;
		}
		length = pos - start;
		return out;
	}

}



using namespace TextEditor::Internal;




class FateHighlighter : public Highlighter {
	Q_OBJECT

	QTextCharFormat mLocalFmt;
	QTextCharFormat mGlobalFmt;
	QTextCharFormat mStructureFmt;
	QTextCharFormat mFieldFmt;
	QTextCharFormat mFunctionFmt;
	IdentStructure* mStructure;

	void testStructures(const QString&, Document* doc);
	void testVariables(const QString&, Document* doc);
	void testFunctions(const QString&, Document* doc);


	int mProgress;


	protected:


	bool mMultilineCommentStarted;
	public:
		FateHighlighter(Document* parent = 0);
		Document* document();

	protected:
		virtual void highlightBlock(const QString &text);

};

}
#endif // HIGHLIGHTER_HPP
