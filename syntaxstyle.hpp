#ifndef SYNTAXSTYLE_HPP
#define SYNTAXSTYLE_HPP

#include <QtXml>
#include <QTextCharFormat>
#include "TextEditor/qate/defaultcolors.h"
#include "TextEditor/fatehighlighter.hpp"

#include "console.hpp"

class TextEdit;
class SyntaxStyle {

	QTextCharFormat mBasicTextFormat;
	QTextCharFormat mKeywordFormat;
	QTextCharFormat mDataTypeFormat;
	QTextCharFormat mCharFormat;
	QTextCharFormat mStringFormat;
	QTextCharFormat mCommentFormat;
	QTextCharFormat mAlertFormat;
	QTextCharFormat mErrorFormat;
	QTextCharFormat mFunctionFormat;
	QTextCharFormat mRegionMarkerFormat;
	QTextCharFormat mOthersFormat;
	QTextCharFormat mOperator;
	QTextCharFormat mNumber;
	QTextCharFormat mSelection;
	//TextEdit colors:
	QTextCharFormat mLineNumber;
	QTextCharFormat mSelectedLineNumber;
	QTextCharFormat mSimilarOccurance;
	QTextCharFormat mRegionVisualizer;
	QTextCharFormat mRegionVisualizerSelected;
	//Console colors
	QTextCharFormat mStdErr;
	QTextCharFormat mStdOut;


	QString mFile;
	QDomDocument mXMLDoc;

	public:

		SyntaxStyle();
		SyntaxStyle(const QString&);

		void loadDefaults();
		bool load(const QString&);
		bool save(const QString&);
		void readAttribute(const QString&);


		void applyToHighlighter(TextEditor::Internal::Highlighter* hiltter);
		void applyToTextEdit(TextEdit* edit);
		void applyToConsole(Console* console);
	private:
		void readScheme(QDomElement element);
		QTextCharFormat* formatByName(const QString&);
};

#endif // SYNTAXSTYLE_HPP
