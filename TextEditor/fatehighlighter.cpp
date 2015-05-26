#include "fatehighlighter.hpp"
#include "document.hpp"
#include <QRegularExpression>
#include <QDebug>
namespace Fate {



FateHighlighter::FateHighlighter(Document* parent):
	mProgress(0),
	mStructure(nullptr),
	Highlighter(qobject_cast<QTextDocument*>(parent)) {
	mLocalFmt.setForeground(QBrush(Qt::darkRed));
	mGlobalFmt.setForeground(QBrush(Qt::darkCyan));
	mStructureFmt.setForeground(QBrush(Qt::darkMagenta));
	mFunctionFmt.setForeground(QBrush(Qt::darkGreen));
	mFieldFmt.setForeground(QBrush(Qt::darkCyan));
}


Document* FateHighlighter::document() {
	return qobject_cast<Document*>(QSyntaxHighlighter::document());
}

void FateHighlighter::highlightBlock(const QString &text) {
	Highlighter::highlightBlock(text);

	Document* doc = document();



}


void FateHighlighter::testStructures(const QString& s, Document* doc) {

}

void FateHighlighter::testVariables(const QString& s, Document* doc) {

}

void FateHighlighter::testFunctions(const QString& s, Document* doc) {



}

}
