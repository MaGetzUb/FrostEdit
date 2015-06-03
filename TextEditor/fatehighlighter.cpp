#include "fatehighlighter.hpp"
#include "document.hpp"
#include <QRegularExpression>
#include <QDebug>
namespace Fate {
namespace Utils {
bool isSeparator(QChar c) {
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
}




FateHighlighter::FateHighlighter(Document* parent):
	mProgress(0),
	mStructure(nullptr),
	Highlighter(qobject_cast<QTextDocument*>(parent)) {
	mLocalFmt.setForeground(QBrush(Qt::darkRed));
	mGlobalFmt.setForeground(QBrush(Qt::darkCyan));
	mStructureFmt.setForeground(QBrush(Qt::darkMagenta));
	mFunctionFmt.setForeground(QBrush(Qt::darkGreen));
	mFieldFmt.setForeground(QBrush(Qt::darkCyan));
	mOperatorFmt.setForeground(QBrush(QColor(128, 128, 192)));
}


Document* FateHighlighter::document() {
	return qobject_cast<Document*>(QSyntaxHighlighter::document());
}

void FateHighlighter::setOperatorFmt(const QTextCharFormat& fmt) {
	mOperatorFmt = fmt;
}

void FateHighlighter::highlightBlock(const QString &text) {
	for(int i = 0; i < text.length(); i++) {
		if(Utils::isSeparator(text[i])) {
			setFormat(i, 1, mOperatorFmt);
		}
	}
	Highlighter::highlightBlock(text);


}



}

